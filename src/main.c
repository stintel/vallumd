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

#include "config.h"
#include "mosquitto.h"

static void print_usage() {
    printf("Usage: -h host [-p port] [-t topic]\n");
    printf(" -h: MQTT host to connect to\n");
    printf(" -p: MQTT port to connect to (1883)\n");
    printf(" -t: MQTT topic and IPset name (vallumd)\n");
    printf(" -V: print version number and exit\n");
}

int main(int argc, char **argv) {
    char *host = NULL;
    char *topic = "vallumd";
    unsigned int port = 1883;
    unsigned int opt = 0;

    while ((opt = getopt(argc, argv, "h:p:t:V")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                topic = optarg;
                break;
            case 'V':
                fprintf(stdout, "vallumd-%d.%d.%d\n",
                        VERSION_MAJOR,
                        VERSION_MINOR,
                        VERSION_PATCH);
                return 0;
            default:
                print_usage();
                return 1;
        }
    }

    mqtt_host = host;
    mqtt_port = port;
    mqtt_topic = topic;

    init_mqtt();

    return 0;
}
