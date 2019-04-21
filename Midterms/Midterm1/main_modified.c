/*
 * Midterm1.c
 *
 * Created: 4/9/2019 2:59:42 AM
 * Author : Ali Asadi
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <String.h>
#include <util/delay.h>

void Timer_init();
void USART_init();
void ADC_init();
uint16_t readADC();
float readTemperatue(uint16_t);
void USART_send(char );
void sendString(const char *);
void ESP_init();
void ESP_sendData(int);
char receiveString();
char receiveString(char* ,const char* );
float celciusToFarenheit(float);

char buffer[128];
const char ssid[] = "\"D798BE\"",	// The SSID (name) of the Wi-Fi network I connect to
pwd[]       = "\"72C26B2B20500\"",	// The password of the Wi-Fi network
ok[]        = "\r\nOK\r\n",			// OK respond after connection 
sendOK[]    = "\r\nSEND OK\r\n",
error[]     = "ERROR",
apiKey[]    = "1SFM68R6DZI4P8UG",	// My channel's Write API Key
wifiMode[]  = "AT+CWMODE_CUR=3",
connectWiFi[] = "AT+CWJAP_CUR=",
openTCP[]   = "AT+CIPSTART=",
dataSend[]  = "AT+CIPSEND=",
closeTCP[]  = "AT+CIPCLOSE",
link[]      = "GET /update?api_key=%s&field1=%d HTTP/1.1\r\n"
"Host: api.thingspeak.com\r\n"
"Connection: close\r\n\r\n",
send[]      = "\r\n";
float temperature ;

ISR(TIMER1_COMPA_vect)
{
	temperature =  celciusToFarenheit(readTemperatue(readADC()));
	ESP_sendData((int)temperature) ;
}

int main()
{
	USART_init();
	ADC_init();
	Timer_init();
	ESP_init();
	
	sei();
	while (1)
	;
	return 0;
}

void ESP_init()
{
	sendString(wifiMode);      // set wifimode = station + softAP
	sendString(send);
	if(!receiveString(buffer, ok))
	{
		sendString("ERROR");
		return;
	}
	strcpy(buffer,connectWiFi);    // connect to the router
	strcat(buffer,ssid);           // ssid
	strcat(buffer,",");
	strcat(buffer,pwd);            // password for the router
	strcat(buffer,send);
	sendString(buffer);            // connect
	if(!receiveString(buffer, ok))
	{
		sendString("ERROR");
		return;
	}
}

void ESP_sendData(int temp)
{
	strcpy(buffer,openTCP);        // open TCP connection
	strcat(buffer,"\"TCP\"");      // type TCP
	strcat(buffer,",");
	strcat(buffer,"\"api.thingspeak.com\"");   // host
	strcat(buffer,",");
	strcat(buffer,"80");           // port
	strcat(buffer,send);
	sendString(buffer);            // send it to ESP
	memset(buffer,0,128);
	if(!receiveString(buffer, ok))
	{
		sendString("ERROR");
		return;
	}
	
	sprintf(buffer,link,apiKey,temp);
	char tmp[4];
	itoa(strlen(buffer), tmp, 10);
	strcpy(buffer,dataSend);           // data send cmd
	strcat(buffer,tmp);                // number of data to be send
	strcat(buffer,send);
	sendString(buffer);                // send to ESP
		memset(buffer,0,128);
	if(!receiveString(buffer, ok))
	{
		sendString("ERROR");
		return;
	}
	
	sprintf(buffer,link,apiKey,temp);      // send HTTP text to the ESP
	strcat(buffer,send);
	sendString(buffer);
		memset(buffer,0,128);
	if(!receiveString(buffer, sendOK))
	{
		sendString("ERROR");
		return;
	}
}

void Timer_init()
{
	TCCR1B = 0x0C;         // CTC mode, Prescalar 256
	OCR1A  = 62499;        // generate 1 sec delay
	TIMSK1 = 0x02;         // enable OCIE1A Interrupt
}

void USART_init()
{
	UCSR0B = 0x18;         // TX enable
	UCSR0C = 0x06;         // 8 bit data
	UBRR0L = 8;           // Baud Rate 19200
}

void ADC_init()
{
	ADMUX  = 0x43;          // AVcc reference, channel 3
	ADCSRA = 0x87;          // Enable ADC, set Clk scalar to /128
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
	faren *= 1.8;          // celsius = ( 9/5 * tempertaure ) + 32
	faren += 32;
	return faren;
}

void USART_send(char data)
{
	while(!(UCSR0A & _BV(UDRE0))); // check if the Data register is empty
	UDR0 = data;           // send the data
}

void sendString(const char *str)
{
	while(*str)            // send all the data till null Character
	USART_send(*str++);
}

char USART_Receive()
{
	while(!(UCSR0A & _BV(RXC0)));  // wait for character in receiver
	return UDR0;
}

char receiveString(char* buffer,const char* limit)
{
	char *ptr = buffer;
	while(1)
	{
		*ptr++ = USART_Receive();  // store the received character in buffer
		if(strstr(buffer, limit))  // check for ok
		return 1;
		if(strstr(buffer, error)) // check for error
		return 0;
	}
}
