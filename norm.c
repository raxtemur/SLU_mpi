#include <math.h>


double vec_norm(double *v, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++)
    {
        s += v[i]*v[i];
    }
    return sqrtl(s);
}

double pogreshnost_norm(double *b, double n)
{
    for (int i = 0; i < n; i++)
        b[i] = b[i] - (i+1)%2;
    return vec_norm(b, n)/sqrtl((n+1)/2);
}

double nesvyazka_norm(double *matrix, double *b, double *x, int n)
{
    double Ax = 0, sum = 0;
    for (int i = 0; i < n; i++)
    {
        Ax = 0;
        for (int j = 0; j < n; j++)
        {
            Ax += matrix[i + n*j]*x[j];
        }
        sum += (Ax-b[i])*(Ax-b[i]);
    }
    return sqrtl(sum)/vec_norm(b, n);    
}