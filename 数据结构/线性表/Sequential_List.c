//顺序表
#include<stdio.h>

struct Sequential_List
{
    int *data; //数据
    int length; //当前长度
    int max_size; //最大长度
};

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void init_list(struct Sequential_List *list, int max_size)
{
    list->data = (int *)malloc(max_size * sizeof(int)); //分配内存
    list->length = 0; //初始化长度
    list->max_size = max_size; //设置最大长度
}

void find_element(struct Sequential_List *list, int index)
{
    if(index <= 0|| index > list->length +1){
        printf("寻找位置不合法\n");
        return;
    }

    printf("寻找位置的数据是%d\n",list->data[index - 1]);
    return;
};

void find_position(struct Sequential_List *list, int data)
{
    int flag = 0;
    for(int i = 0; i < list->length; i++){
        if(list->data[i] == data){
            printf("数据%d在第%d个位置\n",data,i + 1);
            flag = 1;
            break;
        }
    }

    if(flag == 0){
        printf("数据%d不在顺序表中\n",data);
    }
    return;
}

void sort_list(struct Sequential_List *list)
{
    for(int i = 0; i < list->length - 1; i++){
        for(int j = 0; j < list->length - i - 1; j++){
            if(list->data[j] > list->data[j + 1]){
                swap(&list->data[j], &list->data[j + 1]);
            }
        }
    }
}

void insert_list(struct Sequential_List *list, int data, int index){
    if(list->length >= list->max_size){
        printf("顺序表已满，无法插入数据\n");
        return;
    }
    if(index <= 0 || index > list -> length + 1){
        printf("插入位置不合法\n");
        return;
    }
    for(int i = list->length; i>=index;i--){
        list->data[i] = list->data[i - 1];
    }
    list->data[index - 1] =data;
    list->length++;
}

void delete_list(struct Sequential_List *list, int index){
    if(index <= 0 || index > list->length){
        printf("删除位置不合法\n");
        return;
    }
    for(int i = index - 1; i < list->length - 1; i++){
        list->data[i] = list->data[i + 1];
    }
    list->length--;
}

void print_list(struct Sequential_List *list)
{
    for(int i = 0; i < list->length; i++){
        printf("%d ",list->data[i]);
    }
    printf("\n");
}

int main(){
    struct Sequential_List *list;
    printf("请输入顺序表的最大长度：");
    scanf("%d",&list->max_size);
    init_list(list, list->max_size);
    printf("请输入顺序表的长度：");
    scanf("%d",&list->length);
    for(int i = 0; i < list->length; i++){
        printf("请输入第%d个数据：",i + 1);
        scanf("%d",&list->data[i]);
    }
    find_element(list, 2);
    find_position(list, 3);
    sort_list(list);
    printf("排序后的顺序表：");
    print_list(list);
    free(list ->data);
    return 0;
}