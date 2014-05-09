#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "util.h"
#include "media.h"
#include "channel.h"
#include "http.h"
#include "jsmn.h"


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

int main(int argc, char **argv)
{
    struct sigaction sa;
    jsmntok_t * tokens;
    char *buffer;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0 );

    log_init(LOG_VERB, NULL);
    log_debug(LOG_DEBUG, "cliveserver starting..");
    buffer = read_cfg("cliveserver.conf");
    tokens = parse_cfg(buffer);

    typedef enum { START, KEY, PRINT, SKIP} parse_state;
    parse_state state = START;

    int i = 0, j = 1;
    char *value;
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
                size_t m;
                for (m = 0; m < sizeof(KEYS)/sizeof(char *); m++)
                {
                    if (json_token_streq(buffer, t, KEYS[m]))
                    {
                        printf("%s: ", KEYS[m]);
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
                printf("%s\n", value);
                state = KEY;


        }
    }
    return 0;
}
