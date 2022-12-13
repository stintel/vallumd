/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <errno.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>
#include <libmnl/libmnl.h>
#include <libnftnl/set.h>
#include <linux/netfilter/nf_tables.h>

#include "inet.h"
#include "log.h"
#include "nftnl.h"

static struct nftnl_set *nftnl_init_set(const char *tbl, const char *name)
{
    struct nftnl_set *ns;

    ns = nftnl_set_alloc();
    if (ns == NULL) {
        errno = -ENOMEM;
        return NULL;
    }

    nftnl_set_set_str(ns, NFTNL_SET_TABLE, tbl);
    nftnl_set_set_str(ns, NFTNL_SET_NAME, name);

    return ns;
}

static int nftnl_init_elem(struct nftnl_set *ns, const void *elem, uint8_t af)
{
    size_t len;
    struct nftnl_set_elem *nse;

    nse = nftnl_set_elem_alloc();
    if (nse == NULL) {
        errno = -ENOMEM;
        return errno;
    }

    switch (af) {
        case AF_INET:
            len = sizeof(struct in_addr);
            break;
        case AF_INET6:
            len = sizeof(struct in6_addr);
            break;
        default:
            return -EINVAL;
    }

    nftnl_set_elem_set(nse, NFTNL_SET_ELEM_KEY, elem, len);
    nftnl_set_elem_add(ns, nse);

    return 0;
}

static int nftnl_send(char *set, char *elem, uint16_t type)
{
    char buf[MNL_SOCKET_BUFFER_SIZE];
    int fam, ret;
    struct mnl_nlmsg_batch *batch;
    struct mnl_socket *nl;
    struct nftnl_set *ns;
    struct nlmsghdr *nlh;
    uint16_t flags = NLM_F_ACK;
    uint32_t portid, seq;
    unsigned char addr[sizeof(struct in6_addr)];

    if (type == NFT_MSG_NEWSETELEM) {
        flags |= NLM_F_CREATE | NLM_F_REPLACE;
    }

    fam = get_inet_family(elem);
    ret = inet_pton(fam, elem, addr);
    if (ret != 1) {
        return ret;
    }

    ns = nftnl_init_set(nc.table, set);
    ret = nftnl_init_elem(ns, addr, fam);
    if (ret) {
        return ret;
    }

    seq = time(NULL);

    batch = mnl_nlmsg_batch_start(buf, sizeof(buf));
    nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
    mnl_nlmsg_batch_next(batch);

    nlh = nftnl_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch), type, nc.family, flags, seq++);
    nftnl_set_elems_nlmsg_build_payload(nlh, ns);
    nftnl_set_free(ns);
    mnl_nlmsg_batch_next(batch);
    nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
    mnl_nlmsg_batch_next(batch);

    nl = mnl_socket_open(NETLINK_NETFILTER);
    if (nl == NULL) {
        return -1;
    }

    if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
        return -1;
    }

    portid = mnl_socket_get_portid(nl);

    if (mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch), mnl_nlmsg_batch_size(batch)) < 0) {
        return -1;
    }

    mnl_nlmsg_batch_stop(batch);
    ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
    while (ret > 0) {
        ret = mnl_cb_run(buf, ret, 0, portid, NULL, NULL);
        if (ret <= 0) {
            break;
        }
        ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
    }

    if (ret == -1) {
        return ret;
    }
    mnl_socket_close(nl);

    return 0;
}

int nftnl_add(char *set, char *elem)
{
    int ret;
    uint16_t type;

    type = NFT_MSG_NEWSETELEM;
    ret = nftnl_send(set, elem, type);
    if (!ret) {
        pr_info("nftnl: added %s to %s\n", elem, set);
    } else {
        pr_err("nftnl: failed to add %s to %s: %s\n", elem, set, strerror(ret));
    }

    return ret;
}

int nftnl_del(char *set, char *elem)
{
    int ret;
    uint16_t type;

    type = NFT_MSG_DELSETELEM;
    ret = nftnl_send(set, elem, type);
    if (!ret) {
        pr_info("nftnl: deleted %s from %s\n", elem, set);
    } else {
        pr_err("nftnl: failed to remove %s from %s: %s\n", elem, set, strerror(ret));
    }

    return ret;
}