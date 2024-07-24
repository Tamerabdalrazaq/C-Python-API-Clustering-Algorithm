#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define ITER 300
#define BETA 0.5
#define EPSILON 1e-4
#define ERROR_MSG "An Error Has Occurred"

double* sym(double* X, int N, int d);
void printMatrix(double* matrix, int rows, int cols);
void *createArray(int n, int size);
int get_1d_index(int i, int j, int d);
double euc_l2(double *v1, double *v2, int d);
double* calculate_inverse_square_root(double* D, int N);
void matrix_multiply(double* A, double* B, double* C, int N, int d, int K);
double* ddg(double* A, int N);
double* norm(double* A, double* D, int N);
void transpose_matrix(double* A, double* transposed, int N, int d);
double frobenius_norm(double* H, double* H_next, int N, int k);
double* symnmf(double* W, double* H, int N, int k);
void copyArray(double* A, double* B, int size);


#define INITIAL_BUFFER_SIZE 128
/*in this file, we used 1D arrays representing the matrices*/

char* read_line(FILE* file) {
    size_t buffer_size = INITIAL_BUFFER_SIZE;
    size_t length = 0;
    char* buffer = (char*)malloc(buffer_size * sizeof(char));
    int ch;

    if (buffer == NULL) {
        perror(ERROR_MSG);
        exit(EXIT_FAILURE);
    }

    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        buffer[length++] = ch;
        if (length == buffer_size) {
            buffer_size *= 2;
            buffer = (char*)realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL) {
                perror(ERROR_MSG);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (length == 0 && ch == EOF) {
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

void count_rows_and_columns(const char* file_name, int* rows, int* cols) {
    FILE *file;
    char *line;
    int temp_cols;
    char *token;

    file = fopen(file_name, "r");
    if (file == NULL) {
        perror(ERROR_MSG);
        exit(EXIT_FAILURE);
    }

    *rows = 0;
    *cols = 0;

    while ((line = read_line(file)) != NULL) {
        (*rows)++;
        token = strtok(line, ",");
        temp_cols = 0;
        while (token != NULL) {
            temp_cols++;
            token = strtok(NULL, ",");
        }
        if (*cols == 0) {
            *cols = temp_cols;
        } else if (temp_cols != *cols) {
            perror(ERROR_MSG);
            free(line);
            exit(EXIT_FAILURE);
        }
        free(line);
    }

    fclose(file);
}

void read_data(const char* file_name, double* data, int rows, int cols) {
    FILE *file;
    char *line;
    int i = 0;
    char *token;
    rows = rows;
    cols = cols;
    file = fopen(file_name, "r");
    if (file == NULL) {
        perror(ERROR_MSG);
        exit(EXIT_FAILURE);
    }

    while ((line = read_line(file)) != NULL) {
        token = strtok(line, ",");
        while (token != NULL) {
            data[i++] = atof(token);
            token = strtok(NULL, ",");
        }
        free(line);
    }

    fclose(file);
}


int main(int argc, char *argv[]) {
    const char* operation;
    const char* file_name;
    int N, d;
    double* data;
    double* Diagonal;
    double* similarity;
    double* Normalized;

    if (argc != 3) {
        perror(ERROR_MSG);
        return EXIT_FAILURE;
    }

    operation = argv[1];
    file_name = argv[2];

    count_rows_and_columns(file_name, &N, &d);

    data = (double*)malloc((size_t)(N * d) * sizeof(double));
    if (data == NULL) {
        perror(ERROR_MSG);
        return EXIT_FAILURE;
    }

    read_data(file_name, data, N, d);

    if (strcmp(operation, "sym") == 0){
        similarity = sym(data, N, d);
        printMatrix(similarity, N, N);
        free(similarity);
        
    }
    else if (strcmp(operation, "ddg") == 0){
        similarity = sym(data, N, d);
        Diagonal = ddg(similarity, N);
        printMatrix(Diagonal, N, N);
        free(similarity);
        free(Diagonal);

    }
    else if (strcmp(operation, "norm") == 0){
        similarity = sym(data, N, d);
        Diagonal = ddg(similarity, N);
        Normalized = norm(similarity,Diagonal, N);
        printMatrix(Normalized, N, N);
        free(similarity);
        free(Diagonal);
        free(Normalized);
    }

    else{
        perror(ERROR_MSG);
        free(data);
        exit(1);
    }
    free(data);
    
    return 0;
}




/*
Calculates the similarity matrix of X. 
Input: a matrix X with N vectors of size d represented by a 1d array of size N*d
Output: A matrix of dimension N*N represented by a 1d array of size N*N
*/

double* sym(double* X, int N, int d){
    double* A = (double*) createArray(N*N, sizeof(double));
    int i,j;
    for (i = 0; i<N; i++){
        double a_i_j;
        for(j=0; j<N; j++){
            if(i !=j){
                a_i_j = exp(-pow(euc_l2(X + d*i, X + d*j, d), 2)/2 );
            } else{
                a_i_j = 0;
            }
            A[get_1d_index(i,j,N)] = a_i_j;
        }
    }
    return A;
}


/*
Calculates the diagonal degree Matrix
Input: 1d array representing the similarity matrix A, the dimension N
Output: 1d array representing the diagonal degree Matrix D based on A
*/
double* ddg(double* A, int N){
    double* D = (double*) createArray(N*N, sizeof(double));
    int i;
    for(i = 0; i<N; i++){
        double d_i = 0;
        int j;
        for (j=0; j<N; j++)
            d_i += A[get_1d_index(i, j, N)];
        D[get_1d_index(i, i, N)] = d_i;
    }
    return D;
}



/*
Calculates the  normalized similarity matrix
Input: two 1d arrays representing the similarity matrix A and the ddg matrix D , the dimension N
Output: 1d array representing the  normalized similarity matrix D^-1/2AD^-1/2
*/
double* norm(double* A, double* D, int N){
    double* D_invsqrt;
    double* D_invsqrtA;
    double* normalized;
    D_invsqrt = calculate_inverse_square_root(D, N);
    D_invsqrtA = createArray(N*N, sizeof(double));
    matrix_multiply(D_invsqrt, A, D_invsqrtA, N, N, N);
    normalized = createArray(N*N, sizeof(double));
    matrix_multiply(D_invsqrtA, D_invsqrt, normalized, N, N, N);
    free(D_invsqrt);
    free(D_invsqrtA);
    return normalized;
}

/*
Optimizing the randomly generated H
Input: W - 1d array representation of the NxN normalized similarity matrix 
H - 1d array representation of a Nxk randomly initialized matrix
Output: Optimized H
*/


double* symnmf(double* W, double* H, int N, int k ){
    int iter = 0;
    double* H_curr = H;
    double* H_next = createArray(N*k, sizeof(double));
    double* WH = createArray(N*k, sizeof(double));
    double* H_transposed = createArray(N*k, sizeof(double));
    double* HH_T = createArray(N*N, sizeof(double));
    double* HH_TH = createArray(N*k, sizeof(double));

    for(iter=0; iter < ITER; iter ++){
        int i;
        int j;
        double diff;
        matrix_multiply(W, H_curr, WH, N, N, k);
        transpose_matrix(H_curr, H_transposed, N, k);
        matrix_multiply(H_curr, H_transposed, HH_T, N, k, N);
        matrix_multiply(HH_T, H_curr, HH_TH, N, N, k);
        for(i=0; i<N; i++){
            for(j=0; j<k; j++){
                /*check division by zero*/
                double alpha = (1 - BETA  + BETA*( WH[get_1d_index(i, j, k)] / HH_TH[get_1d_index(i, j, k)]));
                H_next[i*k + j] = H_curr[i*k+j] * alpha;
            }
        }
        diff = frobenius_norm(H_curr, H_next, N, k);
        copyArray(H_next, H_curr, N*k);
        if (diff < EPSILON)
            break;
    }

    free(WH);
    free(H_transposed);
    free(HH_T);
    free(HH_TH);
    free(H_next);
    return H_curr;
}



void matrix_multiply(double* A, double* B, double* C, int N, int d, int K) {
    int i,j,k;
    for (i = 0; i < N*K; i++)
                C[i] = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < K; j++) 
            for (k = 0; k < d; k++)
                C[get_1d_index(i,j, K)] += A[get_1d_index(i,k, d)] * B[get_1d_index(k, j, K)];
}

double* calculate_inverse_square_root(double* D, int N) {
    double* D_inv_sqrt = (double*) createArray(N*N, sizeof(double));
    int i,j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            int i_j_index = get_1d_index(i, j, N);
            if (i == j) {
                D_inv_sqrt[i_j_index] = 1.0 / sqrt(D[i_j_index]);
            } else {
                D_inv_sqrt[i_j_index] = 0.0;
            }
        }
    }
    return D_inv_sqrt;
}


int get_1d_index(int i, int j, int d){
    return i*d+j;
}
void transpose_matrix(double* A, double* transposed, int N, int d) {
    int i,j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < d; j++) {
            transposed[j * N + i] = 0;
        }
    }
    for (i = 0; i < N; i++) {
        for (j = 0; j < d; j++) {
            transposed[j * N + i] = A[i * d + j];
        }
    }
}

void copyArray(double* A, double* B, int size) {
    int i;
    for (i = 0; i < size; i++) {
        B[i] = A[i];
    }
}


/*creates an array*/
void *createArray(int n, int size)
{
    void *array = calloc(n, size);
    if (array == NULL)
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    return array;
}


/*calculates the euc distance between two given vectors*/
double euc_l2(double *v1, double *v2, int d)
{
    int i;
    double dist;
    dist = 0.0;
    for (i = 0; i < d; i++)
    {

        dist += pow(v1[i] - v2[i], 2.0);
    }
    return sqrt(dist);
}




void printMatrix(double* matrix, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; ++i) { /*maybe needs to be changed*/
        for (j = 0; j < cols; ++j) { /*maybe needs to be changed*/
            printf("%.4f", matrix[i * cols + j]);
            if (j < cols - 1) printf(",");
        }
        printf("\n");
    }
}



double frobenius_norm(double* H, double* H_next, int N, int k) {
    int i;
    double norm = 0.0;
    for (i = 0; i < N * k; i++) {
        norm += (H_next[i] - H[i]) * (H_next[i] - H[i]);
    }
    return norm;
}
