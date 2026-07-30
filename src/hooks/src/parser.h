#ifndef PARSER_H
#define PARSER_H

struct token {
    int type;
    char *text;
    char *escape;
    struct token *next;
};

extern struct token *tokens;

#endif
