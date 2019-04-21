/*
 * DA3B.c
 *
 * Created: 3/30/2019 11:29:01 AM
 * Author : Ali Asadi
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void Timer_init();
void USART_init();
void ADC_init();
uint16_t readADC();
float readTemperatue(uint16_t);
float celciusToFarenheit(float faren);
void USART_send(char );
void sendString(char *);

char temp[] = "Temperature (F): ";										//change to instruction to print Fahrenheit value
//char temp[] = "Temperature (C): ";											//change to comment to print Fahrenheit value
float temperature ;

ISR(TIMER1_COMPA_vect)
{
	char buffer[8];
	sendString(temp);
	temperature = celciusToFarenheit(readTemperatue(readADC()));			//change to instruction to print Fahrenheit value
	//temperature = readTemperatue(readADC());								//change to comment to print Fahrenheit value
	itoa((int)temperature,buffer,10);
	sendString((char*)buffer);
	sendString("\n\r");
	//sendString(" `F\n\r");
}

int main()
{
	USART_init();
	ADC_init();
	Timer_init();
	sei();
	while (1)
	;
	return 0;
}

void Timer_init()
{
	TCCR1B = 0x0C;         // CTC mode, Prescalar 256
	OCR1A  = 62499;        // generate 1 sec delay
	TIMSK1 = 0x02;         // enable OCIE1A Interrupt
}

void USART_init()
{
	UCSR0B = 0x08;         // TX enable
	UCSR0C = 0x06;         // 8 bit data
	UBRR0L = 51;           // Baud Rate 19200
}

void ADC_init()
{
	ADMUX  = 0x43;          // AVcc reference, channel 3
	ADCSRA = 0x87;          // Enable ADC, set Clk
}

uint16_t readADC()
{
	ADCSRA |= _BV(ADSC);     // Start Conversion
	while(!(ADCSRA & _BV(ADIF)));   // wait for Conversion to happen
	ADCSRA |= _BV(ADIF);     // clear Conversion done Flag
	return ADC;              // return converted ADC value
}

float readTemperatue(uint16_t adc)
{
	float temp = 0;
	temp = (adc * 5.0) / 1023;	// binary to voltage
	temp /= 0.01;				// divide by 10mV to get the temperature
	return temp;				// return Temperature
}

float celciusToFarenheit(float faren)
{
	faren *= 1.8;				// celsius = ( 9/5 * tempertaure ) + 32
	faren += 32;
	return faren;
}

void USART_send(char data)
{
	while(!(UCSR0A & _BV(UDRE0))); // check if the Data register is empty
	UDR0 = data;           // send the data
}

void sendString(char *str)
{
	while(*str)            // send all the data till null Character
	USART_send(*str++);
}


