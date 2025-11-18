/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifndef IPSET_H_INCLUDED
#define IPSET_H_INCLUDED

#include <libipset/types.h>

extern int ipset_add(char *set, char *elem);
extern int ipset_del(char *set, char *elem);

#endif /* IPSET_H_INCLUDED */
