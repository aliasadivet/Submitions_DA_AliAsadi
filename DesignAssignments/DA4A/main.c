/*
 * DA4A.c
 *
 * Created: 4/13/2019 11:49:01 AM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void Timer2_init();
void ADC_init();
void Port_init();
uint16_t ReadADC(uint8_t );
void INT0_init();
void Motor();


volatile uint8_t Motor_ON = 0;


ISR(INT0_vect)
{
	uint8_t tmp = PIND & _BV(PD2);          // read the PIN D2
	_delay_ms(10);                          // debounce Delay
	if(tmp == (PIND & _BV(PD2)))            // check if the PIN is stable
	Motor_ON = Motor_ON ? 0 : 1 ;			// if stable toggle the Motor between ON or OFF state
	while (!(PIND & _BV(PD2)));
}

int main()
{
	Port_init();                            // Initiate the PORT
	ADC_init();								// Initiate the analog to digital converter
	INT0_init();                            // Initiate the Interrupt 0 for the button
	Timer2_init();                          // Using Timer 2 for Motor
	sei();                                  // Enable global interrupt
	while (1)
	{
		Motor();
	}
	return 0;
}

void Port_init()
{
	DDRB = _BV(PB0)|_BV(PB1)|_BV(PB3);		// DC Motor Pin Connection
	PORTB = _BV(PB0);						// set high
	PORTD = _BV(PD2);						//external interrupt 0
}

void Timer2_init()
{
	TCCR2A = _BV(COM2A1)|_BV(WGM20);		// non-inverting mode, Phase Correct PWM
	TCCR2B = _BV(CS21)|_BV(CS20);			// Clk / 32 = 8uS
}

void ADC_init()
{
	ADMUX = _BV(REFS0);						// AVcc as Reference
	ADCSRA = _BV(ADEN)|_BV(ADPS0);          // Enable ADC, Clk / 2
}

uint16_t ReadADC(uint8_t Channel)
{
	ADMUX = (ADMUX & 0xF0)|(Channel & 0x0F);  // Select channel
	ADCSRA |= _BV(ADSC);                      // start conversion
	while(!(ADCSRA & _BV(ADIF)));             // wait for completion
	ADCSRA |= _BV(ADIF);                      // clear flag
	return ADC;                               // return the converted value
}

void INT0_init()
{
	EICRA = _BV(ISC01);		// The falling edge of INT0 generates an interrupt request.
	EIMSK = _BV(INT0);		// External Interrupt Request 0 Enable
}

void Motor()
{
	if(Motor_ON)
	OCR2A =(ReadADC(0) / 1024.0) * 243;		// Makes the max speed of the motor 95% of PWM value. (256*95%=243)
	else
	OCR2A = 0;								// Turn off
}


