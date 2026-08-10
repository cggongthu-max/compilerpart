#ifndef JCC0_LEX_H
#define JCC0_LEX_H

/* Lexer control block: holds the current scan position. */
struct jcc0_lex_ctl {
    char const *input;
};

/* Advance input past whitespace characters. */
void jcc0_lex_skipspaces(struct jcc0_lex_ctl *lexctl);

#endif
