/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

extern char *mqtt_host;
extern char **mqtt_topics;
extern int mqtt_port;
extern unsigned int ntopics;

int init_mqtt();
