#include "neural_net.h"
#include<stdio.h>
#include<stdlib.h>

NeuralNetwork* create_nn(int input_size, int hidden_size, int output_size, float lr){
    NeuralNetwork* nn = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    nn->input_size = input_size;
    nn->hidden_size = hidden_size;
    nn->output_size = output_size;
    nn->learning_rate = lr;

    nn->W1 = create_matrix(hidden_size, input_size);
    initmatrix_weight(nn->W1, hidden_size, input_size);

    nn->b1 = (float*)malloc(hidden_size * sizeof(float));
    for (int i = 0; i < hidden_size; ++i) {
        nn->b1[i] = (float)rand() / RAND_MAX - 0.5f;
    }

    nn->W2 = create_matrix(output_size, hidden_size);
    initmatrix_weight(nn->W2, output_size, hidden_size);
    
    nn->b2 = (float*)malloc(output_size * sizeof(float));
    for (int i = 0; i < output_size; ++i) {
        nn->b2[i] = (float)rand() / RAND_MAX - 0.5f;
    }
    
    return nn;
}

void free_nn(NeuralNetwork* nn) {
    free(nn->W1);
    free(nn->b1);
    free(nn->W2);
    free(nn->b2);
    free(nn);
}