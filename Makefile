
#Use GNU compiler
cc = gcc
ccFLAGS= -g -std=c99
CC = g++
WARNFLAGS= -Wall -Wextra -pedantic
Library = -lreadline


LEX=flex
YACC=yacc
objects = shell.o command.o lex.yy.o y.tab.o

# all: shell cat_grep ctrl-c regular


all: shell

shell: $(objects)
	$(cc) $(ccFLAGS) $(WARNFLAGS) $(objects) $(Library) -o $@

shell.o : shell.c shell.h y.tab.h
	$(cc) $(ccFLAGS) $(WARNFLAGS) -c shell.c

command.o: command.c command.h
	$(cc) $(ccFLAGS) -c command.c

lex.yy.o: lexer.l y.tab.h
	$(LEX) lexer.l
	$(cc) $(ccFLAGS) $(WARNFLAGS) -c -g lex.yy.c

y.tab.o y.tab.h: parser.y
	$(YACC) -d parser.y
	$(cc) $(ccFLAGS) $(WARNFLAGS) -c -g y.tab.c

.PHONY : clean

clean :
	rm -f lex.yy.c y.tab.c y.tab.h shell *.o 

