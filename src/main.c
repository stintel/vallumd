/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "mosquitto.h"

static void print_usage(void)
{
    printf("Usage: -h host [-p port] -t topic1 [-t topicN]\n");
    printf(" -h: MQTT host to connect to\n");
    printf(" -p: MQTT port to connect to (1883)\n");
    printf(" -t: MQTT topic and IPset name\n");
    printf(" -V: print version number and exit\n");
}

int main(int argc, char **argv)
{
    char *host = NULL;
    int opt = 0;
    unsigned int port = 1883;
    unsigned int t = 0;

    openlog("vallumd", LOG_PID, LOG_DAEMON);

    while ((opt = getopt(argc, argv, "h:p:t:V")) != -1) {
        if (opt == 't') {
            ntopics++;
        }
    }

    mqtt_topics = malloc(ntopics * sizeof(*mqtt_topics));

    optind = 0;
    while ((opt = getopt(argc, argv, "h:p:t:V")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                mqtt_topics[t] = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(mqtt_topics[t], optarg);
                t++;
                break;
            case 'V':
                fprintf(stdout, "vallumd-%s\n", VERSION);
                return 0;
            default:
                print_usage();
                return 1;
        }
    }

    if (host == NULL || ntopics == 0) {
        print_usage();
        return 1;
    }

    mqtt_host = host;
    mqtt_port = port;

    init_mqtt();

    return 0;
}
