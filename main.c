#include "matrix_io.h"
#include "matrix_solve.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // for clock
#include "norm.h"
#include <mpi.h>


int main(int argc, char** argv)
{
    int rank, size;
    int debugMode = 1;

    int n, m, k;                                                                                                        //matrix size, site to output , formula
    int thisProcessN;
    char filename[64];
    double* matrix;
    double *rvector, *buf2;                                                                                                   //right-vector, buffer other issues
    int *buf;                                                                                           //?

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);               //this process id
    MPI_Comm_size(MPI_COMM_WORLD, &size);               // total number of processes



    double time_taken = 0;
    clock_t t = 0;                                                                                                      //cpu time
    time_t start_t = time(NULL);                                                                                  //real time


    if (argc == 5                                                                                                       //scenario 1 - input from file
		&& sscanf(argv[1], "%d", &n)
		&& sscanf(argv[2], "%d", &m) 
		&& sscanf(argv[3], "%d", &k) 
        && k == 0
		&& sscanf(argv[4], "%s[63]", &filename))
        {
            /*matrix = (double*)malloc(n*n*sizeof(double));
            printf("Reading matrix...\n");
            matrix_read(matrix, n, filename);*/
            printf("Ne umeyu");
            return -1;
        }

    else if (argc == 4                                                                                                  //scenario 2 - input from file
        && sscanf(argv[1], "%d", &n) 
		&& sscanf(argv[2], "%d", &m) 
		&& sscanf(argv[3], "%d", &k)
        &&  k != 0)
        {
            thisProcessN = (n/size) + (rank<(n%size)?1:0); //сколько строк этому процессу
            if (debugMode)
            {
                printf("\nProcess #%d of %d\n", rank, size);
                printf("I eat %d strings\n", thisProcessN);
            }
            matrix = (double*)malloc(n*thisProcessN*sizeof(double));      //every task has only his strings
            printf("Initializing matrix...\n");
            matrix_init(matrix, n, k, rank, size);
        }
    else 
    {
        printf("arg error.\n");
        return -1;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    printf("Initializing right vector...\n");
    rvector = (double*)malloc(thisProcessN*sizeof(double));
    rvector_init(rvector, matrix, n, rank, size);

    if (debugMode)
    {
        printf("P #%d r-vector: ", rank);
        for (int i = 0; i < thisProcessN; ++i)
        {
            printf("%lf   ", rvector[i]);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        printf("\n");
    }

    buf  = (int*)malloc(n*sizeof(int));
    buf2 = (double*)malloc(n*sizeof(double));


    printf("Starting Gauss SLE alg...\n");
    t = clock();
    SLE_solve(matrix, rvector, n, buf, buf2, rank, size);
    t = clock() - t;
/*
    time_taken = ((double)t)/CLOCKS_PER_SEC;

    matrix_print(buf2, n, 1, m);//вывод полученных ответов

    printf("Time spent: %lf seconds (time)\n", (double)(time(NULL)-start_t));
    printf("CPU time %lf senods (clock)\n", (double)time_taken);


    //переинициализация, по требованиям и для подсчета нормы несвязки
    if (k==0)
    {
        matrix_read(matrix, n, filename);
    }
    else
    {
        matrix_init(matrix, n, k); 
    }
    rvector_init(rvector, matrix, n);
    printf("Norm of nesvyazka is %10.3e\n", nesvyazka_norm(matrix, rvector, buf2, n));
    printf("Norm of pogreshnost is %lf\n", pogreshnost_norm(buf2, n));
*/


    printf("\nPr #%d successfully ending his life\n", rank);
    MPI_Finalize();
    printf("\nPr #%d successfully ended his life\n", rank);


    return 0;
}