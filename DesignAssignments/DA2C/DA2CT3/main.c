/*
 * DA2C-T3.c
 *
 * Created: 3/22/2019 7:30:02 PM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

volatile uint16_t tick;

ISR(TIMER0_COMPA_vect)
{
	tick++;             // increment delay counter
}

void delay_CTC(volatile uint16_t ms)
{
	tick = 0;           // counter = 0
	ms >>= 1;           // divide by 2 since timer generates a delay of 2.048mS for each increment
	while(tick < ms);   // wait counter to complete given time delay
}

void Pulse_Init_CTC()
{
	DDRB |= _BV(2);
}

void Timer_Init_CTC()
{
	TCCR0A = 0x02;
	TCCR0B = 0x03;
	TIMSK0 = 0x02;
	OCR0A  = 249;
	sei();
}

void Pulse_CTC()
{
	PORTB |= _BV(2);
	delay_CTC(435);
	PORTB &= ~_BV(2);
	delay_CTC(290);
}

void LED_Init_CTC()
{
	DDRD &= ~_BV(2);
	PORTD |= _BV(2);
	DDRB |= _BV(2);
	PORTB |= _BV(2);
	
}

void LED_CTC()
{
	static uint8_t pressed = 0;
	if((PIND & _BV(2)) && pressed)
	{
		delay_CTC(10);
		if(PIND & _BV(2))
		{
			PORTB &= ~_BV(2);
			delay_CTC(1250);
			PORTB |= _BV(2);
		}
		pressed = 0;
	}
	else if(!(PIND & _BV(2)))
	{
		delay_CTC(10);
		if(!(PIND & _BV(2)))
		pressed = 1;
	}
}


void main()
{
	//Pulse_Init_CTC();
	LED_Init_CTC();
	Timer_Init_CTC();
	while(1)
	//Pulse_CTC();
	LED_CTC();
	
}

