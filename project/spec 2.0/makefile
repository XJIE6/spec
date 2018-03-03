DEPS = spec.h
OBJ = spec.o test_programs/sort.o test_programs/pow.o test_programs/dict.o test_programs/kmp.o test_programs/kmp2.o test_programs/kmp3.o x86_funcs/funcs.o my_malloc.c test_programs/eratosphen.c

main: main.o $(OBJ)
	gcc -W -o $@ $^

%.o: %.c $(DEPS)
	gcc -c -W -o $@ $<

clean:
	rm -f *.o main
