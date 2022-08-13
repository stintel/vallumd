/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifndef NFTNL_H_INCLUDED
#define NFTNL_H_INCLUDED

int nftnl_add(char *set, char *elem);
int nftnl_del(char *set, char *elem);

#endif /* NFTNL_H_INCLUDED */
