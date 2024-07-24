#ifndef FUNCTIONS_H
#define FUNCTIONS_H

double* sym(double* X, int N, int d);
double* ddg(double* A, int N);
double* norm(double* A, double* D, int N);
double* symnmf(double* W, double* H, int N, int k );

#endif
