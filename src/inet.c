/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <arpa/inet.h>
#include <string.h>

int get_inet_family(char *ipaddr)
{
    unsigned char dst[sizeof(struct in6_addr)];
    unsigned int family = 0;
    unsigned int ret = 0;

    family = strchr(ipaddr, '.') ? AF_INET : AF_INET6;

    ret = inet_pton(family, ipaddr, dst);

    if (ret == 1) {
        return family;
    } else {
        return AF_UNSPEC;
    }
}

int ip_valid(char *ipaddr)
{
    return get_inet_family(ipaddr) != AF_UNSPEC;
}
