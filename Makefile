CC = gcc
BISON = bison
FLEX = flex
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

PARSER = parser

SRCS = symbol_table.c quadruplet.c

all: $(PARSER)

$(PARSER): mathlang.tab.c lex.yy.c $(SRCS)
	$(CC) $(CFLAGS) mathlang.tab.c lex.yy.c $(SRCS) -o $(PARSER) $(LDFLAGS)

mathlang.tab.c: mathlang.y
	$(BISON) -d -o mathlang.tab.c mathlang.y

lex.yy.c: mathlang.l mathlang.tab.h
	$(FLEX) -o lex.yy.c mathlang.l

.PHONY: test memcheck clean

test: $(PARSER)
	@./scripts/run_tests.sh


clean:
	rm -f $(PARSER) mathlang.tab.c mathlang.tab.h lex.yy.c
