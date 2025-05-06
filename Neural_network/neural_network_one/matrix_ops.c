#include "matrix_ops.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

float** create_matrix(int rows, int cols){
    float** matrix = (float**)malloc(rows * sizeof(float*));
    for(int i = 0; i < rows; i++){
        matrix[i] = (float*)malloc(cols * sizeof(float));
    }
     return matrix;
}

void freematrix(float** matrix, int rows){
    for(int i = 0; i < rows; i++){
        free(matrix[i]);
    }

    free (matrix);
}

void initmatrix_weight(float ** matrix, int rows, int cols){
    float stddev = sqrtf(2.0f / cols); // 输入维度倒数
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            matrix[i][j] = (float)rand()/RAND_MAX * stddev;
        }
    }
}

void matrix_vector_mult(float** matrix, float* vector, float*result, int rows, int cols){
    for (int i = 0; i < rows; ++i) {
        result[i] = 0.0f;
        for (int j = 0; j < cols; ++j) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}