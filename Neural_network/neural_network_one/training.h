#ifndef TRAINING_H
#define TRAINING_H

#include "neural_net.h"
#include "activations.h"

void forward(NeuralNetwork* nn, float* input, float* hidden, float* output);
void train(NeuralNetwork* nn, float* input, float* target);

#endif