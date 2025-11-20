/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ipset.h"
#include "log.h"
#include "mosquitto.h"

static const int mqtt_keepalive = 60;

static struct topic parse_topic(char *topic)
{
    struct topic parsed_topic;

    parsed_topic.action = malloc(4);
    parsed_topic.name = malloc(strlen(topic) + 1);

    if (strchr(topic, '/') != NULL) {
        strcpy(parsed_topic.name, strsep(&topic, "/"));
        strcpy(parsed_topic.action, topic);
    } else {
        strcpy(parsed_topic.name, topic);
        strcpy(parsed_topic.action, "add");
    }

    return parsed_topic;
}

static int set_will(struct mosquitto *m)
{
    return mosquitto_will_set(m, MQTT_WILL_TOPIC, strlen(mqttconn.cid), mqttconn.cid, MQTT_WILL_QOS, MQTT_WILL_RETAIN);
}

static void cb_con(struct mosquitto *m, void *userdata, int result)
{
    unsigned int t = 0;

    (void) userdata;
    if (!result) {
        char *topic = NULL;
        pr_info("Connected to %s:%d using CID %s\n", mqttconn.host, mqttconn.port, mqttconn.cid);
        for (t = 0; t < mqttconn.ntopics; t++) {
            topic = malloc(strlen(mqttconn.topics[t]) + 3);
            strcpy(topic, mqttconn.topics[t]);
            strcat(topic, "/#");
            if (mosquitto_subscribe(m, NULL, topic, 2) == MOSQ_ERR_SUCCESS) {
                pr_info("Subscribed to topic %s", topic);
            }
            free(topic);
        }
    }
}

static void cb_msg(struct mosquitto *m, void *userdata, const struct mosquitto_message *msg)
{
    (void) m;
    (void) userdata;
    if (msg->payloadlen) {
        struct topic parsed_topic = parse_topic(msg->topic);
        if (strcmp(parsed_topic.action, "add") == 0) {
            ipset_add(parsed_topic.name, msg->payload);
        } else if (strcmp(parsed_topic.action, "del") == 0) {
            ipset_del(parsed_topic.name, msg->payload);
        }
        free(parsed_topic.action);
        free(parsed_topic.name);
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
    int keepalive = mqtt_keepalive, ret;
    struct mosquitto *m = NULL;

    gen_cid(&mqttconn.cid[0]);

    mosquitto_lib_init();

    m = mosquitto_new(mqttconn.cid, clean_session, NULL);

    mosquitto_connect_callback_set(m, cb_con);
    mosquitto_message_callback_set(m, cb_msg);
    mosquitto_username_pw_set(m, mqttconn.username, mqttconn.password);

#ifdef WITH_TLS
    if (mqttconn.tls) {
        mosquitto_tls_set(m, mqttconn.cafile, NULL, NULL, NULL, NULL);
    }
#endif
    ret = set_will(m);
    if (ret) {
        pr_err("Failed to set LWT: %d\n", ret);
    }
    mosquitto_connect(m, mqttconn.host, mqttconn.port, keepalive);

    mosquitto_loop_forever(m, -1, 1);

    mosquitto_destroy(m);
    mosquitto_lib_cleanup();
    return 0;
}
