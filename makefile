a.out: main.c ass4.c
	gcc main.c ass4.c -std=c99 -lm -DSIZE=8
run: a.out
	./a.out
