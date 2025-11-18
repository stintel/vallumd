/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifndef INET_H_INCLUDED
#define INET_H_INCLUDED

extern int get_inet_family(const char *ipaddr);
extern int ip_valid(const char *ipaddr);

#endif /* INET_H_INCLUDED */
