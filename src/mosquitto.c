/*
 * This file is part of vallumd.
 *
 * Copyright (C) 2017  Stijn Tintel <stijn@linux-ipv6.be>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ipset.h"
#include "log.h"

char *mqtt_host;
char **mqtt_topics;
char *mqtt_username;
char *mqtt_password;
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

int init_mqtt()
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
    mosquitto_username_pw_set(m, mqtt_username, mqtt_password);

    mosquitto_connect(m, mqtt_host, mqtt_port, keepalive);

    mosquitto_loop_forever(m, -1, 1);

    mosquitto_destroy(m);
    mosquitto_lib_cleanup();
    return 0;
}
