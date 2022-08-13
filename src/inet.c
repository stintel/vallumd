/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <arpa/inet.h>
#include <string.h>

extern int ip_valid(char *ipaddr)
{
    unsigned char dst[sizeof(struct in6_addr)];
    unsigned int family = 0;
    unsigned int ret = 0;

    family = strchr(ipaddr, '.') ? AF_INET : AF_INET6;

    ret = inet_pton(family, ipaddr, dst);
    return ret != 0;
}
