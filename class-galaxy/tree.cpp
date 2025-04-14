#include<iostream>
#include<string>
#include<cstdlib>
#include<vector>

using namespace std;

class Node{
    public:
        int data;
        Node* left;
        Node* right;
    
    public:
        Node(int value) : data(value), left(nullptr), right(nullptr) {};
        ~Node(){} ;

        int getData() {return data;};
};

void preOrder(Node *node){
    if (node == nullptr) return ;
    cout << node->getData() << " ";
    preOrder(node->left);
    preOrder(node->right);
}

void deleteTree(Node *node){
    if(node == nullptr) return ;
    deleteTree(node -> left);
    deleteTree(node -> right);
    delete node;
}

int main(){
    Node *root = new Node(1);
    root->left = new Node(2);
    root->right = new Node(3);
    root->left->left = new Node(4);
    root->left->right = new Node(5);

    cout << "Preorder traversal: ";
    preOrder(root);
    cout << endl;

    deleteTree(root); // Free the allocated memory
    return 0;
}