.PHONY: all codegen.exe generate

SOURCES = lrparser.tab.c lrlex.yy.c ast.c codegen.c main.c
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wno-unused-function

all: generate
	$(CC) $(CFLAGS) $(SOURCES) -o genllvm

codegen.exe: generate
	$(CC) $(CFLAGS) $(SOURCES) -o $@

generate:
	bison -d -o lrparser.tab.c lrparser.y
	flex -o lrlex.yy.c lrlex.l
