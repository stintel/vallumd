/* SPDX-License-Identifier: GPL-3.0 */
/*
 * Copyright (C) 2017-2021  Stijn Tintel <stijn@linux-ipv6.be>
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "log.h"
#include "mosquitto.h"

static void print_usage(void)
{
    printf("Usage: -h host [-p port] [-u username] [-P password] -t topic1 [-t topicN]\n");
    printf(" -h: MQTT host to connect to\n");
    printf(" -p: MQTT port to connect to (1883)\n");
    printf(" -u: MQTT username\n");
    printf(" -P: MQTT password\n");
    printf(" -t: MQTT topic and IPset name\n");
    printf(" -V: print version number and exit\n");
#ifdef WITH_TLS
    printf("TLS options:\n");
    printf(" -c: path to CA file\n");
    printf(" -T: use TLS\n");
#endif
}

int main(int argc, char **argv)
{
#ifdef WITH_TLS
    bool tls = false;
    char *cafile = NULL;
#endif
    char *host = NULL;
    char *username = NULL;
    char *password = NULL;
    int opt = 0;
    unsigned int port = 1883;
    unsigned int t = 0;

    openlog("vallumd", LOG_PID, LOG_DAEMON);

    while ((opt = getopt(argc, argv, "c:h:p:P:t:Tu:V")) != -1) {
        if (opt == 't') {
            ntopics++;
        }
    }

    mqtt_topics = malloc(ntopics * sizeof(*mqtt_topics));

    optind = 0;
    while ((opt = getopt(argc, argv, "c:h:p:P:t:Tu:V")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'P':
                password = optarg;
                break;
            case 't':
                mqtt_topics[t] = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(mqtt_topics[t], optarg);
                t++;
                break;
#ifdef WITH_TLS
            case 'c':
                cafile = optarg;
                break;
            case 'T':
                tls = true;
                break;
#endif
            case 'u':
                username = optarg;
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

#ifdef WITH_TLS
    if (tls) {
        if (cafile) {
            if (access(cafile, F_OK) == -1) {
                fprintf(stderr, "CA file %s does not exist.\n", cafile);
                return 1;
            }
        } else {
            fprintf(stderr, "TLS operation requires a CA file.\n");
            return 1;
        }
    }
#endif

    mqtt_host = host;
    mqtt_port = port;
    mqtt_username = username;
    mqtt_password = password;
#ifdef WITH_TLS
    mqtt_cafile = cafile;
    mqtt_tls = tls;
#endif

    init_mqtt();

    return 0;
}
