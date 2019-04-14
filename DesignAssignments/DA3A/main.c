/*
 * DA3A.c
 *
 * Created: 3/28/2019 12:46:34 AM
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
void USART_send(char );
void sendString(char *);

volatile char temp[] = "Random:)\t";				// a random string
int randomInt[] = {1,2,689,48,25,1235,2548,322,465,415,471};
float temperature = 20.432 ;						//just a number to generate a random floating point

ISR(TIMER1_COMPA_vect)								//Interrupt Service Routine
{
	char buffer[8];
	sendString(temp);
	itoa(rand(),buffer,10);							//generate a random integer
	sendString(buffer);
	sendString("\t");
	dtostrf(temperature*rand(),2,4,buffer);			//generate a random floating point value
	sendString(buffer);
	sendString("\n\r");
}

int main()
{
	USART_init();
	Timer_init();
	sei();			//set global interrupt
	while (1);
	return 0;
}



void Timer_init()
{
	TCCR1B = 0x0C;		//timer mode CTC, clock division 256 (WGM12 (CTC mode) CS12 CS11 CS10 (1, 0, 0 : clkI/O/256 (From prescaler)))
	OCR1A  = 62500;		//to generate 1 sec delay
	TIMSK1 = 0x02;		//OCIE0=1 (OCIE1A=1)over flow output compare match A interrupt enable
}

void USART_init()
{
	UCSR0B = 0x08;		//Transmit enable
	UCSR0C = 0x06;		//databit= 8bit 
						//Transmitted Data Changed (Output of TxDn Pin): Rising XCKn Edge
						//Received Data Sampled (Input on RxDnPin): Falling XCKn Edge
						//UCSZn1 UCSZn0 UCPOLn : 1, 1, 0
	UBRR0L = 51;		//buadrate value=19200
}


void USART_send(char data)
{
	while(!(UCSR0A & _BV(UDRE0)));		//wait for data register to be empty
	UDR0 = data;						//send data
}

void sendString(char *str)				
{
	while(*str)							//loop till end
	USART_send(*str++);					//send data
}