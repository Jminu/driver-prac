executable: distance.o main.o
	ld *.o /usr/lib/aarch64-linux-gnu/crt1.o -lm -lc

distance.o: distance.c
	gcc -c -o distance.o distance.c

main.o: main.c
	gcc -c -o main.o main.c

