/*************************************************************
© 2024 - 2024 José Novais & Tiago Diogo - All Rights Reserved.
Last Updated : 01/09/2024 18:52:37
**************************************************************/

#include <REG51F380.H>
#include <stdio.h>
#include "types.h"
#include "lcd1602.h"
#include <intrins.h>

sbit PB1 = P0^6;
sbit trig = P1^0;
sbit echo = P1^1;
sbit buzz = P1^2;
unsigned int distance=0;
// LCD ADDRESS 0x27
#define LCD_ADDR_W 0x4E
#define LCD_ADDR_R 0x4F

/*******************
 * function prototypes 																	 *
 *******************/
void init_device(void);

 
/*******************/	
void init_device(void){
	P0MDOUT   = 0x04;
	PCA0MD    = 0x00;
	CLKSEL    = 0x02;
	XBR1      = 0x40;
	P1MDOUT |= (1 << 2); // Define o pino P1^2 como saída
}
	void timer2_init(void){
		// sysclock /12
		TMR2CN = 0;
		#define B_T2MH 5	
		
		//Timer 2 high byte T2MH and low byte T2ML use the system clock
		CKCON &= ~((1 << B_T2MH));
		TMR2H = 0;
		TMR2L = 0;	
}

/*******************
 * main loop			  																		 *
 *******************/

void main(){
	int rc;
	
	unsigned int i = 0;
	
	init_device();
	timer2_init();
	buzz = 0;
	trig=1;
	
	TMR2H = 0;
	TMR2L = 0;	

	rc = lcd1602Init(LCD_ADDR_W);
	
	lcd1602Clear(LCD_ADDR_W);	    // clear the memory
			
	lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
		
	lcd1602SetCursor(LCD_ADDR_W, 0, 0);
		
	lcd1602WriteString(LCD_ADDR_W, "Distancia:");
	
		while(1){
			char str[6]={0};
			buzz = 0;
			trig=0;
			while(echo==0 && i<5000){
				i++;
			}
			i = 0;
			
			TR2 = 1;
			while(echo==1);
			TR2 = 0;

			trig = 1;
			
			distance=TMR2H<<8;
			distance+=TMR2L;
			distance*=0.5;
			distance=distance*0.034/2;
			TMR2H = 0;
			TMR2L = 0;	
					
			sprintf(str,"%d",distance);
			
			if(distance<300){
					
				lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
			
				lcd1602SetCursor(LCD_ADDR_W, 10,0);
		
				lcd1602WriteString(LCD_ADDR_W, "   ");
			  
				lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
			
				lcd1602SetCursor(LCD_ADDR_W, 10,0);
		
				lcd1602WriteString(LCD_ADDR_W, str);
			
				lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
		
				lcd1602SetCursor(LCD_ADDR_W, 14, 0);
		
				lcd1602WriteString(LCD_ADDR_W, "cm");
				
				lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
		
				lcd1602SetCursor(LCD_ADDR_W,4, 1);
		
				lcd1602WriteString(LCD_ADDR_W, "                ");
			}
			
			if(distance<20){

				buzz = 1;
				lcd1602Control(LCD_ADDR_W, 1,0,1); // backlight on, underline on, blink block on 
		
				lcd1602SetCursor(LCD_ADDR_W,4, 1);
		
				lcd1602WriteString(LCD_ADDR_W, "CAUTION!");
			}
		}
}
