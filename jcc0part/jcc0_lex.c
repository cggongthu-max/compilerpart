#include "jcc0_lex.h"
#include <ctype.h>

void jcc0_lex_skipspaces(struct jcc0_lex_ctl *lexctl)
{
    while (isspace((unsigned char)*lexctl->input))
        lexctl->input++;
}
