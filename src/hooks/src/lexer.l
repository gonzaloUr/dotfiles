%{
#include "y.tab.h"
#include <stdlib.h>
#include <string.h>

int yywrap(void) {
    return 1;
}
%}

%%

\%[^{}] {
    yylval.data.escape = NULL;
    yylval.data.text = strdup(yytext + 1);
    return ESCAPE;
}

\%\{[^}]*\} {
    yylval.data.escape = strdup(yytext + 2);
    yylval.data.escape[strlen(yylval.data.escape)-1] = '\0';
    yylval.data.text = NULL;
    return ESCAPE;
}

\%.\{[^}]*\} {
    yylval.data.text = strdup(yytext + 1);
    yylval.data.text[1] = '\0';

    yylval.data.escape = strdup(yytext + 3);
    yylval.data.escape[strlen(yylval.data.escape)-1] = '\0';

    return ESCAPE;
}

[^\%]+ {
    yylval.text = strdup(yytext);
    return TEXT;
}

. ;

%%
