#ifndef MATRIX_SOLVE
#define MATRIX_SOLVE
int col_swap(double *matrix, int n, int p, int q);
int SLE_solve(double *matrix, double *b, int n, int *seq, double *b_copy, int rank, int size);
#endif