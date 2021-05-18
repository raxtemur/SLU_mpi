#ifndef MATRIX_IO
#define MATRIX_IO

double fun(int k, int n, int i, int j); //num of fun, size, parametrs
int matrix_init(double *matrix, int n, int k, int rank, int size); //matrix, size, num of formula
int matrix_read(double *matrix, int n, char *filename); //matrix, size, filename
void rvector_init(double *b, double *matrix, int n);

int matrix_print(double* matrix, int strings, int columns, int max_n);//matrix, lines, columns, size to print
int rvector_print(double* b, int n, int l);

#endif