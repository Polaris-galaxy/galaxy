#include<stdio.h>
#include<math.h>
#include "activations.h"

float sigmoid(float x){
    return 1 / (1 + exp(-x));
}

float sigmoid_derivative(float x){
    float s = sigmoid(x);
    return s * (1 - s);
}