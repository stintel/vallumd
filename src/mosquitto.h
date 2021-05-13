/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifdef WITH_TLS
#include <stdbool.h>
extern bool mqtt_tls;
extern char *mqtt_cafile;
#endif
extern char *mqtt_host;
extern char **mqtt_topics;
extern char *mqtt_username;
extern char *mqtt_password;
extern int mqtt_port;
extern unsigned int ntopics;

int init_mqtt();

struct topic {
    char *action;
    char *name;
};
