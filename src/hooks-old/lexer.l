%{
#include "y.tab.h"
#include <stdlib.h>
#include <string.h>

int yywrap(void) {
    return 1;
}
%}

%%

\%. {
    yylval.data.escape = yytext[1];
    yylval.data.text = NULL;

    return ESCAPE;
}

[^%]+ {
    yylval.text = strdup(yytext);
    return TEXT;
}

\%$ {
    yylval.text = strdup("%");
    return TEXT;
}

%%
