#include <reg52.h>
#include <QX_A11.h>
#include <intrins.h>
bit Timer1Overflow;	
unsigned char disbuff[4]={0,0,0,0};
unsigned int LeftDistance = 0, RightDistance = 0, FrontDistance = 0; 
unsigned int DistBuf[5] = {0};
unsigned char	pwm_val_left,pwm_val_right;	
unsigned char 	pwm_left,pwm_right;			
unsigned char mode;
#define		PWM_DUTY		200			
#define		PWM_HIGH_MIN	70			
#define		PWM_HIGH_MAX	PWM_DUTY	

void Timer0_Init(void); 
void Timer1_Init(void);
void LoadPWM(void);
void Delay_Ms(unsigned int ms);
void forward(unsigned char LeftSpeed,unsigned char RightSpeed); 
void left_run(unsigned char LeftSpeed,unsigned char RightSpeed);  
void right_run(unsigned char LeftSpeed,unsigned char RightSpeed);
void back_run(unsigned char LeftSpeed,unsigned char RightSpeed);
void stop(void);
unsigned int QXMBOT_GetDistance(void);
void QXMBOT_TrigUltrasonic(void);
void Tracking();
void UltrasonicAvoid(unsigned int val);
void ULT_TrakAvoid(unsigned int val);
void ULT_IRAvoid(unsigned int val);

void main(void)
{
	Timer0_Init();
	Timer1_Init();
	IT1 = 1;                        
    EX1 = 1;                        
	EA_on;	
	while(1)
	{
		switch(mode)
		{
			case 0:	 LED1=1,LED2=1; UltrasonicAvoid(300); break; 
			case 1:	 LED1=0,LED2=1; ULT_IRAvoid(300); break;  
			case 2:	 LED1=1,LED2=0; ULT_TrakAvoid(200); break;
			case 3:	 LED1=0,LED2=0; Tracking(); break;
			default: stop(); 	 break;
		}		
	}	
}

void exint1() interrupt 2           
{
	if(KEY==0)
	{
		stop();
		Delay_Ms(10); 
		if(KEY==0)
		{
			mode++;
			while(!KEY);
		}
	}
	if(mode > 3)	mode = 0;
}

void forward(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;
	left_motor_go; 
	right_motor_go; 
}

void left_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;
	left_motor_back; 
	right_motor_go; 	
}

void right_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;
	right_motor_back;
	left_motor_go;    
}

void back_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;
	right_motor_back;
	left_motor_back; 
}

void stop(void)
{
	left_motor_stops;
	right_motor_stops;
}

void Delay_Ms(unsigned int ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 96000; 
		  while(--i);   
     }while(--ms);
}

void LoadPWM(void) 
{
	if(pwm_left > PWM_HIGH_MAX)		pwm_left = PWM_HIGH_MAX;	
	if(pwm_left < PWM_HIGH_MIN)		pwm_left = PWM_HIGH_MIN;	
	if(pwm_right > PWM_HIGH_MAX)	pwm_right = PWM_HIGH_MAX;	
	if(pwm_right < PWM_HIGH_MIN)	pwm_right = PWM_HIGH_MIN;	
	if(pwm_val_left<=pwm_left)		Left_moto_pwm = 1;  
	else Left_moto_pwm = 0; 						    
	if(pwm_val_left>=PWM_DUTY)		pwm_val_left = 0;	

	if(pwm_val_right<=pwm_right)	Right_moto_pwm = 1; 
	else Right_moto_pwm = 0; 							
	if(pwm_val_right>=PWM_DUTY)		pwm_val_right = 0;	
}

void QXMBOT_TrigUltrasonic() //超声波模块触发函数
{
	TrigPin = 0; 
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	TrigPin = 1; 
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	TrigPin = 0; 
}

unsigned int QXMBOT_GetDistance() //超声波模块
{
	unsigned int Distance = 0;	
	unsigned int  Time=0;		
	QXMBOT_TrigUltrasonic();	
	while(!EchoPin);  	
	Timer1On;			
	while(EchoPin);		
	Timer1Off;			
	Time=TH1*256+TL1;	
	TH1=0;
	TL1=0;				
    Distance = (float)(Time*1.085)*0.17;
	return(Distance);			
}

void UltrasonicAvoid(unsigned int val) //超声波避障
{
	unsigned int Dis;
	Delay_Ms(60);
	Dis = QXMBOT_GetDistance();
	if(Dis < val)
	{	
		do{
			stop();	
			beep = 0;
			Delay_Ms(100);
			beep = 1;
			back_run(120, 120);
			Delay_Ms(50);
			left_run(120, 120);
			Delay_Ms(100);
			Dis = QXMBOT_GetDistance();
		}while(Dis < val);
		forward(120, 120);
		Delay_Ms(60);
	}
	else
	{
		forward(120, 120);	
	}
}

void ULT_TrakAvoid(unsigned int val) //超声波+循迹避障
{
    unsigned int i;
    unsigned int Dis;
    for(i=0; i<6000; i++)		Tracking();
    Dis = QXMBOT_GetDistance();
    if(Dis < val)
{
		do{
			stop();	
			beep = 0;
			Delay_Ms(100);
			Dis = QXMBOT_GetDistance();
		}while(Dis < val);
		beep = 1;
	}
}

void ULT_IRAvoid(unsigned int val) //红外+超声波避障
{
	unsigned int Dis;
	unsigned int i;
	for(i=0; i<6000; i++)
	{
		if(right_led2 == 0)
		{
			left_run(120,120);
		}
		if(left_led2 == 0)
		{
			right_run(120,120);
		}
	}
	Dis = QXMBOT_GetDistance();
	if(Dis < val)
	{	
		do{
			stop();	
			beep = 0;
			Delay_Ms(100);
			beep = 1;
			back_run(120, 120);
			Delay_Ms(50);
			left_run(120, 120);
			Delay_Ms(100);
			Dis = QXMBOT_GetDistance();
		}while(Dis < val);
		forward(120, 120);
		Delay_Ms(60);
	}
	else
	{
		forward(120, 120);	
	}
}

void Tracking() //循迹
{
	char data1,data2 = left_led1,data3 = right_led1;
	data1 = data2*10+data3;
	if(data1 == 11)
	{
		forward(120,120);
	}
	else
	{
	 	if(data1 == 10)
		{
			left_run(80,160);
		}
		if(data1 == 1)
		{
			right_run(160,80);
		}
		if(data1 == 0)
		{
			stop();	
		}
	}
}

void Timer0_Init(void)
{
	TMOD |= 0x02;
	TH0 = 164;
	TL0 = 164;
	TR0 = 1;
	ET0 = 1;	
}

void Timer1_Init(void)		
{
	TMOD |= 0x10;	
	TH1 = 0;		   
    TL1 = 0;
	ET1 = 1;             
}
 
void timer0_int (void) interrupt 1
{
	 pwm_val_left++;
	 pwm_val_right++;
	 LoadPWM();
}

void tm1_isr() interrupt 3 using 1
{
	Timer1Overflow = 1;	
	EchoPin = 0;		
}	
