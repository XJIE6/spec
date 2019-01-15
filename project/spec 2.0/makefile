DEPS = spec.h
OBJ = spec.o test_programs/interpreter.o test_programs/interpreter2.o test_programs/sort.o test_programs/pow.o test_programs/dict.o test_programs/kmp.o test_programs/kmp2.o test_programs/kmp3.o x86_funcs/funcs.o my_malloc.c test_programs/eratosphen.c
CPPOBJ = test_programs/parser.o

main: main.o $(OBJ) $(CPPOBJ)
	gcc -Wall -O0 -o $@ $^ -lstdc++

test_programs/parser.o: test_programs/parser.cpp
	g++ -c -O0 -W -std=c++14 -o test_programs/parser.o -I test_programs/pcomb/include/ test_programs/parser.cpp

%.o: %.c $(DEPS)
	gcc -c -O0 -W -o $@ $< -mno-tls-direct-seg-refs

clean:
	rm -f *.o test_programs/*.o x86_funcs/*.o main
