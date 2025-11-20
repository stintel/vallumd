/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <arpa/inet.h>
#include <string.h>

int get_inet_family(const char *ipaddr)
{
    int family = 0;
    unsigned char dst[sizeof(struct in6_addr)];
    unsigned int ret = 0;

    family = strchr(ipaddr, '.') ? AF_INET : AF_INET6;

    ret = inet_pton(family, ipaddr, dst);

    if (ret == 1) {
        return family;
    }

    return AF_UNSPEC;
}

int ip_valid(const char *ipaddr)
{
    return get_inet_family(ipaddr) != AF_UNSPEC;
}
