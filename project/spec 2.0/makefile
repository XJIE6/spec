OBJ = test_programs/parser.o test_programs/interpreter2.o test_programs/run_config.o spec.o machine_code.o state.o my_malloc.o

main: main.o $(OBJ) x86_funcs/funcs.o spec_funcs_aggregator.o
	gcc -Wall -W -Wfatal-errors -O0 -o $@ $^ -lstdc++

test_programs/parser.o: test_programs/parser.cpp
	g++ -c -O0 -Wall -W -Wfatal-errors -std=c++14 -o $@ -I test_programs/pcomb/include/ $^

%.o: %.c
	gcc -c -O0 -Wall -W -Wfatal-errors -o $@ $^ -mno-tls-direct-seg-refs

spec_funcs_aggregator.o: spec_funcs_aggregator.c spec_funcs.c
	gcc -c -O0 -Wall -W -Wfatal-errors -o $@ spec_funcs_aggregator.c -mno-tls-direct-seg-refs

x86_funcs/funcs.o: x86_funcs/funcs.c x86_funcs/add_funcs.c x86_funcs/cmp_funcs.c x86_funcs/imul_funcs.c x86_funcs/jmp_funcs.c x86_funcs/mov_funcs.c x86_funcs/pop_funcs.c x86_funcs/push_funcs.c x86_funcs/sub_funcs.c
	gcc -c -O0 -Wall -W -Wfatal-errors -o $@ x86_funcs/funcs.c -mno-tls-direct-seg-refs

clean:
	rm -f *.o test_programs/*.o x86_funcs/*.o main
