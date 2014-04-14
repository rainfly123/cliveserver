#include <sys/timerfd.h>
#include <errno.h>
#include <time.h>
#include "util.h"
#include "log.h"
#include "event.h"
#include "timer.h"

static void con_close(struct con * conn)
{
    ASSERT(conn != NULL);
    struct timer * timer= (struct timer *) conn;
    log_debug(LOG_VERB, "close on sd %d", conn->skt);
    close(conn->skt);
    clive_free(conn);
}

static int con_recv(struct con *conn)
{
    ssize_t n;

    ASSERT(conn != NULL);
    struct timer * timer= (struct timer *) conn;

    for (;;) {
        n = clive_read(conn->skt, &timer->rbuffer, sizeof(uint64_t));
        log_debug(LOG_VERB, "recv on sd %d %zd %lld ", conn->skt, n, timer->rbuffer);
        if (n > 0) {
            //call handler
            timer->handle->handle(timer, timer->handle->data);
            return CL_OK;
        }

        if (n == 0) {
            log_debug(LOG_INFO, "recv on sd %d eof", conn->skt);
            return CL_CLOSE;
        }

        if (errno == EINTR) {
            log_debug(LOG_VERB, "recv on sd %d not ready - eintr", conn->skt);
            continue;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            log_debug(LOG_VERB, "recv on sd %d not ready - eagain", conn->skt);
            return CL_OK;
        } else {
            conn->err = errno;
            log_error("recv on sd %d failed: %s", conn->skt, strerror(errno));
            return CL_ERROR;
        }
    }

    NOT_REACHED();
    return CL_ERROR;
}

static int con_send(struct con *conn)
{
    ASSERT(conn != NULL);
    ssize_t n;

    struct timer * timer= (struct timer *) conn;
    for (;;) 
    {
        n = clive_write(conn->skt, &timer->sbuffer, sizeof(uint64_t));
        log_debug(LOG_VERB, "send on sd %d %zd", conn->skt, n);
        if (n > 0) {
            {
                log_debug(LOG_VERB, "send on sd %d complete \n", conn->skt);
                return CL_OK;
              //send completed;
            }
        } else {

        if (errno == EINTR) {
            log_debug(LOG_VERB, "send on sd %d not ready - eintr", conn->skt);
            continue;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            log_debug(LOG_VERB, "send on sd %d not ready - eagain", conn->skt);
            return CL_OK;
        } else {
            conn->err = errno;
            log_error("send on sd %d failed: %s", conn->skt, strerror(errno));
            return CL_ERROR;
        }
        }
    }

    NOT_REACHED();
    return CL_ERROR;
}


struct timer * clive_timer_new(struct event_base *evb, uint32_t msecs, tevent_handler handle,\
                            void *data)
{
    struct timer * timer = clive_calloc(1, sizeof(struct timer));
    timer->handle = clive_calloc(1, sizeof(struct tevents));
    struct itimerspec new_value;
    struct timespec now;

    timer->connection.skt = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    timer->connection.type = tTIMER;
    timer->connection.evb = evb;
    timer->connection.send = &con_send;
    timer->connection.recv = &con_recv;
    timer->connection.close = &con_close;
    timer->handle->handle = handle;
    timer->handle->data = data;
    if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
        log_debug(LOG_DEBUG, "clock gettime error");
        clive_free(timer);
        return NULL;
    }

    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;
    new_value.it_value.tv_sec = now.tv_sec + msecs / 1000;
    new_value.it_value.tv_nsec = now.tv_nsec + msecs % 1000;
    if (timerfd_settime(timer->connection.skt, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
    {
        log_debug(LOG_DEBUG, "timerfd settime error");
        clive_free(timer);
        return NULL;
    }

    event_add_conn(evb, &timer->connection);
    event_del_out(evb, &timer->connection);
    return timer;
}

int clive_timer_update_time(struct timer *timer, uint32_t msecs)
{
    ASSERT(time != NULL);
    struct itimerspec new_value;
    struct timespec now;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
        log_debug(LOG_DEBUG, "clock gettime error");
        return -1;
    }

    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;
    new_value.it_value.tv_sec = now.tv_sec + msecs / 1000;
    new_value.it_value.tv_nsec = now.tv_nsec + msecs % 1000;
   
    if (timerfd_settime(timer->connection.skt, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
    {
        log_debug(LOG_DEBUG, "timerfd settime error");
        return -1;
    }
    return 0;
}

int clive_timer_update_handle(struct timer *timer, tevent_handler handle, void *data)
{
    ASSERT(time != NULL);
    timer->handle->handle = handle;
    timer->handle->data = data;
    return 0;
}
