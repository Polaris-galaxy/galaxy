#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "neural_net.h"
#include "training.h"

int main() {
    srand(time(NULL));
    
    // 训练数据
    float X[4][2] = {{0,0}, {0,1}, {1,0}, {1,1}};
    float y[4][1] = {{0}, {1}, {1}, {0}};
    
    // 创建网络
    NeuralNetwork* nn = create_nn(2, 10, 1, 0.01);
    
    // 训练循环
    for (int epoch = 0; epoch < 10000; ++epoch) {
        float total_loss = 0;
        for (int i = 0; i < 4; ++i) {
            float hidden[2], output[1];
            forward(nn, X[i], hidden, output);
            
            float loss = (output[0] - y[i][0]) * (output[0] - y[i][0]);
            total_loss += loss;
            
            train(nn, X[i], y[i]);
        }
        if (epoch % 1000 == 0) {
            printf("Epoch %d Loss: %.4f\n", epoch, total_loss/4);
        }
    }
    
    // 测试
    printf("\nTest results:\n");
    for (int i = 0; i < 4; ++i) {
        float hidden[2], output[1];
        forward(nn, X[i], hidden, output);
        printf("%.0f XOR %.0f => %.2f (expected %.0f)\n", 
              X[i][0], X[i][1], output[0], y[i][0]);
    }
    
    free_nn(nn);
    return 0;
}