#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "log.h"
#include "util.h"
#include "media.h"
#include "channel.h"
#include "http.h"
#include "jsmn.h"
#include "event.h"
#include "core.h"


char * KEYS[] = {"channel_name", "input_url", "output"};

static bool json_token_streq(char *js, jsmntok_t *t, char *s)
{
    return (strncmp(js + t->start, s, (size_t)(t->end - t->start)) == 0
            && (int32_t)strlen(s) == (t->end - t->start));
}

static char * json_token_tostr(char *js, jsmntok_t *t)
{
    js[t->end] = '\0';
    return js + t->start;
}

static char *read_cfg(const char *file_name)
{
    FILE *cfg;
    size_t size;
    char *buffer;
    size_t got;

    cfg  = fopen(file_name, "r");
    if (cfg == NULL) {
        log_error("open config file failed");
        exit(-1);
    }
    fseek(cfg, 0, SEEK_END);
    size = (size_t)ftell(cfg);
    fseek(cfg, 0, SEEK_SET);
    buffer = (char *) malloc(size);
    got = fread(buffer, 1, size, cfg);
    if (got != size) {
        log_error("read config file failed");
        exit(-1);
    }
    fclose(cfg);
    return buffer;
}

static jsmntok_t * parse_cfg(char *buffer)
{
    uint32_t n  = 256;
    int32_t ret;
    jsmn_parser parser;

    jsmn_init(&parser);
    jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);
    ret = jsmn_parse(&parser, buffer, strlen(buffer), tokens, n);
    while (ret == JSMN_ERROR_NOMEM)
    {
        n = n * 2 + 1;
        tokens = realloc(tokens, sizeof(jsmntok_t) * n);
        ret = jsmn_parse(&parser, buffer, strlen(buffer), tokens, n);
    }

    if ((ret == JSMN_ERROR_INVAL) || (ret == JSMN_ERROR_PART)) {
        log_error("jsmn_parse: invalid JSON string");
        exit(-1);
    }

    return tokens;
}

#define MAX_OUTPUTS 5
typedef struct {
    char *channel_name;
    char *input_url;
    char *outputs[MAX_OUTPUTS];
    int output_total;
}CfgStore;

int main(int argc, char **argv)
{
    struct sigaction sa;
    jsmntok_t * tokens;
    char *buffer;
    struct event_base *evb;
    Channel *channel;
    int i = 0, j = 1, k = 0;
    char *value;
    CfgStore temp;
    size_t m;
    int comma;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0 );

    log_init(LOG_DEBUG, NULL);
    clive_init_channel();
    clive_media_task_thread_start();
    clive_http_server_start();
    log_debug(LOG_DEBUG, "cliveserver starting..");
    evb  = event_base_create(EVENT_SIZE, &clive_core_core);
    buffer = read_cfg("cliveserver.conf");
    tokens = parse_cfg(buffer);
    //clive_new_channel(evb, );
    typedef enum { START, KEY, PRINT, SKIP} parse_state;
    parse_state state = START;

    for (; j > 0; i++, j--)
    {
        jsmntok_t *t = &tokens[i];
        if ((t->start == -1) || (t->end == -1))
            return -1;
        if (t->type == JSMN_ARRAY || t->type == JSMN_OBJECT)
            j += t->size;
        switch (state)
        {
            case START:
                if (t->type != JSMN_OBJECT) {
                    return -1;
                }
                state = KEY;

            case KEY:

                if (t->type != JSMN_PRIMITIVE) {
                    break;
                }
                state = SKIP;
                for (m = 0; m < sizeof(KEYS)/sizeof(char *); m++)
                {
                    if (json_token_streq(buffer, t, KEYS[m]))
                    {
                        state = PRINT;
                        break;
                    }
                }
                break;

            case SKIP:
                state = KEY;
                break;

            case PRINT:
                value = json_token_tostr(buffer, t);
                if (m == 0) {
                    memset(&temp, 0, sizeof(temp));
                    temp.channel_name = value;
                }else if (m == 1) {
                    temp.input_url = value;
                } else {
                    temp.output_total = t->size;
                    k = 0;
                    comma = t->start;
                    for (; k < temp.output_total; k++) {
                        do {
                            if (buffer[comma] >= 'h')
                                break;
                            comma++;
                        }while( buffer[comma] != '\0');
                        temp.outputs[k] = &buffer[comma];
                        do {
                            if (buffer[comma] == '"')
                                break;
                            comma++;
                        }while( buffer[comma] != '\0');
                        buffer[comma] = '\0';
                    }
                    log_debug(LOG_INFO, "add channel:");
                    log_debug(LOG_INFO, "    channel_name: %s", temp.channel_name);
                    log_debug(LOG_INFO, "    input_url: %s", temp.input_url);
                    log_debug(LOG_INFO, "    outputs: ");
                    for (k = 0; k < temp.output_total; k++) {
                        log_debug(LOG_INFO, "        %s", temp.outputs[k]);
                    }
                    channel = clive_new_channel(evb, temp.input_url, temp.channel_name);
                    for (k = 0; k < temp.output_total; k++) {
                        clive_channel_add_output(channel, temp.outputs[k]);
                    }
                    clive_channel_start(channel);
                    clive_channel_add(channel);
                }
                state = KEY;
        }
    }
    while (1) {
        int nsd;
        nsd = event_wait(evb, 500);
        if (nsd < 0) {
            log_debug(LOG_INFO, "wait error");
            return (void *)0;
        }
    }
    return 0;
}
