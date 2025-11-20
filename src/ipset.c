/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <arpa/inet.h>
#include <libipset/session.h>
#include <libipset/types.h>
#include <string.h>

#ifdef WITH_LIBIPSET_V6_COMPAT
#include <libipset/ui.h>
#else
#include <libipset/ipset.h>
#endif

#include "inet.h"
#include "log.h"

static int exit_error(int err, struct ipset_session *sess)
{
#ifdef WITH_LIBIPSET_V6_COMPAT
    pr_err("ipset: %s\n", ipset_session_error(sess));
#else
    pr_err("ipset: %s\n", ipset_session_report_msg(sess));
#endif
    ipset_session_fini(sess);

    return err;
}

static int ipset_do(int command, const char *set, const char *elem)
{
    const struct ipset_type *type = NULL;
    enum ipset_cmd cmd = command;
    int family = 0, ret = 0;
    struct ipset_session *sess;

    if (!ip_valid(elem)) {
        pr_err("ipset: %s is not a valid IP address", elem);
        return 1;
    }

    ipset_load_types();

#ifdef WITH_LIBIPSET_V6_COMPAT
    sess = ipset_session_init(printf);
#else
    sess = ipset_session_init(NULL, NULL);
#endif
    if (sess == NULL) {
        pr_err("ipset: failed to initialize session\n");
        return 1;
    }

    if (cmd == IPSET_CMD_ADD) {
#ifdef WITH_LIBIPSET_V6_COMPAT
        ret = ipset_envopt_parse(sess, IPSET_ENV_EXIST, NULL);
        if (ret < 0) {
            return exit_error(1, sess);
        }
#else
        ipset_envopt_set(sess, IPSET_ENV_EXIST);
#endif
    }

    ret = ipset_parse_setname(sess, IPSET_SETNAME, set);
    if (ret < 0) {
        return exit_error(1, sess);
    }

    type = ipset_type_get(sess, cmd);
    if (type == NULL) {
        return exit_error(1, sess);
    }

    family = ipset_data_family(ipset_session_data(sess));
    if (family != get_inet_family(elem)) {
        pr_err("ipset: address %s does not match family of set %s", elem, set);
        return 1;
    }

    ret = ipset_parse_elem(sess, type->last_elem_optional, elem);
    if (ret < 0) {
        return exit_error(1, sess);
    }

    ret = ipset_cmd(sess, cmd, 0);
    if (ret < 0) {
        return exit_error(1, sess);
    }

    ipset_session_fini(sess);

    return 0;
}

int ipset_add(const char *set, const char *elem)
{
    int ret = ipset_do(IPSET_CMD_ADD, set, elem);
    if (ret == 0) {
        pr_info("ipset: added %s to %s\n", elem, set);
    }

    return ret;
}

int ipset_del(const char *set, const char *elem)
{
    int ret = ipset_do(IPSET_CMD_DEL, set, elem);
    if (ret == 0) {
        pr_info("ipset: deleted %s from %s\n", elem, set);
    }

    return ret;
}
