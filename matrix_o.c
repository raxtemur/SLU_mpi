#include "matrix_io.h"
#include <stdio.h>


int matrix_print(double* matrix, int strings, int columns, int max_n) //
{
    for (int i=0; (i<strings)&&(i<max_n); i++)//line
    {
        for (int j=0; (j<columns)&&(j<max_n); j++)//column
        {
            printf("%10.3e", matrix[i*strings + j]);
        }
        printf("\n");
    }
    return 0;
}
