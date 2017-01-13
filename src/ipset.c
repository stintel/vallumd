/*
 * This file is part of vallumd.
 *
 * Copyright (C) 2017  Stijn Tintel <stijn@linux-ipv6.be>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

#include "log.h"

static int exit_error(int e, struct ipset_session *sess)
{
#ifdef WITH_LIBIPSET_V6_COMPAT
    pr_err("ipset: %s\n", ipset_session_error(sess));
#else
    pr_err("ipset: %s\n", ipset_session_report_msg(sess));
#endif
    ipset_session_fini(sess);

    return e;
}

static int ip_valid(char *ipaddr)
{
    unsigned int family = 0;
    unsigned int ret = 0;
    struct sockaddr_in sa;

    family = strchr(ipaddr, '.') ? AF_INET : AF_INET6;

    ret = inet_pton(family, ipaddr, &(sa.sin_addr));
    return ret != 0;
}

int ipset_do(int c, char *set, char *elem)
{
    const struct ipset_type *type = NULL;
    enum ipset_cmd cmd = c;
    int ret = 0;
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

#ifdef WITH_LIBIPSET_V6_COMPAT
    ret = ipset_envopt_parse(sess, IPSET_ENV_EXIST, NULL);
    if (ret < 0) {
        return exit_error(1, sess);
    }
#else
    ipset_envopt_set(sess, IPSET_ENV_EXIST);
#endif

    ret = ipset_parse_setname(sess, IPSET_SETNAME, set);
    if (ret < 0) {
        return exit_error(1, sess);
    }

    type = ipset_type_get(sess, cmd);
    if (type == NULL) {
        return exit_error(1, sess);
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

int ipset_add(char *set, char *elem)
{
    int ret = ipset_do(IPSET_CMD_ADD, set, elem);
    if (ret == 0) {
        pr_info("ipset: added %s to %s\n", elem, set);
    }

    return ret;
}
