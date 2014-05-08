#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "util.h"
#include "media.h"
#include "channel.h"
#include "http.h"

int main(int argc, char *argv)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0 );

    log_init(LOG_VERB, NULL);
    log_debug(LOG_DEBUG, "cliveserver starting..");

}
