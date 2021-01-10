gcc = g++ -std=c++17 

all:	clear compiler

compiler: 
	bison -d -o parser.c parser.y
	flex -o lexer.c lexer.l
	$(gcc) lexer.c parser.c symbols.cpp compiler.cpp values/values.cpp utils/utils.cpp -o compiler

clear: 
	rm -f lexer.c lexer.o parser.c parser.h parser.o compiler

test:
	./compiler ../tests/test.imp ../tests/test.mr

testvm: test vm

testvmcln: test
	../maszyna_wirtualna/maszyna-wirtualna-cln ../tests/test.mr	

vm:
	../maszyna_wirtualna/maszyna-wirtualna ../tests/test.mr