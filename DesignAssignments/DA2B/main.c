/*
 * DA2B.c
 *
 * Created: 3/6/2019 2:56:14 AM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

ISR(INT0_vect)
{
	PORTB &= ~_BV(2);       // Clear the PORTD pin 2
	_delay_ms(1250);        // delay 1.250 Secs
	PORTB |= _BV(2);        // Set the PORTD pin 2
}

void int_init()
{
	DDRB  |= _BV(2);        // PORTB pin 2 output
	PORTB |= _BV(2);        // Set the PORTD pin 2
	DDRD  &= ~_BV(2);       // Clearing DDRx register set that pin to INPUT mode
	PORTD |= _BV(2);        // setting PORTx, for an INPUT pin , enables the pull-up resister
	
	EICRA |= 0x03;          // EICRA to configure the edge of the incoming pulse 0x11 or 3 - Rising edge
	EIMSK |= 1;             // Enable INT0
	
	sei();                  // Enable global Interrupt
}

void main()
{
	int_init();
	while (1);
}

