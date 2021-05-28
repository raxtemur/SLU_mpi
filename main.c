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
    int debugMode = 0,
        gatherAnswers = 1;

    int n, m, k;                                                                                                        //matrix size, site to output , formula
    int thisProcessN;
    char filename[64];
    double* matrix;
    double *rvector, *buf2;                                                                                                   //right-vector, buffer other issues
    int *buf;                                                                                           //?
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);               //this process id
    MPI_Comm_size(MPI_COMM_WORLD, &size);               // total number of processes



    double time_taken = 0;
    double t = 0;                                                                                                      //cpu time
    //time_t start_t = time(NULL);                                                                                  //real time


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

    else if (argc == 4                                                                                                  //scenario 2 - init
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

    printf("P #%d initializing right vector...\n", rank);
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

    printf("P #%d starting Gauss SLE alg...\n", rank);
    t = MPI_Wtime();
    SLE_solve(matrix, rvector, n, buf, buf2, rank, size);
    t = MPI_Wtime() - t;



    MPI_Barrier(MPI_COMM_WORLD);

    //ща будет нормальная сборка ответов
    //better than any MPI_Gather
    if (rank == 0)
    {
        printf("\nGatheting answers...\n");
        for (int j = 0; j < thisProcessN; ++j)
        {
            buf2[j*size] = rvector[j];
        }
        for (int i = 1; i < size; ++i)
        {
            thisProcessN = (n/size) + (i<(n%size)?1:0);
            MPI_Recv(rvector, thisProcessN, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            for (int j = 0; j < thisProcessN; ++j)
            {
                buf2[j*size + i] = rvector[j];
            }
        }
        printf("Answers:\n");
        matrix_print(buf2, 1, n, m, rank, size);
        printf("\n");
    }
    else
    {
        MPI_Send(rvector, thisProcessN, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) printf("Time spent: %lf seconds (time)\n", t);
    //printf("P#%d CPU time %lf senods (clock)\n", rank, (double)time_taken);


    //переинициализация, по требованиям и для подсчета нормы несвязки
    if (k==0)
    {
        printf("Ne umeju, skazal je!\n");
        //matrix_read(matrix, n, filename);
    }
    else
    {
        matrix_init(matrix, n, k, rank, size);
    }




    if (rank == 0) //0 - выводит и один считает погрешность - он трудоголик(нельзя же все оптимизировать), остальным не помешало бы скинуться на невязку
    {
        printf("Norm of nesvyazka is %10.3e\n", nesvyazka_norm(matrix, rvector, buf2, n, rank, size));
        printf("Norm of pogreshnost is %lf\n", pogreshnost_norm(buf2, n));
    }
    else
    {
        nesvyazka_norm(matrix, rvector, buf2, n, rank, size);
    }



    MPI_Finalize();


    return 0;
}