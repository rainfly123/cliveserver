#include <signal.h>

int main(int argc, char *argv)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0 );

}
