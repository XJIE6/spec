DEPS = spec.h
OBJ = spec.o sort.o pow.o dict.o kmp.o kmp2.o funcs.o my_malloc.c eratosphen.c

main: main.o $(OBJ)
	gcc -o $@ $^

%.o: %.c $(DEPS)
	gcc -c -o $@ $<

clean:
	rm -f *.o main
