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

#define mqtt_cid "vallumd"

int init_mqtt(char *mqtt_host, int mqtt_port) {
    bool clean_session = true;
    int keepalive = 60;
    struct mosquitto *m = NULL;

    mosquitto_lib_init();

    m = mosquitto_new(mqtt_cid, clean_session, NULL);

    if(mosquitto_connect(m, mqtt_host, mqtt_port, keepalive)) {
        fprintf(stderr, "Unable to connect to %s:%d\n", mqtt_host, mqtt_port);
        return 1;
    }

    mosquitto_loop_forever(m, -1, 1);

    mosquitto_destroy(m);
    mosquitto_lib_cleanup();
    return 0;
}