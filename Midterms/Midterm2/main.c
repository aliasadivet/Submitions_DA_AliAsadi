/*
 * MT2.c
 *
 * Created: 5/13/2019 11:16:10 AM
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
#include "i2c.h"
#include "APDS9960_def.h"

#define APDS9960_WRITE (APDS9960_I2C_ADDR<<1)
#define APDS9960_READ ((APDS9960_I2C_ADDR<<1)|0x01)

void USART_init();
void USART_send(char );
void sendString(const char *);
void init_APDS9960();
uint16_t getreading(uint8_t);
void ESP_init();
void ESP_sendData(int);
char receiveString(char* ,const char* );

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
link[]      = "GET /update?api_key=%s&field2=%d HTTP/1.1\r\n"
"Host: api.thingspeak.com\r\n"
"Connection: close\r\n\r\n",
send[]      = "\r\n";

int main()
{
	uint8_t status;
	uint16_t value;

	USART_init();
	i2c_init();    // initialize I2C module
     ESP_init();    // initialize ESP module
	init_APDS9960();    // initialize APDS9960 module 
	while (1)
	{
		i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode 
		i2c_write(APDS9960_STATUS);   // set pointer
		i2c_start(APDS9960_READ);     // I2C start Slave Address + Read mode  
		status = i2c_read_nack();     // read status byte
		i2c_stop();

		if (status & 0x01)            // check if Avalid Bit is Set
		{
			value = getreading(APDS9960_CDATAL);    // read the LUX value 
               ESP_sendData(value) ;    // sent the LUX value
		}

		else if ( status & 0x80)      // check if saturation bit is set
		{
			i2c_start(APDS9960_READ);
			i2c_write(APDS9960_CICLEAR); // clear all flags
			i2c_stop();

			i2c_start(APDS9960_READ);
			i2c_write(APDS9960_AICLEAR);  // clear all flags
			i2c_stop();
		}
		_delay_ms(1000);
	}
	return 0;
}

void USART_init()
{
	UCSR0B = 0x18;         // TX enable
	UCSR0C = 0x06;         // 8 bit data
	UBRR0L = 8;           // Baud Rate 115200
}

void init_APDS9960(void) {
	_delay_ms(150);		/* Power up time >100ms */
	i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode 
	i2c_write(APDS9960_ENABLE);   // set Address
	i2c_write(APDS9960_PON | APDS9960_AEN | APDS9960_WEN); // enable PON, AEN, WEN
	i2c_stop();                   // I2C Stop

	i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode 
	i2c_write(APDS9960_ATIME);    // set Address
	i2c_write(DEFAULT_ATIME);     // set adc integration time
	i2c_stop();                   // I2C Stop

	i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode
	i2c_write(APDS9960_WTIME);    // set Address
	i2c_write(DEFAULT_WTIME);     // set wait time
	i2c_stop();                   // I2C Stop

	i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode
	i2c_write(APDS9960_CONFIG1);  // set Address
	i2c_write(DEFAULT_CONFIG1);   // set Config
	i2c_stop();                   // I2C Stop

}

uint16_t getreading(uint8_t Register) {

	uint16_t reading;
	i2c_start(APDS9960_WRITE);    // I2C start Slave Address + Write mode
	i2c_write(Register);          // set pointer

	i2c_start(APDS9960_READ);     // I2C start Slave Address + Read mode  
	reading = ((int)i2c_read_ack() | ((int)i2c_read_nack() << 8));   // read both low byte and High byte of 16 bit value
	i2c_stop();                   // I2C Stop
	return  reading;              // return the reading
}

void USART_send(char data)
{
	while (!(UCSR0A & _BV(UDRE0))); // check if the Data register is empty
	UDR0 = data;           // send the data
}

void sendString(const char *str)
{
	while (*str)           // send all the data till null Character
	USART_send(*str++);
}

char USART_Receive()
{
	while (!(UCSR0A & _BV(RXC0))); // wait for character in receiver
	return UDR0;
}
void ESP_init()
{
	sendString(wifiMode);      // set wifimode = station + softAP
	sendString(send);
	if(!receiveString(buffer, ok))
	{

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
	
		return;
	}
	
	sprintf(buffer,link,apiKey,temp);      // send HTTP text to the ESP
	strcat(buffer,send);
	sendString(buffer);
		memset(buffer,0,128);
	if(!receiveString(buffer, sendOK))
	{

		return;
	}
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