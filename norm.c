#include <math.h>
#include <mpi.h>


double vec_norm(double *v, int n) //now it doesn't take sqrt
{
    double s = 0;
    for (int i = 0; i < n; i++)
    {
        s += v[i]*v[i];
    }
    return s;
}

double pogreshnost_norm(double *b, double n)
{
    for (int i = 0; i < n; i++)
        b[i] = b[i] - (i+1)%2;
    return sqrtl(vec_norm(b, n))/sqrtl((n+1)/2);
}

double nesvyazka_norm(double *matrix, double *b, double *x, int n, int rank, int size)
{
    double Ax = 0, sum = 0, buf = 0;
    int thisProcessN = (n/size) + (rank<(n%size)?1:0); //мы все уже давно знакомы знакомы с этой величиной - сколько строк обрабатывает этот процесс
    for (int i = 0; i < thisProcessN; i++)
    {
        Ax = 0;
        for (int j = 0; j < n; j++)
        {
            Ax += matrix[i*n + j]*x[j]; //Ax - произведение конкретной строки на x
        }
        sum += (Ax-b[i])*(Ax-b[i]); //произведение всех доступных строк
    }
    MPI_Reduce(&sum, &buf, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    sum = buf;
    Ax = vec_norm(b, n);    //А тут Ax - сумма квадратов координат доступного rvector
    MPI_Reduce(&Ax, &buf, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        return sqrtl(sum/buf);
    } else {
        return 0;
    }
}