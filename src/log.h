/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <syslog.h>

#define pr_err(fmt, ...)    syslog(LOG_ERR, fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)   syslog(LOG_INFO, fmt, ##__VA_ARGS__)
