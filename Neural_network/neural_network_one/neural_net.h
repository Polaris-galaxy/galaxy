#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "matrix_ops.h"

typedef struct {
    int input_size;
    int hidden_size;
    int output_size;
    float learning_rate;
    
    float** W1;//权重矩阵(隐藏层尺寸 x 输入尺寸)
    float* b1;//隐藏层偏置(隐藏层尺寸)
    float** W2;//权重矩阵(输出尺寸 x 隐藏层尺寸)
    float* b2;//输出层偏置(输出尺寸)
} NeuralNetwork;

NeuralNetwork* create_nn(int input_size, int hidden_size, int output_size, float lr);
void free_nn(NeuralNetwork* nn);

#endif