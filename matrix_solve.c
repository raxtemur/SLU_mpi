#include "matrix_solve.h"
#include <math.h>
#include <stdio.h>

int col_swap(double *matrix, int n, int p, int q)
{
    double buf;
    if (p == q) return 0;
    for(int i=0; i<n; i++)
    {
        buf = matrix[i+p*n];
        matrix[i+p*n] = matrix[i+q*n];
        matrix[i+q*n] = buf;
    }
    return 0;
}

int line_minus(double *matrix, double *b, int n, int l, int m) //(l) - k*(m)
{
    double k = matrix[l + m*n]/matrix[m + m*n];
    if (k == 0) return 0;
    for (int i = 0; i < n; i++)
    {
        matrix[l + n*i] = matrix[l + n*i] - matrix[m + n*i] * k;
    }
    b[l] = b[l] - k*b[m];
    return 0;
}

void debugout(double *matrix, double *b, int n)
{
    for (int i=0; i<n; i++)//line
    {
        for (int j=0; j<n; j++)//column
        {
            printf("%10.3e", matrix[i + j * n]);
        }
        printf(" | %lf\n", b[i]);
    }
    printf("\n");
}

int SLE_solve(double *matrix, double *b, int n, int *seq, double *b_copy)
{
    int k;
    double buf;
    
    //debugout(matrix, b, n);

    //порядок следования неизвестных
    for (int i = 0; i < n; i++)
        seq[i] = i;
    
    for (int i=0; i<n-1; i++) //прямой
    {
        k = i;
        for (int j=i; j<n; j++)//с последней строкой не надо
        {
            if(fabs(matrix[i+k*n]) < fabs(matrix[i+j*n])) 
                {
                    k = j;
                }
        }

        col_swap(matrix, n, i, k);

        //запоминаем порядок
        buf = seq[i];
        seq[i] = seq[k];
        seq[k] = buf; 

        for (int j=i+1; j<n; j++)
        {
            line_minus(matrix, b, n, j, i);
        }
    }

    //debugout(matrix, b, n);

    //обратный ход
    for (int i=n-1; i>-1; i--)
    {
        for (int j=0; j<i; j++)
        {
            line_minus(matrix, b, n, j, i);
        }
    }
    

    //debugout(matrix, b, n);
    
    //ищем неизвестные
    for (int i = 0; i < n; i++)
    {
        b[i] = 1.0*b[i]/matrix[i + n*i];
        matrix[i + n*i] = 1;
    }
    
    //debugout(matrix, b, n);
    
    //восстанавливаем порядок
    for (int i = 0; i < n; i++)
        b_copy[seq[i]] = b[i];
    for (int i = 0; i < n; i++)
        b[i] = b_copy[i];
    
    return 0;
}