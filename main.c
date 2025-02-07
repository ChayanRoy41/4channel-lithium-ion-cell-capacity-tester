/*******************************/
/*** Created by Ovidiu Sabau ***/
/***	7th January 2022	 ***/
/*******************************/
//Define-----------------
#define F_CPU 8000000UL
/*----------------- Include -------------------*/
//#include "LCD.h"
//#include "LCD.c"
#include "lcd.h"
#include "lcd.c"
#include <avr/io.h>
#include  <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/eeprom.h>

//Macro ------------------------
#define VCC   5.0
#define NORMAL_MODE 0x10
#define SET_UP_MODE 0x20
#define STOP        0x30
//----------------------------
//PFP-----------------------------------
void adc_init();
int adc_read(int ch);
float read_current();
float capacity_calculation(float disch_car);
void pwmfunc(int dc);
void empyt_buffer();
float get_setting_parameter(void);
//-------------------------------------
//GV---------------------------------
//Battery calculations-----------------
float v1;//battery voltage
float c1;//battery current
float capacity;//battery capacity
int adc_value;
int time_=0;
int prev_time=0;
 char buff[10];
 char buff1[10];
 char buff2[10];
 char buff3[10];
 char buff4[10];
 char mode=NORMAL_MODE;
 float cuttoffvolt=0.00;
 int duration=0.00;
 uint8_t test_complete=0;
 uint8_t test_duration=1;
float c_1=0;
float c_2=0;
float c_3=0;
float c_4=0;
float v_1=0;
float v_2=0;
float v_3=0;
float v_4=0;
	char v_arr[100];
	char v_arr1[100];
	char v_arr2[100];
	char v_arr3[100];
//----------------------------------
//Battery parameter----------------------
float battery_rated_current;
float discarge_current;
//--------------------------------------
//Timer variable-----------------------
uint8_t hr=0,min=0,sec=0;
//------------------------------------
//ADC functions------------------------------
void adc_init()
{
	ADMUX=(1<<REFS0);
	//ADMUX = (0<<REFS0) | (0<<REFS1);
	
	// ADC Enable and prescaler of 128
	// 8000000/128 = 62500
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}
int adc_read(int ch)
{
	// select the corresponding channel 0~7
	// ANDing with '7' will always keep the value
	// of 'ch' between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}
float read_voltage(){
	adc_value=0;
	adc_value=adc_read(0);
	v1=adc_value*5;
	v1=v1/1024;
	v1=v1*2;
	/*if(v1<=cuttoffvolt || hr>=duration){
	PORTB |=(1<<0);
	TCCR1B=(0<<CS10)|(0<<CS12);
	
	}*/
	return v1;
	
	
}
//Read discharge current--------------------
float read_current(){
	int adc_buffer[20];
	adc_value=0;
	for(uint8_t l=0;l<20;l++){
	adc_buffer[l]=adc_read(0);
	adc_value=adc_value+adc_buffer[l];
	_delay_ms(10);
	}
	c1=adc_value/20;
	c1=c1*5;
	c1=c1/1024;
	c1=c1/1;
	for(uint8_t p=0;p<20;p++){
		adc_buffer[p]=0;
	}
	return c1;
}
//Calculate capacity-------------------
float capacity_calculation(float disch_car){
	
	// time_=(int)((hr * 3600) + (min * 60) + sec);

	// capacity=disch_car/battery_rated_current;
	// capacity=time_*disch_car/3600;
	// capacity=capacity*100;//-(capacity*100);
   duration=(hr * 3600) + (min * 60) +sec;
   capacity=(float)(duration *(disch_car*1000)/3600);// / 3600.0;
   	
   return capacity;
	
}
//VArying pwm -------------------
void pwmfunc(int dc){
	OCR2A = dc;
	TCCR2A = (1<<COM2A1) | (1<<WGM20) | (1<<WGM21)|(1<<WGM22);
	TCCR2B = (1<<CS22) | (1<<CS20);
}
//Empty buffer-------------
void empyt_buffer(){
	
	for(uint8_t y=0;y<10;y++){
		buff[y]=0;
		buff1[y]=0;
		buff2[y]=0;
		buff3[y]=0;
	}
	
	for(uint8_t t=0;t<100;t++){
		//v_arr[t]=0;
	}
}
//Set Parameter and return---------------
float get_setting_parameter(void){
	
	adc_value=0;
	adc_value=adc_read(1);
	v1=adc_value*5;
	v1=v1/1024;
	return v1;
	
}
float get_dc_current(void){
	float v2=0;
	
	int adc_result=adc_read(1);
	v2=adc_result*5;
	v2=v2/1024;
	return v2;
	
}
//Initial voltage check------------------------
void initial_voltage_check(void){

	for(uint8_t f=0;f<5;f++){
		PORTC |=(1<<5);
		PORTC &=~(1<<4);
		PORTC &=~(1<<3);
		v_1=read_voltage();
		_delay_ms(10);
		//dtostrf(read_voltage(), 4,1,buff);
		PORTC |=(1<<5);
		PORTC &=~(1<<4);
		PORTC |=(1<<3);
		_delay_ms(10);
		v_2=read_voltage();
		//  dtostrf(read_voltage(), 4,1,buff1);
		PORTC |=(1<<5);
		PORTC |=(1<<4);
		PORTC &=~(1<<3);
		v_3=read_voltage();
		_delay_ms(10);
		//  dtostrf(read_voltage(), 4,1,buff2);
		PORTC |=(1<<5);
		PORTC |=(1<<4);
		PORTC |=(1<<3);
		v_4=read_voltage();
		_delay_ms(10);
		
	}

    dtostrf(v_1, 4,1,buff);
  dtostrf(v_2, 4,1,buff1);
  dtostrf(v_3, 4,1,buff2);
 dtostrf(v_4, 4,1,buff3);
 sprintf(v_arr,"%s%s%s","V1:",buff,"V");
  sprintf(v_arr1,"%s%s%s","V2:",buff1,"V");
   sprintf(v_arr2,"%s%s%s","V3:",buff2,"V");
    sprintf(v_arr3,"%s%s%s","V4:",buff3,"V");
	if(v_1<4.1 || v_2<4.1 || v_3 <4.1 || v_4<4.1){
	    mode=STOP;
		lcd_gotoxy(0,0);
		lcd_puts(v_arr);
		lcd_gotoxy(0,1);
		lcd_puts(v_arr1);
		lcd_gotoxy(0,2);
		lcd_puts(v_arr2);
		lcd_gotoxy(0,3);
		lcd_puts(v_arr3);
	}
	else{
		mode=NORMAL_MODE;
		
			
			PORTB &=~(1<<0);
			PORTB &=~(1<<1);
			PORTB &=~(1<<2);
			PORTB &=~(1<<3);
			TCCR1B=(0<<CS10)|(0<<CS12);
			TCNT1=56000;
			sec=0;
			min=0;
			hr=0;
			TCCR1B=(1<<CS10)|(1<<CS12);
		
	}
	for(uint8_t f=0;f<100;f++){
		
		v_arr[f]=0;
		v_arr1[f]=0;
		v_arr1[f]=0;
		v_arr1[f]=0;
	}
}
/*-------------- Main function ----------------*/
int main(void)
{   
	DDRB |=(1<<0);//CH4
	DDRB |=(1<<1);//Ch3
	DDRB |=(1<<2);//CH2
	DDRB |=(1<<3);//CH1
	DDRB &=~(1<<7);
	DDRC &=~(1<<2);
	//DDRC |=(1<<2);
	DDRC |=(1<<3);
	DDRC |=(1<<4);
	DDRC |=(1<<5);
	float voltage1 =0; 
	float voltage2 =0;
    float voltage3 =0;
    float voltage4 =0;   
	float current1 = 0; 
	float current2= 0; 
	float current3 =0; 
	float current4= 0; 
	float capacity1=0;
	float capacity2=0;
	float capacity3=0;
	float capacity4=0;
 
	uint8_t set_voltage =1;
	uint8_t set_discharge_current=2;
	uint8_t set_rated_current=3;
	uint8_t set_duration_in_hr=4;
	uint8_t next=set_voltage;
	uint8_t b1_complete=0;
    uint8_t b2_complete=0;
	uint8_t b3_complete=0;
	uint8_t b4_complete=0;
	
	 if(cuttoffvolt>3){
	  cuttoffvolt=0;
	 }
	 if(discarge_current>4){
		 discarge_current=0;
	 }
	 if(duration>6){
		duration=0;
	 }
	 cuttoffvolt=2.7;
	 battery_rated_current=3;
  	// clearScreen();
    // pwmfunc(10);

	
	sei();

	//initLCD();
    adc_init();
	PORTB |=(1<<0);
	PORTB |=(1<<1);
	PORTB |=(1<<2);
	PORTB |=(1<<3);
	  TCNT1=65000;
	  TIMSK1 |=(1<<TOIE1);
		//TCCR1B=(1<<CS10)|(1<<CS12);
		//PORTB &=~(1<<0);
	//updateLCDScreen(row on LCD, "Text 1", number, "Text 2");
	lcd_init(LCD_DISP_ON); /*initialize lcd,display on, cursor on */

	lcd_clrscr();             /* clear screen of lcd */

   
	
while(1){
	if(mode !=STOP){
initial_voltage_check();
	}
     	if(!(PINC & (1<<2))){
	      	_delay_ms(100);
	      	if(!(PINC & (1<<2))){
		      	
		      	
				 mode=SET_UP_MODE;
				lcd_clrscr(); 
				empyt_buffer();
				while(!(PINC & (1<<2)));
	      	}
	      	while(mode==SET_UP_MODE){
	      	dtostrf(get_dc_current(),2,1,buff4);
			sprintf(v_arr,"%s",buff4);
			  lcd_gotoxy(0,0);
			  lcd_puts("DischargeCurrent");
			  lcd_gotoxy(6,1);
			  lcd_puts(v_arr);
			  	if(!(PINC & (1<<2))){
				  	_delay_ms(100);
				  	if(!(PINC & (1<<2))){
						  lcd_clrscr(); 
					while(!(PINC & (1<<2)));
					for(uint8_t z=0;z<10;z++){
						buff4[z]=0;
					}
				
						 mode=NORMAL_MODE; 
						 PORTB &=~(1<<3);
						 PORTB &=~(1<<2);
						 PORTB &=~(1<<0);
						 PORTB &=~(1<<1); 
						 TCCR1B=(1<<CS10)|(1<<CS12);
					  }
					  }
			  }
      	}
		  /*if(mode==STOP){
			  lcd_clrscr();
			  lcd_gotoxy(7,3);
			  lcd_puts("Test Complete");
			  _delay_ms(2000);
			  
		  }*/
		 
		  if(mode==NORMAL_MODE){
			  if(hr>=4){
				  
					PORTB |=(1<<0);
					PORTB |=(1<<1);
					PORTB |=(1<<2);
					PORTB |=(1<<3);
					TCCR1B=(1<<CS10)|(1<<CS12);  
				  
			  }
			//  if(hr>=test_duration){
				  
			//	  test_complete=1;
				  
			//  }
			 /// if(test_complete==0){
	//***********************************************************************************************			 
		empyt_buffer();
        PORTC |=(1<<5);
		PORTC &=~(1<<4);
		PORTC &=~(1<<3);
		v_1=read_voltage();;
	    //dtostrf(read_voltage(), 4,1,buff);
		PORTC |=(1<<5);
		PORTC &=~(1<<4);
		PORTC |=(1<<3);
		v_2=read_voltage();
	  //  dtostrf(read_voltage(), 4,1,buff1);
		PORTC |=(1<<5);
		PORTC |=(1<<4);
		PORTC &=~(1<<3);
		v_3=read_voltage();
	  //  dtostrf(read_voltage(), 4,1,buff2);
		PORTC |=(1<<5);
		PORTC |=(1<<4);
		PORTC |=(1<<3);
		v_4=read_voltage();
	   // dtostrf(read_voltage(), 4,1,buff3);
	    //sprintf(v_arr,"%s%s%s%s%s%s%s%s",buff," ",buff1," ",buff2," ",buff3);
     	//updateLCDScreen(1,v_arr,(double)voltage1,(double)current1,(double)capacity1);
	
		if(v_1>cuttoffvolt){
			   PORTC &=~(1<<5);
			   PORTC &=~(1<<4);
			   PORTC &=~(1<<3);
			   c_1= read_current();
		dtostrf(capacity_calculation(c_1),1,1,buff);
		//dtostrf(get_dc_current(),2,1,buff4);
	    sprintf(v_arr,"%s%s","A1:",buff);
		}
		else{
			if(b1_complete==0){
				empyt_buffer();
				b1_complete=1;
			capacity1=capacity_calculation(c_1);
			}
			  dtostrf(capacity1,1,1,buff);
			// dtostrf(get_dc_current(),2,1,buff4);
			 sprintf(v_arr,"%s%s","C1:",buff);
		}
           lcd_gotoxy(0,0);
           lcd_puts(v_arr);
          
		//updateLCDScreen(1,v_arr,(double)voltage1,(double)current1,(double)capacity1);
		empyt_buffer();
		if(v_2>cuttoffvolt){
				PORTC &=~(1<<5);
				PORTC &=~(1<<4);
				PORTC |=(1<<3);
				c_2=read_current();
		dtostrf((c_2*1000),1,1,buff1);
	   	dtostrf(hr, 1,0,buff);
		sprintf(v_arr,"%s%s%s%s%s","A2:",buff1,"  ","H:",buff);
		//sprintf(v_arr,"%s%s%s","A2:",buff1,"mAH");
		}
		else{
			if(b2_complete==0){
				empyt_buffer();
				b2_complete=1;
				capacity2=capacity_calculation(c_2);
				
			}
			dtostrf(capacity2,1,1,buff1);
			dtostrf(hr,1,0,buff);
		sprintf(v_arr,"%s%s%s%s%s","C2:",buff1,"  ","H:",buff);
			//sprintf(v_arr,"%s%s%s","C2:",buff1,"mAH");
			
		}
		 lcd_gotoxy(0,1);
		 lcd_puts(v_arr);
		
		///updateLCDScreen(2,v_arr,(double)voltage1,(double)current1,(double)capacity1);
		empyt_buffer();
		if(v_3>cuttoffvolt){
			PORTC &=~(1<<5);
			PORTC |=(1<<4);
			PORTC &=~(1<<3);
			c_3=read_current();
		dtostrf(min,2,0,buff1);	
		dtostrf((c_3*1000),1,1,buff2);
		sprintf(v_arr,"%s%s%s%s%s","A3:",buff2,"  ","M:",buff1);
		//sprintf(v_arr,"%s%s%s","A3:",buff2,"mAH");
		}
		else{
			if(b3_complete==0){
				empyt_buffer();
				b3_complete=1;
				capacity3=capacity_calculation(c_3);
			}
			dtostrf(min,2,0,buff1);
			dtostrf(capacity3,1,1,buff2);
			sprintf(v_arr,"%s%s%s%s%s","C3:",buff2,"  ","M:",buff1);
			//sprintf(v_arr,"%s%s%s","C3:",buff2,"mAH");
		}
		 lcd_gotoxy(0,2);
		 //lcd_gotoxy(-4,2);
		 lcd_puts(v_arr);
		
		//updateLCDScreen(3,v_arr,(double)voltage1,(double)current1,(double)capacity1);
		empyt_buffer();
		if(v_4>cuttoffvolt){
			PORTC &=~(1<<5);
			PORTC |=(1<<4);
			PORTC |=(1<<3);
			c_4=read_current();
			
		//dtostrf(sec,2,0,buff2);
		dtostrf((c_4*1000),1,1,buff3);
	    sprintf(v_arr,"%s%s","A4:",buff3);
		//sprintf(v_arr,"%s%s%s","A4:",buff3,"mAH");
		
		}
		else{
			if(b4_complete==0){
				empyt_buffer();
				b4_complete=1;
				capacity4=capacity_calculation(c_4);
				
			}
			//dtostrf(sec,2,0,buff2);
			dtostrf(capacity4,1,1,buff3);
			sprintf(v_arr,"%s%s","C4:",buff3);
			 //sprintf(v_arr,"%s%s%s","C4:",buff3,"mAH");
		}
		// lcd_gotoxy(-4,3);
		 lcd_gotoxy(0,3);
		 lcd_puts(v_arr);
		//updateLCDScreen(4,v_arr,(double)voltage1,(double)current1,(double)capacity1);
		empyt_buffer();
		//*****************************************************************************************************
		  }
			
		
}
	return 0;
}

ISR(TIMER1_OVF_vect){
	sec++;
	
	if(sec>59){
		min++;
	    sec=0;
	}
	if(min>59){
		hr++;
		min=0;
 
	}
	
	dtostrf(sec,2,0,buff4);
	sprintf(v_arr1,"%s%s","S:",buff4);
	lcd_gotoxy(7,3);
	lcd_puts(v_arr1);
    for(uint8_t w=0;w<10;w++){
		buff4[w]=0;
	}
	TCNT1=65000;//original 56000
	
}
	