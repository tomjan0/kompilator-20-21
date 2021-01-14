gcc = g++ -std=c++17 -Wall

all:	clear compiler

compiler: 
	bison -d -o parser.c parser.y
	flex -o lexer.c lexer.l
	$(gcc) lexer.c parser.c compiler.cpp\
		   symbols/symbols.cpp  values/values.cpp \
		   utils/utils.cpp expressions/expressions.cpp \
		   conditions/conditions.cpp -o kompilator

clear: 
	rm -f lexer.c lexer.o parser.c parser.h parser.o kompilator