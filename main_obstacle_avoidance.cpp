/*
 * Obstacal_avoidance car.cpp
 *
 * Created: 13-07-2020 10.55.36 PM
 * Author : abhin
 */ 
#define F_CPU 1000000UL
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define LCD_data PORTB
#define Data_dir_LCD_data DDRB
#define LCD_Control PORTC
#define Data_dir_LCD_Control DDRC
#define LCD_Enable 2
#define LCD_RW 1
#define LCD_RS 0
void LCD_location(uint8_t x,uint8_t y);
char column_first_position[2]={0,64};
void peek_a_boo(void);
void send_a_command(unsigned char command);
void send_a_character(unsigned char Character);
void Check_if_busy(void);
void send_a_string(char *name);
void Timer0_Init(void);
void Sonar_Init(void);
void delay_func(void);
char dis[4];
char r_dis[4];char L_dis[4];
volatile uint16_t Distance=0;
volatile uint16_t Echo=0;
volatile uint16_t Left_Distance=0;
volatile uint16_t Right_Distance=0;
volatile int transiton=0;
int main(void)
{
	Timer0_Init();
	Sonar_Init();
    DDRD|=(1<<PIND1)|(1<<PIND3)|(1<<PIND4)|(1<<PIND6);
	DDRC|=(1<<PINC4)|(1<<PINC5);
	DDRD&=~(1<<PIND2);
	PORTD&=~((1<<PIND3)|(1<<PIND4));
	PORTC&=~((1<<PINC4)|(1<<PINC5));
    
	PORTD&=~(1<<PIND2);
	PORTD&=~(1<<PIND1);
	
	sei();
	Data_dir_LCD_Control |=(1<<LCD_Enable) |(1<<LCD_RS) |(1<<LCD_RW);
	_delay_ms(15);
	

	send_a_command(0x01);
	_delay_ms(2);
	send_a_command(0x38);
	_delay_us(50);
	send_a_command(0x0C);
	_delay_us(50);
	LCD_location(1,2);
	send_a_string("DISTANCE=");
	LCD_location(13,2);
	send_a_string("cm");
	
    while (1) 
    {
		
		_delay_ms(10);
		PORTD|=(1<<PIND1);
		_delay_us(10);
		PORTD&=~(1<<PIND1);
		OCR0A=230;
		_delay_ms(100);
		Distance=Echo/58;
		itoa(Distance,dis,10);
		LCD_location(1,1);
		send_a_string("forward     ");
		LCD_location(10,2);
		send_a_string(dis);
		
		
		
		if(Distance<50)
		{
			PORTD|=(1<<PIND3)|(1<<PIND4);
			PORTC|=(1<<PINC5)|(1<<PINC4);
			//Distance=Echo/58;
			//itoa(Distance,dis,10);
			LCD_location(1,1);
			send_a_string("stop  ");
			LCD_location(10,2);
			send_a_string(dis);
			
			
			OCR0A=247;
			_delay_ms(1000);
			PORTD|=(1<<PIND1);
			_delay_us(10);
			PORTD&=~(1<<PIND1);
			LCD_location(1,1);
			send_a_string("right Scan ");
			_delay_ms(3000);
			Right_Distance=Echo/58;
			itoa(Right_Distance,r_dis,10);
			LCD_location(10,2);
			send_a_string(r_dis);
	       _delay_ms(3000);
			OCR0A=230;
			_delay_ms(1000);
			OCR0A=214;
			_delay_ms(1000);
			PORTD|=(1<<PIND1);
			_delay_us(10);
			PORTD&=~(1<<PIND1);
			LCD_location(1,1);
			send_a_string("Left Scan ");
			_delay_ms(3000);
			Left_Distance=Echo/58;
			itoa(Left_Distance,L_dis,10);
			LCD_location(10,2);
			send_a_string(L_dis);
			_delay_ms(3000);
			OCR0A=230;
			_delay_ms(1000);
			if(Left_Distance >Right_Distance)
			{
				LCD_location(1,1);
				send_a_string("Left turn");
				PORTD|=(1<<PIND4);
				PORTD&=~(1<<PIND3);
				PORTC|=(1<<PINC5);
				PORTC&=~(1<<PINC4);
				Left_Distance=0;
				Right_Distance=0;
				_delay_ms(1000);
			}
			if(Right_Distance>Left_Distance)
			{
				LCD_location(1,1);
				send_a_string("right turn ");
				PORTD&=~(1<<PIND4);
				PORTD|=(1<<PIND3);
				PORTC|=(1<<PINC4);
				PORTC&=~(1<<PINC5);
				Left_Distance=0;
				Right_Distance=0;
				_delay_ms(1000);
			}
		}
		else
		{
			PORTD|=(1<<PIND4 );
			PORTD&=~(1<<PIND3);
			PORTC|=(1<<PINC4);
			PORTC&=~(1<<PINC5);
		}
	
			
    }
}
void Timer0_Init()
{
	TCCR0A|=(1<<COM0A1)|(1<<COM0A0);
	TCCR0A|=(1<<WGM01)|(1<<WGM00);
	TCCR0B|=(1<<CS01)|(1<<CS00);
	OCR0A=230;
}
void Sonar_Init()
{
	EICRA|=(1<<ISC00);
	EIMSK|=(1<<INT0);
	
}
ISR(INT0_vect)
{
	if(transiton==0)
	{
		TCCR1B|=(1<<CS10);
		transiton=1;
	}
	else
	{
		TCCR1B&=~((1<<CS10)|(1<CS11)|(1<<CS12));
		//uint8_t the_lowbit= TCNT1L;
		Echo=TCNT1;//|the_lowbit;
		
		//the_lowbit=0;
		transiton=0;
		TCNT1=0;
		
	}
}

void Check_if_busy()
{
	Data_dir_LCD_data=0x00;
	LCD_Control |=(1<< LCD_RW);
	LCD_Control &=~(1<< LCD_RS);
	
	while(LCD_data>=0x80);
	{
		peek_a_boo();
		peek_a_boo();
		
		
	}
	
	Data_dir_LCD_data=0xFF;
	
}


void peek_a_boo()
{
	LCD_Control |= 1<<LCD_Enable;
	asm volatile("nop");
	asm volatile("nop");
	
	LCD_Control &=~(1<<LCD_Enable);
	
}
void send_a_command(unsigned char command)
{
	Check_if_busy();
	LCD_data=command;
	LCD_Control &=~(1<<LCD_RW);
	LCD_Control &=~(1<<LCD_RS);
	peek_a_boo();
	LCD_data=0;
}
void send_a_character(unsigned char Character)
{
	Check_if_busy();
	LCD_data=Character;
	LCD_Control &=~(1<<LCD_RW);
	LCD_Control |=(1<<LCD_RS);
	peek_a_boo();
	LCD_data=0;
	
}

void send_a_string(char *name)
{
	while(*name>0)
	{
		send_a_character(*name ++);
	}
}

void LCD_location(uint8_t x,uint8_t y)
{
	send_a_command(0x80 + column_first_position[y-1] +(x-1) );
}
void delay_func()
{
	int j=0;
	for(int i=0;i<30000;i++)
	{
		j++;
	}
}