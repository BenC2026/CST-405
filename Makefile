CC = gcc
LEX = flex
YACC = bison
CFLAGS = -g -Wall

TARGET = minicompiler
OBJS = lex.yy.o parser.tab.o main.o ast.o symtab.o codegen.o tac.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

lex.yy.c: scanner.l parser.tab.h
	$(LEX) scanner.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

main.o: main.c ast.h codegen.h tac.h
	$(CC) $(CFLAGS) -c main.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

codegen.o: codegen.c codegen.h ast.h symtab.h
	$(CC) $(CFLAGS) -c codegen.c

tac.o: tac.c tac.h ast.h
	$(CC) $(CFLAGS) -c tac.c

clean:
	rm -f $(TARGET) $(OBJS) lex.yy.c parser.tab.c parser.tab.h *.s

test: $(TARGET)
	./$(TARGET) test.c test.s
	@echo "\n=== Generated MIPS Code ==="
	@cat test.s

.PHONY: all clean test