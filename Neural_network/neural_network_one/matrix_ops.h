#ifndef MAXTRIX_OPS_H
#define MAXTRIX_OPS_H

float** create_matrix(int rows, int cols);
void freematrix(float** matrix, int rows);
void initmatrix_weight(float** matrix, int rows, int cols);
void matrix_vector_mult(float** matrix, float* vector, float* result, int rows, int cols);

#endif // MAXYRIX_OPS_H