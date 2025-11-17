/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#ifndef MOSQUITTO_H_INCLUDED
#define MOSQUITTO_H_INCLUDED

#define MQTT_WILL_QOS    2
#define MQTT_WILL_RETAIN true
#define MQTT_WILL_TOPIC  "vallumd/will"

#ifdef WITH_TLS
#include <stdbool.h>
#endif

#include <mosquitto.h>

int init_mqtt();

struct mqtt_conn {
    char *host;
    char **topics;
    char *username;
    char *password;
    char cid[MOSQ_MQTT_ID_MAX_LENGTH];
    int port;
    unsigned int ntopics;
#ifdef WITH_TLS
    bool tls;
    char *cafile;
#endif
};

struct topic {
    char *action;
    char *name;
};

extern struct mqtt_conn mc;

#endif /* MOSQUITTO_H_INCLUDED */
