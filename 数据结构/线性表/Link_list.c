//链表
#include<stdio.h>

struct Node
{
    int data;
    struct Node* next;
};

void createList(struct Node* head, int n){
    struct Node* p = head;
    for (int i = 0; i < n; i++)
    {
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->data = i + 1;
        newNode->next = NULL;
        p->next = newNode;
        p = newNode;
    }
}

void printList(struct Node* head){
    struct Node* p = head->next;
    while (p != NULL)
    {
        printf("%d ", p->data);
        p = p->next;
    }
    printf("\n");
}

void freeList(struct Node* head){
    struct Node* p = head;
    while (p != NULL)
    {
        struct Node* temp = p;
        p = p->next;
        free(temp);
    }
}

int main(){
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    head->next = NULL;

    int n;
    printf("Enter the number of nodes: ");
    scanf("%d", &n);

    createList(head, n);
    printList(head);
    freeList(head);

    return 0;
}