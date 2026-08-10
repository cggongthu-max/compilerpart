CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g

# --- Unit test for jcc0_lex ---
TEST_LEX = jcc0_lex_test
$(TEST_LEX): jcc0_lex_test.o jcc0_lex.o
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_LEX)
	./$(TEST_LEX)

.PHONY: test run clean
