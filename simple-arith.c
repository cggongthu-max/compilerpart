#include <stdio.h>
#include <string.h>
#include <ctype.h>

int score_prec(char op)
{
  if (op == '*' || op == '/') return 2;
  if (op == '+' || op == '-') return 1;
  return 0;
}

void rpolarize_r(char out[], char input[])
{
  int j = 0;
  char stk[100];
  int stk_d = 0;
  int n = strlen(input);
  int i;
  for (i = 0; i < n; i++) {
    if (isdigit(input[i])) out[j++] = input[i];
    if (input[i] == '(') stk[stk_d++] = '(';
    if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/')
      {
	while (score_prec(stk[stk_d-1]) >= score_prec(input[i])) {
	  out[j++] = stk[--stk_d];
	}
	stk[stk_d++] = input[i];
      }
    if (input[i] == ')')
      {
	while (stk[stk_d-1] != '(') out[j++] = stk[--stk_d];
	--stk_d;
      }
  }
  while (stk_d) out[j++] = stk[--stk_d];
  out[j++] = '\0';
}

void rpn_compile(char rpn[])
{
  int i;
  int j = 0;
  int n = strlen(rpn);
  printf("\tLOC\t#100\nMain");
  for (i = 0; i < n; i++) {
    if (isdigit(rpn[i])) printf("\tSET\t$%d,%c\n", ++j, rpn[i]);
    if (rpn[i] == '+') { printf("\tADD\t$%d,$%d,$%d\n",j-1,j-1,j); j--;};
    if (rpn[i] == '-') { printf("\tSUB\t$%d,$%d,$%d\n",j-1,j-1,j); j--;};
    if (rpn[i] == '*') { printf("\tMUL\t$%d,$%d,$%d\n",j-1,j-1,j); j--;};
    if (rpn[i] == '/') { printf("\tDIV\t$%d,$%d,$%d\n",j-1,j-1,j); j--;};
  }
  printf("\tTRAP\t0,0,0\n");
}

int main(void)
{
  char sample_input[] = "3*(5-2)";
  char out[100];
  char result[100];
  rpolarize_r(out, sample_input);
  printf("%s\n", out);
  rpn_compile(out);
  return 0;
}
