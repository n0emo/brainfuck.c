args = sample.bf

bf: brainfuck.c
	gcc -g brainfuck.c -o bf

.PHONY: run
run: bf
	./bf $(args)
