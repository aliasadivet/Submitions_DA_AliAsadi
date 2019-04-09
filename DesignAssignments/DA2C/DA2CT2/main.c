/*
 * DA2C-T2.c
 *
 * Created: 3/20/2019 4:13:12 AM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

volatile uint16_t tick;

ISR(TIMER0_OVF_vect)
{
	tick++;             // increment delay counter
}

void delay_ovf(volatile uint16_t ms)
{
	tick = 0;           // counter = 0
	ms >>= 1;           // divide by 2 since timer generates a delay of 2.048mS for each increment
	ms -=  4;           // subract 4 to compensate 0.048mS
	while(tick < ms);   // wait counter to complete given time delay
}

void Pulse_Init_ovf()
{
	DDRB |= _BV(2);
}

void Timer_Init_ovf()
{
	TCCR0B = 0x03;      // CS = 3 8Mhz/64
	TIMSK0 = 0x01;      // enble Timer Overflow interrupt
	sei();              // enable global interrupt
}

void Pulse_ovf()
{
	PORTB |= _BV(2);
	delay_ovf(435);
	PORTB &= ~_BV(2);
	delay_ovf(290);
}

void LED_Init_ovf()
{
	DDRD &= ~_BV(2);
	PORTD |= _BV(2);
	DDRB |= _BV(2);
	PORTB |= _BV(2);
}

void LED_ovf()
{
	static uint8_t pressed = 0;
	if((PIND & _BV(2)) && pressed)
	{
		delay_ovf(10);
		if(PIND & _BV(2))
		{
			PORTB &= ~_BV(2);
			delay_ovf(1250);
			PORTB |= _BV(2);
		}
		pressed = 0;
	}
	else if(!(PIND & _BV(2)))
	{
		delay_ovf(10);
		if(!(PIND & _BV(2)))
		pressed = 1;
	}
}



void main()
{
	//Pulse_Init_ovf();
	LED_Init_ovf();
	Timer_Init_ovf();
	while(1)
	//Pulse_ovf();
	LED_ovf();
	
}

