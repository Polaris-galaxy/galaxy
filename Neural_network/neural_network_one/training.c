#include "training.h"
#include<stdio.h>
#include<stdlib.h>

void forward(NeuralNetwork* nn, float *input, float* hidden, float* output){

    matrix_vector_mult(nn->W1, input, hidden, nn->hidden_size, nn->input_size);
    for (int i = 0; i < nn->hidden_size; ++i) {
        hidden[i] += nn->b1[i];
        hidden[i] = sigmoid(hidden[i]);
    }//隐藏层

    matrix_vector_mult(nn->W2, hidden, output, nn->output_size, nn->hidden_size);
    for (int i = 0; i < nn->output_size; ++i) {
        output[i] += nn->b2[i];
        output[i] = sigmoid(output[i]);
    }//输出层
}

void train(NeuralNetwork* nn, float* input, float* target){
    float* hidden = (float*)malloc(nn->hidden_size * sizeof(float));
    float* output = (float*)malloc(nn->output_size * sizeof(float));
    
    forward(nn, input, hidden, output);
    
    // 计算输出层误差
    float* delta_output = (float*)malloc(nn->output_size * sizeof(float));
    for (int i = 0; i < nn->output_size; ++i) {
        float error = output[i] - target[i];
        delta_output[i] = error * sigmoid_derivative(output[i]);
    }
    
    // 计算隐藏层误差
    float* delta_hidden = (float*)malloc(nn->hidden_size * sizeof(float));
    for (int i = 0; i < nn->hidden_size; ++i) {
        float error = 0.0f;
        for (int j = 0; j < nn->output_size; ++j) {
            error += nn->W2[j][i] * delta_output[j];
        }
        delta_hidden[i] = error * sigmoid_derivative(hidden[i]);
    }
    
    // 更新权重和偏置
    for (int i = 0; i < nn->output_size; ++i) {
        for (int j = 0; j < nn->hidden_size; ++j) {
            nn->W2[i][j] -= nn->learning_rate * delta_output[i] * hidden[j];
        }
        nn->b2[i] -= nn->learning_rate * delta_output[i];
    }
    
    for (int i = 0; i < nn->hidden_size; ++i) {
        for (int j = 0; j < nn->input_size; ++j) {
            nn->W1[i][j] -= nn->learning_rate * delta_hidden[i] * input[j];
        }
        nn->b1[i] -= nn->learning_rate * delta_hidden[i];
    }
    
    free(hidden);
    free(output);
    free(delta_output);
    free(delta_hidden);
}