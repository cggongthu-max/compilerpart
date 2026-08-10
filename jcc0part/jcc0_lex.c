#include <stdio.h>
#include <ctype.h>

struct jcc0_lex_ctl { char const *input; };

void jcc0_lex_skipspaces (struct jcc0_lex_ctl *lexctl)
{
  while (isspace((unsigned char)*lexctl->input))
    lexctl->input++;
}

void test_skipspaces ()
{
  char input[] = "    int s;";
  struct jcc0_lex_ctl lexctl;
  lexctl.input = input;

  jcc0_lex_skipspaces (&lexctl);
  if (lexctl.input == input+4)
    printf ("test pass\n");
  else
    printf ("test fail\n");
}

int main(void)
{
  printf ("test_skipspaces\n");
  test_skipspaces();
}
