CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
SRCS    = main.c lexer.c ast.c parser.c symtab.c codegen.c
OBJS    = $(SRCS:.c=.o)
TARGET  = jcc02m

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: run clean
