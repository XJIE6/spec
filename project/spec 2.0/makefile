DEPS = spec.h
OBJ = spec.o sort.o pow.o dict.o kmp.o kmp2.o funcs.o my_malloc.c eratosphen.c

main: main.o $(OBJ)
	gcc -W -o $@ $^

%.o: %.c $(DEPS)
	gcc -c -W -o $@ $<

clean:
	rm -f *.o main


#4) val: -304, mem: 0, dyn: 0
#5) val: -232, mem: 0, dyn: 0
