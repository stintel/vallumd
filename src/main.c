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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mosquitto.h"

static void print_usage()
{
    printf("Usage: -h host [-p port] [-u username] [-P password] -t topic1 [-t topicN]\n");
    printf(" -h: MQTT host to connect to\n");
    printf(" -p: MQTT port to connect to (1883)\n");
    printf(" -u: MQTT username\n");
    printf(" -P: MQTT password\n");
    printf(" -t: MQTT topic and IPset name\n");
    printf(" -V: print version number and exit\n");
}

int main(int argc, char **argv)
{
    char *host = NULL;
    char *username = NULL;
    char *password = NULL;
    int opt = 0;
    unsigned int port = 1883;
    unsigned int t = 0;

    while ((opt = getopt(argc, argv, "h:p:P:t:u:V")) != -1) {
        if (opt == 't') {
            ntopics++;
        }
    }

    mqtt_topics = malloc(ntopics * sizeof(*mqtt_topics));

    optind = 0;
    while ((opt = getopt(argc, argv, "h:p:P:t:u:V")) != -1) {
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

    mqtt_host = host;
    mqtt_port = port;
    mqtt_username = username;
    mqtt_password = password;

    init_mqtt();

    return 0;
}
