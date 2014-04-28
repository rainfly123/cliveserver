/*
 * cliveserver - A fast live broadcast server ,supporting mulit-platform viewing.
 * Copyright (C) rainfly xiechc@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "util.h"
#include "event.h"
#include "log.h"
#include "timer.h"
#include "http.h"
#include "media.h"

static int clive_core_close(struct con *conn)
{
    struct event_base *evb;
    int status;
    evb = (struct event_base *)conn->evb;

    log_debug(LOG_INFO, "event del conn %d \n", conn->skt);
    status = event_del_conn(evb, conn);
    if (status < 0) {
        log_warn("event del conn %c %d failed, ignored: %s",
                 conn->type, conn->skt, strerror(errno));
    }

    conn->close(conn);
}

static int clive_core_recv(struct con *conn)
{
    int status;

    status = conn->recv(conn);
    if (status != CL_OK) {
        log_debug(LOG_INFO, "recv on %c %d failed: %s",
                  conn->type, conn->skt,
                  strerror(errno));
    }

    return status;
}

static int clive_core_send(struct con *conn)
{
    int  status;

    status = conn->send(conn);
    if (status != CL_OK) {
        log_debug(LOG_INFO, "send on %c %d failed: %s",
                  conn->type, conn->skt,
                  strerror(errno));
    }

    return status;
}


int 
clive_core_core(void *arg, uint32_t events)
{
    int status;
    struct con *conn = arg;

    log_debug(LOG_VVERB, "event %04x on %c %d", events,
              conn->type, conn->skt);

    conn->events = events;

    /* error takes precedence over read | write */
    if (events & EVENT_ERR) {
        clive_core_close(conn);
        return CL_ERROR;
    }

    /* read takes precedence over write */
    if (events & EVENT_READ) {
        status = clive_core_recv(conn);
        if (status != CL_OK || conn->done || conn->err) {
            clive_core_close(conn);
            return CL_ERROR;
        }
    }

    if (events & EVENT_WRITE) {
        status = clive_core_send(conn);
        if (status != CL_OK || conn->done || conn->err) {
            clive_core_close(conn);
            return CL_ERROR;
        }
    }

    return CL_OK;
}


