/*
 * PWM_Motor_controll.cpp
 *
 * Created: 09-07-2020 1.54.21 PM
 * Author : abhin
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
void Start_Conversion(void);
void PWM_INIT(void);
void ADC_INIT(void);
uint16_t DutyCycle=0;
int button_pressed=0;
int pressed_confidence=0;
int released_confidence=0;
int main(void)
{
	DDRB|=(1<<PINB1)|(1<<PINB6)|(1<<PINB7);
	DDRC&=~(1<<PINC4);
	//PORTB|=(1<<PINB6);
	//PORTB|=(1<<PINB7);
	PORTB|=((0<<PINB6)|(0<<PINB7));
	PORTC|=(1<<PINC4);
	int count=0;
	PWM_INIT();
	ADC_INIT();
	Start_Conversion();
	sei();
	 while (1) 
    {
		
		
		OCR1A=DutyCycle;
		if(bit_is_clear(PINC,4))
		{
			pressed_confidence ++;
			
			if(pressed_confidence>1000)
			{
				if(button_pressed == 0)
				{
					count++;
					if(count==1)
					{
						button_pressed=1;
						PORTB |=(1<<PINB7);
						PORTB &=~(1<<PINB6);
						
					}
					if(count==2)
					{
						
						button_pressed=1;
						PORTB &=~(1<<PINB7);
						PORTB |=(1<<PINB6);
						
					}
					if(count==3)
					{
						button_pressed=1;
						PORTB &=~(1<<PINB7);
						PORTB &=~(1<<PINB6);
						count=0;
					}
					pressed_confidence=0;	
				}		
			}
		}
				
			else
			{
				released_confidence ++;
				if(released_confidence>1000)
				{
					button_pressed =0;
					released_confidence=0;
					
				}
			}					
    }
}
void PWM_INIT()
{
	TCCR1A|=(1<<COM1A1)|(1<COM1A0);
	TCCR1A|=(1<<WGM10)|(1<<WGM11);
	TCCR1B|=(1<<WGM12)|(1<<CS10);
}
void ADC_INIT()
{
	ADMUX|=(1<<REFS0)|(1<<ADLAR);
	ADMUX|=(1<<MUX0)|(1<<MUX2);
	ADCSRA|=(1<<ADEN)|(1<<ADIE)|(1<<ADPS2);//        |(1<<ADPS1);
	DIDR0|=(1<<ADC5D);
	//TIMSK1|=(1<<OCIE1A);
}
void Start_Conversion()
{
	ADCSRA|=(1<<ADSC);
}
ISR(ADC_vect)
{
	uint8_t Low_bit=(ADCL>>6);
	DutyCycle=(ADCH<<2)|(Low_bit);
	Start_Conversion();	
}
//ISR(TIMER1_COMPA_vect)
//{
	//OCR1A=DutyCycle;
//}
