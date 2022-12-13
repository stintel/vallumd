/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2022  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifndef NFTNL_H_INCLUDED
#define NFTNL_H_INCLUDED

#include <inttypes.h>

int nftnl_add(char *set, char *elem);
int nftnl_del(char *set, char *elem);

struct nft_conf {
    uint16_t family;
    char *table;
};

extern struct nft_conf nc;

#endif /* NFTNL_H_INCLUDED */
