CC=gcc
DEPS = flowChart_interpreter2.h flowChart_parser.h serealiser.h bToS.h bracketSer.h
OBJ = bToS.o bracketSer.o flowChart_interpreter2.o flowChart_parser.o serealiser.c

main: comp interpreter decomp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

comp: comp.o $(OBJ)
	gcc -o $@ $^

interpreter: interpreter.o $(OBJ)
	gcc -o $@ $^

decomp: decomp.o $(OBJ)
	gcc -o $@ $^

clean:
	rm -f *.o comp decomp interpreter
