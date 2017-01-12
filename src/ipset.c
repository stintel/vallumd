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

#include <libipset/session.h>
#include <libipset/types.h>
#include <libipset/ui.h>

#include "log.h"

int ipset_add(char *set, char *elem)
{
    const struct ipset_type *type = NULL;
    enum ipset_cmd cmd = IPSET_CMD_ADD;
    int ret = 0;
    struct ipset_session *sess;

    ipset_load_types();

    sess = ipset_session_init(printf);
    if (sess == NULL) {
        pr_err("ipset: failed to initialize session\n");
        return 1;
    }

    ret = ipset_parse_setname(sess, IPSET_SETNAME, set);
    if (ret < 0) {
        pr_err("ipset: %s\n", ipset_session_error(sess));
        return 1;
    }

    type = ipset_type_get(sess, cmd);
    if (type == NULL) {
        pr_err("ipset: %s\n", ipset_session_error(sess));
        return 1;
    }

    ret = ipset_parse_elem(sess, type->last_elem_optional, elem);
    if (ret < 0) {
        pr_err("ipset: %s\n", ipset_session_error(sess));
        return 1;
    }

    ret = ipset_cmd(sess, cmd, 0);
    if (ret < 0) {
        pr_err("ipset: %s\n", ipset_session_error(sess));
        return 1;
    }

    ipset_session_fini(sess);

    pr_info("ipset: added %s to %s\n", elem, set);

    return 0;
}
