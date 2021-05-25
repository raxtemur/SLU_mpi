CC=mpicc
CFLAGS=-c -O2

all: a

a: main.o matrix_o.o matrix_i.o matrix_solve.o norm.o
	$(CC) main.o matrix_o.o matrix_i.o matrix_solve.o norm.o -o a -lm

main.o: main.c
	$(CC) $(CFLAGS) main.c

matrix_i.o: matrix_i.c
	$(CC) $(CFLAGS) matrix_i.c

matrix_o.o: matrix_o.c
	$(CC) $(CFLAGS) matrix_o.c

matrix_solve.o: matrix_solve.c
	$(CC) $(CFLAGS) matrix_solve.c

norm.o: norm.c
	$(CC) $(CFLAGS) norm.c

clean:
	rm -rf *.o
