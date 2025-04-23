#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INPUT_SIZE 3
#define HIDDEN_SIZE 4
#define OUTPUT_SIZE 2
#define LEARNING_RATE 0.1
#define EPOCHS 10000

// 神经网络结构体
typedef struct {
    // 权重矩阵
    double w1[INPUT_SIZE][HIDDEN_SIZE];
    double w2[HIDDEN_SIZE][OUTPUT_SIZE];
    
    // 偏置项
    double b1[HIDDEN_SIZE];
    double b2[OUTPUT_SIZE];
} NeuralNetwork;

// 激活函数（Sigmoid）
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// 激活函数导数
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

// 初始化神经网络
void initialize_network(NeuralNetwork* net) {
    srand(time(0));
    
    // 初始化权重和偏置
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            net->w1[i][j] = ((double)rand() / RAND_MAX) * 2 - 1; // [-1, 1]
        }
    }
    
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        net->b1[i] = ((double)rand() / RAND_MAX) * 2 - 1;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            net->w2[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }
    
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        net->b2[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
}

// 前向传播
void forward_pass(NeuralNetwork* net, double* input,
                 double* hidden, double* output) {
    // 计算隐藏层
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        hidden[j] = 0;
        for (int i = 0; i < INPUT_SIZE; i++) {
            hidden[j] += input[i] * net->w1[i][j];
        }
        hidden[j] += net->b1[j];
        hidden[j] = sigmoid(hidden[j]);
    }
    
    // 计算输出层
    for (int j = 0; j < OUTPUT_SIZE; j++) {
        output[j] = 0;
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            output[j] += hidden[i] * net->w2[i][j];
        }
        output[j] += net->b2[j];
        // 输出层使用线性激活（回归任务）
    }
}

// 均方误差损失函数
double mse_loss(double* predicted, double* target) {
    double loss = 0;
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        loss += pow(predicted[i] - target[i], 2);
    }
    return loss / OUTPUT_SIZE;
}

// 反向传播
void backward_pass(NeuralNetwork* net, double* input, double* hidden,
                   double* output, double* target) {
    // 计算输出层梯度
    double output_error[OUTPUT_SIZE];
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        output_error[i] = (output[i] - target[i]);
    }
    
    // 计算隐藏层梯度
    double hidden_error[HIDDEN_SIZE] = {0};
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        for (int k = 0; k < OUTPUT_SIZE; k++) {
            hidden_error[j] += output_error[k] * net->w2[j][k];
        }
        hidden_error[j] *= sigmoid_derivative(hidden[j]);
    }
    
    // 更新权重和偏置
    // 更新输出层权重
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            net->w2[i][j] -= LEARNING_RATE * output_error[j] * hidden[i];
        }
    }
    
    // 更新输出层偏置
    for (int j = 0; j < OUTPUT_SIZE; j++) {
        net->b2[j] -= LEARNING_RATE * output_error[j];
    }
    
    // 更新输入层权重
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            net->w1[i][j] -= LEARNING_RATE * hidden_error[j] * input[i];
        }
    }
    
    // 更新隐藏层偏置
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        net->b1[j] -= LEARNING_RATE * hidden_error[j];
    }
}

int main() {
    NeuralNetwork net;
    initialize_network(&net);
    
    // 训练数据示例
    double inputs[][INPUT_SIZE] = {{0.1, 0.2, 0.3}};
    double targets[][OUTPUT_SIZE] = {{0.5, 0.8}};
    
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        double total_loss = 0;
        
        for (int sample = 0; sample < 1; sample++) { // 这里只使用一个样本
            double hidden[HIDDEN_SIZE];
            double output[OUTPUT_SIZE];
            
            // 前向传播
            forward_pass(&net, inputs[sample], hidden, output);
            
            // 计算损失
            double loss = mse_loss(output, targets[sample]);
            total_loss += loss;
            
            // 反向传播
            backward_pass(&net, inputs[sample], hidden, output, targets[sample]);
        }
        
        if (epoch % 1000 == 0) {
            printf("Epoch %d, Loss: %.4f\n", epoch, total_loss);
        }
    }
    
    // 测试训练后的网络
    double test_input[INPUT_SIZE] = {0.1, 0.2, 0.3};
    double hidden[HIDDEN_SIZE];
    double output[OUTPUT_SIZE];
    forward_pass(&net, test_input, hidden, output);
    
    printf("\nFinal prediction: [%.4f, %.4f]\n", output[0], output[1]);
    
    return 0;
}