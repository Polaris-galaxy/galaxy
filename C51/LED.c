#include<reg51.h>

sbit P17 = P1^7;

void delay(unsigned int time){
    unsigned int i, j;
    for(i = 0; i < time; i++){
        for (j = 0 ; j < 120; j++){
            // Do nothing, just wait
        }
    }
}

void main(){
    while(1){
        P17 = 0;
        delay(1000);
        P17 = 1;
        delay(1000);
    }
}