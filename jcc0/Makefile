CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g

# --- MDEBUG mode switch ---
ifdef JCC_MINI_DEBUG_PRET
CFLAGS += -DJCC_MINI_DEBUG_PRET
endif

# --- Main compiler ---
SRCS    = main.c lexer.c symtab.c tokenbuf.c gen.c ../jcc0part/jcc0_lex.c
OBJS    = $(SRCS:.c=.o)
TARGET  = jcc02m

CFLAGS  += -I../jcc0part

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

# --- Debug build ---
debug: CFLAGS += -DJCC_MINI_DEBUG_PRET
debug: $(TARGET)

clean:
	rm -f $(OBJS) jcc0_lex_test.o $(TARGET) $(TEST_LEX)

.PHONY: test run debug clean
