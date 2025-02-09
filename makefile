all: scheduler

scheduler: main.c
	gcc -o scheduler main.c
