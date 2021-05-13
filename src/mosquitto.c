/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ipset.h"
#include "log.h"

char *mqtt_host;
char **mqtt_topics;
int mqtt_port;
unsigned int ntopics;

static void cb_con(struct mosquitto *m, void *userdata, int result)
{
    unsigned int t = 0;

    (void) userdata;
    if (!result) {
        pr_info("Connected to %s:%d\n", mqtt_host, mqtt_port);
        for (t = 0; t < ntopics; t++) {
            if(mosquitto_subscribe(m, NULL, mqtt_topics[t], 2) == MOSQ_ERR_SUCCESS) {
                pr_info("Subscribed to topic %s", mqtt_topics[t]);
            }
        }
    }
}

static void cb_msg(struct mosquitto *m, void *userdata, const struct mosquitto_message *msg)
{
    (void) m;
    (void) userdata;
    if (msg->payloadlen) {
        ipset_add(msg->topic, msg->payload);
    }
}

static void gen_cid(char *mqtt_cid)
{
    char hostname[16];

    gethostname(hostname, 15);
    snprintf(mqtt_cid, MOSQ_MQTT_ID_MAX_LENGTH, "%s-%d", hostname, getpid());
}

int init_mqtt(void)
{
    bool clean_session = true;
    char mqtt_cid[MOSQ_MQTT_ID_MAX_LENGTH];
    int keepalive = 60;
    struct mosquitto *m = NULL;

    gen_cid(&mqtt_cid[0]);

    mosquitto_lib_init();

    m = mosquitto_new(mqtt_cid, clean_session, NULL);

    mosquitto_connect_callback_set(m, cb_con);
    mosquitto_message_callback_set(m, cb_msg);

    mosquitto_connect(m, mqtt_host, mqtt_port, keepalive);

    mosquitto_loop_forever(m, -1, 1);

    mosquitto_destroy(m);
    mosquitto_lib_cleanup();
    return 0;
}
