#include<stdio.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include <stdlib.h>
#define MAX_LEN 1000

int number;
short int position1, position2, position3, position4;

struct student
{
    char *m_student_number;
    char *m_student_name;
    char *m_student_sex;
    int m_student_grade;
};


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

float average(int *list, int size_sum){
    int sum = 0;
    for(int i  = 0; i < size_sum; i++){
        sum += list[i];
    }
    return  (float)sum / size_sum;   
}

int factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

void Bubble_sort(int *list, int size_num) {
    int i, j;
    int temp = 0;
    for (i = 0; i < size_num - 1; i++) {
        for (j = 0; j < size_num - 1 - i; j++) {
            if (list[j] > list[j + 1]) {
                swap(&list[j], &list[j + 1]);
            }
        }
    }
}

void problem_1(int num){
    printf("请输入一个整数: ");
    scanf("%d", &num);
    printf("十进制: %d\n", num);
    printf("无符号十进制: %u\n", (unsigned int)num);
    printf("八进制: %o\n", num);
    printf("带前缀的八进制: %#o\n", num);
    printf("十六进制(小写): %x\n", num);
    printf("带前缀的十六进制(小写): %#x\n", num);
    printf("十六进制(大写): %X\n", num);
    printf("带前缀的十六进制(大写): %#X\n", num);
}

void problem_2(int *number) {
    Bubble_sort(number, 10);
    printf("排序后的数组为：\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", number[i]);
    }
    printf("\n");
}

void problem_3(){
    int student_grade[10];
    int size_sum = sizeof(student_grade) / sizeof(student_grade[0]);
    float average_number = 0.0;
    printf("请输入10个学生的成绩：\n");
    for (int i = 0; i < 10; i++) {
        printf("学生 %d 的成绩：", i + 1);
        scanf("%d", &student_grade[i]);
    }
    average_number = average(student_grade, size_sum);
    for(int i = 0; i < size_sum; i++){
        if (student_grade[i] > average_number) {
            printf("同学%d成绩 %d 大于平均分 %.2f\n", i+1, student_grade[i], average_number);
        } 
    }
}

void problem_4(){
    int list[4];
    printf("请输入一个小于9999的整数：\n");
    scanf("%d", &number);
    if (number > 9999) {
        printf("输入有误\n");
        return;
    }
    
    position1 = number / 1000;
    position2 = number % 1000 / 100;
    position3 = number % 100 / 10;
    position4 = number % 10;

    list[0] = position1;
    list[1] = position2;
    list[2] = position3;
    list[3] = position4;

    for(int i  = 0; i < 4; i++){
        switch (i)
        {
        case 0: printf("千位数：%d\n", list[i]);
            break;
        case 1: printf("百位数：%d\n", list[i]);
            break;
        case 2: printf("十位数：%d\n", list[i]);
            break;
        case 3: printf("个位数：%d\n", list[i]);
            break;
        default: break;
        }
    }
}

void problem_5(int n){
    int *list = (int*)malloc(n * sizeof(int));
    int sum = 0;

    for(int i = 0; i < n; i++){
        list[i] = factorial(2 * i + 1);
    }
    for(int i = 0; i < n; i++){
        sum += list[i];
    }

    printf("从1到2n-1的阶乘之和为：%d\n", sum);
    free(list);
}

void problem_6(){
    struct student students[3];
    for (int i =0 ;i < 3 ;i++){
        students[i].m_student_number = (char *)malloc(20 * sizeof(char));
        students[i].m_student_name = (char *)malloc(50 * sizeof(char));
        students[i].m_student_sex = (char *)malloc(10 * sizeof(char));

        printf("请输入第%d个学生的学号：", i + 1);
        scanf("%s", students[i].m_student_number);
        printf("请输入第%d个学生的姓名：", i + 1);
        scanf("%s", students[i].m_student_name);
        printf("请输入第%d个学生的性别：", i + 1);
        scanf("%s", students[i].m_student_sex);
        printf("请输入第%d个学生的成绩：", i + 1);
        scanf("%d", &students[i].m_student_grade);
    }

    for(int i =0 ;i < 3 ;i++){
        printf("学生 %d 的信息：\n", i + 1);
        printf("学号：%s\n", students[i].m_student_number);
        printf("姓名：%s\n", students[i].m_student_name);
        printf("性别：%s\n", students[i].m_student_sex);
        printf("成绩：%d\n", students[i].m_student_grade);
        free(students[i].m_student_number);
        free(students[i].m_student_name);
        free(students[i].m_student_sex);
    }
}

void problem_7(){
    char buffer[MAX_LEN];
    int ch, i = 0;
    
    printf("请输入字符串（以!结束）: ");
    getchar();
    while ((ch = getchar()) != '!' && i < MAX_LEN - 1) {
        if (ch == EOF) break;
        buffer[i++] = islower(ch) ? toupper(ch) : ch;
    }
    buffer[i] = '\0';
    
    FILE *fp = fopen("test.txt", "w");
    if (fp) {
        fputs(buffer, fp);
        fclose(fp);
        printf("保存成功！共写入 %d 个字符\n", i);
    } else {
        printf("文件创建失败\n");
    }
}

void problem_8() {
    struct Candidate {
        char name[20];
        int votes;
    };
    
    struct Candidate candidates[5] = {
        {"zhang", 0},
        {"wang", 0},
        {"li", 0},
        {"zhao", 0},
        {"liu", 0}
    };
    
    char name[20];
    printf("请输入投票人姓名（输入end结束）:\n");
    while(1) {
        scanf("%s", name);
        if(strcmp(name, "end") == 0) break;
        
        for(int i = 0; i < 5; i++) {
            if(strcmp(name, candidates[i].name) == 0) {
                candidates[i].votes++;
                break;
            }
        }
    }
    
    printf("投票结果:\n");
    for(int i = 0; i < 5; i++) {
        printf("%s: %d票\n", candidates[i].name, candidates[i].votes);
    }
}

void problem_9() {
    struct Node {
        int num;
        struct Node* next;
    };
    
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    head->next = NULL;
    struct Node* tail = head;
    
    int n, num;
    printf("创建链表（输入节点数）: ");
    scanf("%d", &n);
    
    printf("输入节点数据:\n");
    for(int i = 0; i < n; i++) {
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        scanf("%d", &newNode->num);
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    
    printf("输入要统计的值: ");
    scanf("%d", &num);
    
    int count = 0;
    struct Node* current = head->next;
    while(current != NULL) {
        if(current->num == num) {
            count++;
        }
        current = current->next;
    }
    
    printf("值为%d的节点个数: %d\n", num, count);
    
    current = head->next;
    while(current != NULL) {
        struct Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(head);
}

void problem_10() {
    int score;
    printf("输入学生成绩: ");
    scanf("%d", &score);
    
    if(score < 0 || score > 100) {
        printf("无效成绩!\n");
        return;
    }
    
    char grade;
    if(score >= 90) grade = 'A';
    else if(score >= 80) grade = 'B';
    else if(score >= 70) grade = 'C';
    else if(score >= 60) grade = 'D';
    else grade = 'E';
    
    printf("成绩等级: %c\n", grade);
}

void problem_11() {
    double r1, r2;
    printf("输入两个电阻值: ");
    scanf("%lf %lf", &r1, &r2);
    
    double series = r1 + r2;
    double parallel = (r1 * r2) / (r1 + r2);
    
    printf("串联电阻: %.2fΩ\n", series);
    printf("并联电阻: %.2fΩ\n", parallel);
}

void problem_12() {
    int a, b;
    printf("输入两个整数: ");
    scanf("%d %d", &a, &b);
    
    printf("交换前: a=%d, b=%d\n", a, b);
    
    int temp = a;
    a = b;
    b = temp;
    
    printf("交换后: a=%d, b=%d\n", a, b);
}

void problem_13() {
    double num;
    printf("输入一个浮点数: ");
    scanf("%lf", &num);
    
    double result = round(num * 100) / 100;
    
    printf("处理结果: %.2f\n", result);
}

void problem_14() {
    int count = 0;
    printf("2000-3000年间的闰年:\n");
    
    for(int year = 2000; year <= 3000; year++) {
        if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            printf("%d ", year);
            count++;
            
            if(count % 10 == 0) {
                printf("\n");
            }
        }
    }
    
    printf("\n闰年总数: %d\n", count);
}

void problem_15() {
    printf("3-100之间的素数:\n");
    int count = 0;
    
    for(int n = 3; n <= 100; n++) {
        int isPrime = 1;
        
        for(int i = 2; i <= n/2; i++) {
            if(n % i == 0) {
                isPrime = 0;
                break;
            }
        }
        
        if(isPrime) {
            printf("%d ", n);
            count++;
            if(count % 10 == 0) printf("\n");
        }
    }
    printf("\n共%d个素数\n", count);
}

void problem_16() {
    #define PI 3.14159
    
    double r;
    int k;
    printf("输入半径和操作类型(1:面积, 2:周长, 3:两者): ");
    scanf("%lf %d", &r, &k);
    
    switch(k) {
        case 1:
            printf("面积: %.2f\n", PI * r * r);
            break;
        case 2:
            printf("周长: %.2f\n", 2 * PI * r);
            break;
        case 3:
            printf("面积: %.2f, 周长: %.2f\n", PI * r * r, 2 * PI * r);
            break;
        default:
            printf("无效操作类型!\n");
    }
}

void problem_17() {
    int steps = 7;
    
    while(1) {
        if((steps % 2 == 1) && 
           (steps % 3 == 2) && 
           (steps % 5 == 4) && 
           (steps % 6 == 5)) {
            break;
        }
        steps += 7;
    }
    
    printf("阶梯总数: %d\n", steps);
}

void problem_18() {
    int n;
    printf("输入自然数n: ");
    scanf("%d", &n);
    
    int cube = n * n * n;
    int start = n * n - n + 1;
    
    printf("%d^3 = %d = ", n, cube);
    for(int i = 0; i < n; i++) {
        printf("%d", start + 2*i);
        if(i < n-1) printf(" + ");
    }
    printf("\n");
}

int main(){
    int number_text;
    printf("测试1\n");
    problem_1(number_text);

    printf("\n测试2\n");
    int number_list[10] = {5, 3, 8, 6, 2, 9, 1, 4, 7, 0};
    problem_2(number_list);

    printf("\n测试3\n");
    problem_3();

    printf("\n测试4\n");
    problem_4();

    printf("\n测试5\n");
    int n;
    printf("请输入n的值: ");
    scanf("%d", &n);
    problem_5(n);

    printf("\n测试6\n");
    problem_6();

    printf("\n测试7\n");
    problem_7();

    printf("\n测试8\n");
    problem_8();

    printf("\n测试9\n");
    problem_9();

    printf("\n测试10\n");
    problem_10();

    printf("\n测试11\n");
    problem_11();

    printf("\n测试12\n");
    problem_12();

    printf("\n测试13\n");
    problem_13();

    printf("\n测试14\n");
    problem_14();

    printf("\n测试15\n");
    problem_15();

    printf("\n测试16\n");
    problem_16();

    printf("\n测试17\n");
    problem_17();

    printf("\n测试18\n");
    problem_18();
    
    return 0;
}