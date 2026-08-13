CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g

# --- Main compiler ---
SRCS    = main.c lexer.c parser.c symtab.c codegen.c ../jcc0part/jcc0_lex.c
OBJS    = $(SRCS:.c=.o)
TARGET  = jcc02mstk

CFLAGS  += -I../jcc0part
SRCS    += ../jcc0part/jcc0_lex.c

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Unit test for jcc0_lex ---
TEST_LEX = jcc0_lex_test
$(TEST_LEX): jcc0_lex_test.o jcc0_lex.o
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_LEX)
	./$(TEST_LEX)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) jcc0_lex_test.o $(TARGET) $(TEST_LEX)

.PHONY: test run clean
