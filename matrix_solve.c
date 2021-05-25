#include "matrix_solve.h"
#include <math.h>
#include <stdio.h>
#include <mpi.h>

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

int SLE_solve(double *matrix, double *b, int n, int *colseqMap, double *recv_str, int rank, int size)
{
    int col_max_id;
    double buf, b_root, koeff;
    int thisProcessN;
    int debugMode = 0;
    thisProcessN = (n/size) + (rank<(n%size)?1:0); //сколько строк у этого процесса

    //debugout(matrix, b, n);

    //порядок следования неизвестных
    for (int i = 0; i < n; i++)
        colseqMap[i] = i;
    
    for (int i=0; i<n; i++) //прямой ход гаусса - с последней строкой не надо
    {
        if (rank == (i % size)) {
            col_max_id = i; //ищем наибольший элемент по строке

            for (int j = i; j < n; j++)     //ищет только один поток
            {
                if (fabs(matrix[(i/size) * n + colseqMap[col_max_id]]) <
                    fabs(matrix[(i/size) * n + colseqMap[j]])) {
                    col_max_id = j;
                }
            }

            buf = matrix[(i/size) * n + colseqMap[col_max_id]];
            for (int j = 0; j < n; ++j) {
                matrix[(i/size) * n + j] = matrix[(i/size) * n + j]/buf;
            }
            b[i/size] = b[i/size]/buf;

            if (debugMode)
            {
                printf("\nPr #%d sand %d:", rank, i);
                for (int j = 0; j < n; ++j) {
                    printf("%lf    ", matrix[(i/size) * n + j]);
                }
                printf("and %lf\n", b[i/size]);
                printf("Max col id: %d, value %lf. \n", colseqMap[col_max_id], matrix[(i/size) * n + colseqMap[col_max_id]]);
            }




            // отправка строки и элемента rvector
            MPI_Bcast(matrix + (i / size) * n, n, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
            MPI_Bcast(b + i / size, 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
        } else {
            //прием строки и элемента rvector
            MPI_Bcast(recv_str, n, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
            MPI_Bcast(&b_root, 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
        }
        MPI_Bcast(&col_max_id, 1, MPI_INT, i % size, MPI_COMM_WORLD);


        if (rank != (i%size) && debugMode)
        {
            printf("\nPr #%d received:", rank);
            for (int j = 0; j < n; j++)
                printf("   %lf ", recv_str[j]);
            printf("and %lf", b_root);
            printf("\n");
        }


        //типо меняем столбцы местами - запоминаем порядок



        buf = colseqMap[i];
        colseqMap[i] = colseqMap[col_max_id];
        colseqMap[col_max_id] = buf;

        if (debugMode) {
            printf("i=%d, cmi=%d : ", i, col_max_id);
            for (int j = 0; j < n; ++j) {
                printf("%d  ", colseqMap[j]);
            }
            printf("\n");
            printf("starting minusing\n");
        }

        if (rank != (i%size))//почти все процессы вычитают полученную строку из своих
        {
            for (int j = 0; j < thisProcessN; ++j)
            {
                if (i <  j*size + rank)     //вычитаем только из последующих строк
                {
                    koeff = matrix[j * n + colseqMap[i]]; //коэффициент - так как отнормировали раньше, то просто соответствующий элемент матрицы
                    for (int k = 0; k < n; k++){
                        if (debugMode) {
                            printf("%lf - %lf*%lf = %lf\n", matrix[j * n + k], koeff, recv_str[k],
                                   matrix[j * n + k] - koeff * recv_str[k]);
                        }
                        matrix[j * n + k] -= koeff * recv_str[k];
                    }
                    b[j] -= koeff * b_root;
                }
            }
        } else //а владелец еще и строки нормирует ее на элемент
        {
            buf = matrix[(i / size) * n + colseqMap[i]];
            for (int j = 0; j < thisProcessN; ++j)
            {
                if (i/size < j)     //вычитаем только из последующих строк
                {
                    koeff = matrix[j * n + colseqMap[i]]; //коэффициент
                    for (int k = i; k < n; k++)
                        matrix[j * n + colseqMap[k]] -= koeff * matrix[(i / size) * n + colseqMap[k]];
                    b[j] -= koeff * b[i / size];
                }
            }
        }

        if (debugMode) {
            for (int j = 0; j < thisProcessN; ++j) {
                printf("Pr #%d, str %d:", rank, j);
                for (int k = 0; k < n; ++k) {
                    printf("%lf    ", matrix[j * n + k]);
                }
                printf("and %lf\n", b[j]);
            }
        }

    }

    MPI_Barrier(MPI_COMM_WORLD);

    //обратный ход
    for (int i=n-1; i>-1; i--)
    {
        if (rank == i % size)
            buf = b[i/size];
        MPI_Bcast(&buf, 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);

            for (int j = 0; j < thisProcessN; j++)
            {
                //i - номер строки во всей матрице, он же - номер столбца в котором вычетаем. j - номер строки в подматрице находящейся на процессе
                if (j*size + rank < i)
                    b[j] -= matrix[j*n+i]*buf;
                if (debugMode)
                {
                    printf("Pr #%d received %lf from %d, and made str %d, r.-v.: %lf\n", rank, buf, i % size, j, b[j]);
                }
            }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    printf("\nAnswers of %d:", rank);

    for (int i = 0; i < thisProcessN; ++i) {
        printf("%lf   ", b[i]);
    }
    printf("\n");

/*
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
     */
}