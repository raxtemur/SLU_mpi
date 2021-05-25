#include "matrix_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//матрица хранится по столбцам
//нужно чтобы хранилась по строкам


double fun(int k, int n, int i, int j) //num of fun, size, parametrs
{
    switch (k) {
        case 1:
            return (i>j)?(n-i):(n-j);     //n-max(i, j)
        case 2:
            return (i>j?i:j) + 1;         //max(i, j)
        case 3:
            return i>j?(i - j):(j - i); //abs(i-j)
        case 4:
            return 1 / (i + 2*j + 1.0);
    }
    return 0;
}

int matrix_init(double *matrix, int n, int k, int rank, int size) //matrix, size, num of formula
{
    int debugMode = 0;
    int thisProcessN;
    if (k < 1 || k > 4) 
    {
        printf("init fun error!\n");
        return -1;
    }
    thisProcessN = (n/size) + (rank<(n%size)?1:0); //сколько строк этому процессу

    for (int i=0; i < thisProcessN; i++)
    {
        if(debugMode)
            printf("Pr #%d, i=%d, ie str=%d:   ", rank,i,size*i+rank);
        for (int j = 0; j < n; j++)
        {
            matrix[i*n+j] = fun(k, n, size*i+rank, j);
            if(debugMode)
                printf("%lf    ", matrix[i*n+j]);
        }
        if(debugMode)
            printf("\n");

    }


    /*for (int i = 0; i < n; i++) 
        if (size*j+rank < n)
            b[i] = fun(k, n, i, rank);
        matrix_print(b, n, 1, n);
        MPI_Gather(b, n, MPI_DOUBLE, matrix, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    */
    /*
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
            matrix[i + n * j] = fun(k, n, i, j);
    }
    */
    return 0;
}

void rvector_init(double *b, double *matrix, int n, int rank, int size)
{
    int debugMode = 0;
    int thisProcessN;
    for (int i=0; i<n; i++)// line
    {
        b[i] = 0;
    }
    thisProcessN = (n/size) + (rank<(n%size)?1:0); //сколько строк этому процессу
    if (debugMode)
        printf("N for this process: %d\n", thisProcessN);

    for (int i = 0; i < thisProcessN; ++i) {
        for (int k=0; k < (n-1)/2 + 1; k++) //column, seems like there is mistake in task
        {
            if (debugMode)
                printf("rv %d: %lf + %lf\n", i, b[i], matrix[i*n + 2*k]);
            b[i] += matrix[i*n + 2*k];
        }
        if (debugMode)
            printf("rv %d: %lf\n", i, b[i]);
    }

}

int matrix_read(double *matrix, int n, char *filename) //matrix, size, filename
{
    FILE* ftr = fopen(filename, "r");
    char buf;
    if (ftr == NULL) {
        printf("opening file error!\n");
        return -1;
    }

    for (int i=0; i<n; i++)//line
    {
        for (int j=0; j<n; j++)//column
        {
            if (!fscanf(ftr, "%lf", &matrix[i+j*n]))
            {
                printf("file reading error!\n");
                fclose(ftr);
                return -2;
            }
            //?how to deal " " 
        }
        buf = fgetc(ftr);
        if (buf == ' ')
        {
            buf = fgetc(ftr);
        }
        if ((buf != '\n') && ((i == n -1) && (buf != EOF))) 
        {
            printf("matrix reding error!\n");
            fclose(ftr);
            return -3;
        }
    }
    fclose(ftr);
    return 0;
}

