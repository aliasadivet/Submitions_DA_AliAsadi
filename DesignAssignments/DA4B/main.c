/*
 * DA4B.c
 *
 * Created: 4/22/2019 12:46:51 PM
 * Author : Ali Asadi
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void Timer0_init();
void Timer1_init();
void Timer2_init();
void ADC_init();
void Port_init();
uint16_t ReadADC(uint8_t );
void INT0_init();
void Servo();
void Stepper();
void Motor();

const char steps[] = {0x30,0x90,0xC0,0x60}; // winding engerzising sequence
uint16_t delay = 0;                         // Stepper Motor Speed
volatile uint8_t Motor_ON = 0;              // Brushed Motor state

ISR(TIMER0_COMPA_vect)
{
	static int8_t step = 0;                 // initial step
	static uint16_t delay_expired = 0;      // delay timer
	if(delay_expired++ > delay)             // check if specified delay expired
	{
		PORTD = (PORTD&0x0F)|steps[step++]; // Engerzise stepper motor winding
		if(step > 3) step = 0;              // if step exceeds maximum number of sequence then reset
		delay_expired = 0;                  // reset delay timer
	}
}

ISR(INT0_vect)
{
	uint8_t tmp = PIND & _BV(PD2);          // read the PIN D2
	_delay_ms(10);                          // debounce Delay
	if(tmp == (PIND & _BV(PD2)))            // check if the PIN is stable
	Motor_ON = Motor_ON ? 0 : 1 ;		    // if stable toggle the Motor between ON or OFF state
}

int main()
{

	Port_init();                             // init the PORT
	ADC_init();                              // ADC enable
	INT0_init();                             // init the Interrupt for the button
	Timer0_init();                           // Timer 0 for Stepper Motor
	Timer1_init();                           // Timer 1 for Servo Motor
	Timer2_init();                           // Timer 2 for Motor
	sei();                                   // enable global interrupt
	while (1)
	{
		//Motor();
		Servo();
		//Stepper();
	}
	return 0;
}

void Port_init()
{
	DDRB = _BV(PB0)|_BV(PB1)|_BV(PB2)|_BV(PB3);     // DC and Servo Motor Pin Connection
	DDRD = _BV(PD7)|_BV(PD6)|_BV(PD5)|_BV(PD4);     // Stepper Motor pin connection
	PORTB = _BV(PB0);                               // set high
	PORTD = _BV(PD2);
}

void Timer0_init()
{
	TCCR0A = _BV(WGM01);                            // Timer 0 CTC mode
	TCCR0B = _BV(CS01)|_BV(CS00);                   // Clk / 64 , 16M / 64 = 4uS
	OCR0A  = 249;                                   // 4u * 250 = 1mS delay generation
	TIMSK0 = _BV(OCIE0A);                           // Enable Output compare match A interrupt
}

void Timer1_init()
{
	TCCR1A = _BV(COM1B1)|_BV(WGM11)|_BV(WGM10);     //  non-inverting mode, Fast PWM mode
	TCCR1B = _BV(WGM13)|_BV(WGM12)|_BV(CS11)|_BV(CS10);// Fast PWM mode with OCR1A as TOP, clk / 64 = 4uS
	OCR1A  = 4999;                                  // 5000 * 4u = 20mS
}

void Timer2_init()
{
	TCCR2A = _BV(COM2A1)|_BV(WGM20);                // non-inverting mode, Phase Correct PWM
	TCCR2B = _BV(CS21)|_BV(CS20);                   // Clk / 32 = 8uS
}

void ADC_init()
{
	ADMUX = _BV(REFS0);                             // AVcc as Reference
	ADCSRA = _BV(ADEN)|_BV(ADPS0);                  // Enable ADC, Clk / 2
}

uint16_t ReadADC(uint8_t Channel)
{
	ADMUX = (ADMUX & 0xF0)|(Channel & 0x0F);        // Select channel
	ADCSRA |= _BV(ADSC);                            // start conversion
	while(!(ADCSRA & _BV(ADIF)));                   // wait for completion
	ADCSRA |= _BV(ADIF);                            // clear flag
	return ADC;                                     // return the converted value
}

void INT0_init()
{
	EICRA = _BV(ISC01);                             // falling edge
	EIMSK = _BV(INT0);                              // Enable Interrupt 0
}

void Servo()
{
	OCR1B = ((ReadADC(1) / 1024.0) * 400) + 125;    // Read ADC Convert to value in the range of 125 - 525 to get 0.5mS to 2mS
}

void Stepper()
{
	delay = 1024 - ReadADC(2);                      // inverse the read value
}

void Motor()
{
	if(Motor_ON)
	OCR2A = (ReadADC(0) / 1024.0) * 242;        // Motor duty cycle from 0 - 95
	else
	OCR2A = 0;                                  // turn OFF
}

