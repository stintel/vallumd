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

struct mqtt_conn mc = {
    .port = 1883,
    .ntopics = 0,
#ifdef WITH_TLS
    .tls = false,
#endif
};

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
    int opt = 0;
    unsigned int t = 0;

    openlog("vallumd", LOG_PID, LOG_DAEMON);

    while ((opt = getopt(argc, argv, "c:h:p:P:t:Tu:V")) != -1) {
        if (opt == 't') {
            mc.ntopics++;
        }
    }

    mc.topics = malloc(mc.ntopics * sizeof(*mc.topics));

    optind = 0;
    while ((opt = getopt(argc, argv, "c:h:p:P:t:Tu:V")) != -1) {
        switch (opt) {
            case 'h':
                mc.host = optarg;
                break;
            case 'p':
                mc.port = atoi(optarg);
                break;
            case 'P':
                mc.password = optarg;
                break;
            case 't':
                mc.topics[t] = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(mc.topics[t], optarg);
                t++;
                break;
#ifdef WITH_TLS
            case 'c':
                mc.cafile = optarg;
                break;
            case 'T':
                mc.tls = true;
                break;
#endif
            case 'u':
                mc.username = optarg;
                break;
            case 'V':
                fprintf(stdout, "vallumd-%s\n", VERSION);
                return 0;
            default:
                print_usage();
                return 1;
        }
    }

    if (mc.host == NULL || mc.ntopics == 0) {
        print_usage();
        return 1;
    }

#ifdef WITH_TLS
    if (mc.tls) {
        if (mc.cafile) {
            if (access(mc.cafile, F_OK) == -1) {
                fprintf(stderr, "CA file %s does not exist.\n", mc.cafile);
                return 1;
            }
        } else {
            fprintf(stderr, "TLS operation requires a CA file.\n");
            return 1;
        }
    }
#endif

    init_mqtt();

    return 0;
}
