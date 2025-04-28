#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// 定义二叉树节点结构
typedef struct Node {
    char data;
    struct Node *left;
    struct Node *right;
} Node;

// 函数声明
int precedence(char op);
void infixToPostfix(char* infix, char* postfix);
Node* createExpressionTree(char* postfix);
int evaluate(Node* root);
void freeTree(Node* root);

int main() {
    while (1) {
        char infix[100] = {0};
        int i = 0;
        int c;
        
        while ((c = getchar()) != '=' && c != EOF) {
            if (isspace(c)) continue;
            if (i < 99) infix[i++] = c;
        }
        
        // 处理结束条件
        if (i == 0 && c == '=') {
            break;
        } else if (c == EOF) {
            break;
        }
        
        // 转换中缀为后缀表达式
        char postfix[100] = {0};
        infixToPostfix(infix, postfix);
        
        // 创建表达式树
        Node* root = createExpressionTree(postfix);
        
        // 计算并输出结果
        int result = evaluate(root);
        printf("%d\n", result);
        
        // 释放内存
        freeTree(root);
        
        // 清空输入缓冲区
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return 0;
}

// 计算运算符优先级
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 中缀转后缀表达式
void infixToPostfix(char* infix, char* postfix) {
    char stack[100];
    int top = -1;
    int pIndex = 0;
    
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];
        
        if (isdigit(c)) {
            postfix[pIndex++] = c;
        } else if (c == '(') {
            stack[++top] = c;
        } else if (c == ')') {
            while (top >= 0 && stack[top] != '(') {
                postfix[pIndex++] = stack[top--];
            }
            top--; // 弹出左括号
        } else {
            while (top >= 0 && precedence(stack[top]) >= precedence(c)) {
                postfix[pIndex++] = stack[top--];
            }
            stack[++top] = c;
        }
    }
    
    // 弹出栈中剩余运算符
    while (top >= 0) {
        postfix[pIndex++] = stack[top--];
    }
    postfix[pIndex] = '\0';
}

// 创建表达式树
Node* createExpressionTree(char* postfix) {
    Node* stack[100];
    int top = -1;
    
    for (int i = 0; postfix[i] != '\0'; i++) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->data = postfix[i];
        newNode->left = newNode->right = NULL;
        
        if (isdigit(postfix[i])) {
            stack[++top] = newNode;
        } else {
            newNode->right = stack[top--];
            newNode->left = stack[top--];
            stack[++top] = newNode;
        }
    }
    return stack[top];
}

// 递归计算表达式树
int evaluate(Node* root) {
    if (!root) return 0;
    if (isdigit(root->data)) return root->data - '0';
    
    int left = evaluate(root->left);
    int right = evaluate(root->right);
    
    switch (root->data) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return left / right;
        default: return 0;
    }
}

// 释放二叉树内存
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}