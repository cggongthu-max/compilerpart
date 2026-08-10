#include <stdio.h>
#include "jcc0_lex.h"

static void test_skipspaces(void)
{
    char input[] = "    int s;";
    struct jcc0_lex_ctl lexctl;
    lexctl.input = input;

    jcc0_lex_skipspaces(&lexctl);

    if (lexctl.input == input + 4)
        printf("test_skipspaces: pass\n");
    else
        printf("test_skipspaces: FAIL\n");
}

int main(void)
{
    test_skipspaces();
    return 0;
}
