#include<stdio.h>

void printfMath(int  *a, int number){
    for(int i =0;i< number;i++){
        if(i == number - 1){
            printf("%d", a[i]);
        }else{
            printf("%d ", a[i]);
        }
    }
}