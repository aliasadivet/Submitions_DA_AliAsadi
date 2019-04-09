/*
 * DA2C-T1.c
 *
 * Created: 3/16/2019 6:41:15 AM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>



void delay_poll(volatile uint16_t ms)
{
	ms >>= 1;                           // divide by 2 since timer generates a delay of 2.048mS for each increment
	ms -= 4;                            // subract 4 to compensate 0.048mS
	while(ms--)                         // wait till given time delay expire
	{
		while(!(TIFR0 & _BV(TOV0)));    // wait for timer to complete 2.048mS
		TIFR0 |=_BV(TOV0);              // clear timer flag
	}
}

void Pulse_Init_poll()
{
	DDRB |= _BV(2);			// PORTB Pin 2 Output if bit in DDRx register is set
}

void Timer_Init_poll()
{
	TCCR0B = 0x03;			//clock select bits in Timer/Counter Control Register (clkI/O/64 (From prescaler))
}

void Pulse_poll()
{
	PORTB &= ~_BV(2);
	delay_poll(435);
	PORTB |= _BV(2);
	delay_poll(290);
}

void LED_Init_poll()
{
	DDRD &= ~_BV(2);
	PORTD |= _BV(2);
	PORTB |= _BV(2);
	DDRB |= _BV(2);
}

void LED_poll()
{
	static uint8_t pressed = 0;			// variable to store previous value of the PIN
	if((PIND & _BV(2)) && pressed)		// if PINx register is used to read the voltage level on that Pin
	{
		delay_poll(10);
		if(PIND & _BV(2))
		{
			PORTB &= ~_BV(2);
			delay_poll(1250);
			PORTB |= _BV(2);
		}
		pressed = 0;
	}
	else if(!(PIND & _BV(2)))			// if current state is LOW
	{
		delay_poll(10);
		if(!(PIND & _BV(2)))
		pressed = 1;
	}
}

void main()
{
	//Pulse_Init_poll();		//change to comment when executing second part of the task (just for convenience)
	LED_Init_poll();		//change to code instruction when executing second part of the task
	Timer_Init_poll();
	while(1)
	//Pulse_poll();			//change to comment when executing second part of the task (just for convenience)
	LED_poll();			//change to code instruction when executing second part of the task	
}
