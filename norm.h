#ifndef NORM
#define NORM

double pogreshnost_norm(double *b, double n);
double nesvyazka_norm(double *matrix, double *b, double *x, int n, int rank, int size);

#endif