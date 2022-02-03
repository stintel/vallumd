/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <arpa/inet.h>
#include <string.h>

extern int ip_valid(char *ipaddr)
{
    unsigned int family = 0;
    unsigned int ret = 0;
    struct sockaddr_in sa;

    family = strchr(ipaddr, '.') ? AF_INET : AF_INET6;

    ret = inet_pton(family, ipaddr, &(sa.sin_addr));
    return ret != 0;
}
