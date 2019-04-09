/*
 * DA2A.c
 *
 * Created: 2/27/2019 4:54:25 AM
 * Author : Ali Asadi
 */ 

#include <avr/io.h>


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>



void Pin_Init()
{
	DDRB |= _BV(2);     // PORTB Pin 2 Output if bit in DDRx register is set
	// then corresponding Pin will be output
	PORTB |= _BV(2);    // Set Pin in PortB High if bit in PORTx is Set then corresponding Pin is HIGH
	
}                       // _BV(x) = (1<<x) ; _Bv(1) = 1<<1 = 00000010

void Pulse()
{
	PORTB &= ~_BV(2);   // clear the bit in PORTb , to set the output low
	_delay_ms(435);     // delay for 435 ms, built-in function
	PORTB |= _BV(2);    // Set Pin in PortB High if bit in PORTx is Set then corresponding Pin is HIGH
	_delay_ms(290);     // delay for 290 ms , total of 725mS
}

void Switch_Init()
{
	DDRD &= ~_BV(2);    // Clearing DDRx register set that pin to INPUT mode
	PORTD |= _BV(2);    // setting PORTx, for an INPUT pin , enables the pull-up resistor
	DDRB |= _BV(2);     // PORTB pin 2 output
	PORTB |= _BV(2);    // Set Pin in PortB High if bit in PORTx is Set then corresponding Pin is HIGH
	
}

void LED()
{
	static uint8_t pressed = 0;     // variable to store previous value of the PIN
	if((PIND & _BV(2)) && pressed)  // if PINx register is used to read the voltage level on that Pin
	{
		_delay_ms(10);              // if current State is high, and previous state is low , delay 10ms
		if(PIND & _BV(2))           // for Debonuce
		{                           // if the pin maintains same state as before than Pin stable
			PORTB &= ~_BV(2);       // Clear the PORTD pin 2
			_delay_ms(1250);        // delay 1.250 Secs
			PORTB |= _BV(2);        // Set the PORTD pin 2
		}
		pressed = 0;                // Previous state value to high
	}
	else if(!(PIND & _BV(2)))       // if current state is LOW
	{
		_delay_ms(10);              // debounce delay
		if(!(PIND & _BV(2)))        // still low
		pressed = 1;            // previous value is LOW
	}
}
void main()
{
	//Pin_Init();			//change to comment when executing second part of the task (just for convenience)
	Switch_Init();			//change to code instruction when executing second part of the task
	while (1)
	{
		//Pulse();			//change to comment when executing second part of the task (just for convenience)
		LED();				//change to code instruction when executing second part of the task
	}
}