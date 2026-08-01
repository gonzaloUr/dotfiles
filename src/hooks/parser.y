%{
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

int yylex(void);
void yyerror(const char *s);

struct token *tokens = NULL;
struct token *last = NULL;
%}

%union {
    char *text;
    struct {
        char escape;
        char *text;
    } data;
}

%token <text> TEXT
%token <data> ESCAPE

%%

input: | input token
;

token:
    TEXT
    {
        struct token *t = malloc(sizeof(*t));
        t->type = TEXT;
        t->text = $1;
        t->next = NULL;

        if (last)
            last->next = t;
        else
            tokens = t;

        last = t;
    }

  | ESCAPE
    {
        struct token *t = malloc(sizeof(*t));
        t->type = ESCAPE;
        t->text = $1.text;
        t->escape = $1.escape;
        t->next = NULL;

        if (last)
            last->next = t;
        else
            tokens = t;

        last = t;
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "parse error: %s\n", s);
}
