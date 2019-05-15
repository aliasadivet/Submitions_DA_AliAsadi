/*
 * main_tx.c
 *
 * Created: 5/10/2019 2:36:22 PM
 * Author : Ali Asadi
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "nRF24L01.h"

#define CSN_SELECT      PORTB &= ~_BV(PB0)
#define CSN_DESELECT    PORTB |= _BV(PB0)
#define CE_SELECT       PORTB |= _BV(PB1)
#define CE_DESELECT     PORTB &= ~_BV(PB1)
#define RX 1
#define TX 0

void SPI_init();
void USART_init();
void USART_send(char );
void sendString(const char *);
void nRF_init(uint8_t);
void nRF_regWrite(uint8_t, uint8_t);
uint8_t nRF_regRead(uint8_t);
void PORT_init();
void nRF_sendData(char *, uint8_t);
uint16_t ReadADC(uint8_t);
void ADC_init();

char buffer[64];
int main()
{
  PORT_init();
  USART_init();
  ADC_init();
  SPI_init();     
  nRF_init(TX);   // nRF as transmitter

  while (1)
  {
    uint8_t status;   
    uint16_t value = ReadADC(8);                        // read internal temperature
    nRF_sendData(W_TX_PAYLOAD, (uint8_t *)&value, 2);   // send the value to sent to the receiver 
    _delay_ms(100); 
    status =  nRF_regRead(STATUS);                      // read the status
    if (status & RX_DR)                                 // check if data received  
    {
      CSN_SELECT;
      SPI_Transmit(R_RX_PL_WID);                        // read number of data received in the FIFO  
      uint8_t length = SPI_Transmit(NOP);
      CSN_DESELECT;
      nRF_readData(buffer, length);                     // read all the data from FIFO  
      sendString("Data received : ");
      sendString(itoa(buffer, *(uint16_t *)buffer, 10));// display the value
      sendString("\n\r");   
    }
    status =  nRF_regRead(STATUS);                      // read the status
    if(status & TX_DS)                                  // check if data in TX FIFO is sent
      nRF_regWrite(STATUS, TX_DS);                      // clear the flag
          
    _delay_ms(500);
  }
  return 0;
}

void PORT_init()
{
  DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB5);
  PORTB = (~(_BV(PB1) | _BV(PB3) | _BV(PB5))) & (_BV(PB4) | _BV(PB0));
}

void USART_init()
{
  UCSR0B = 0x18;         // TX enable
  UCSR0C = 0x06;         // 8 bit data
  UBRR0L = 8;           // Baud Rate 19200
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

void ADC_init()
{
  ADMUX = _BV(REFS0) | _BV(REFS1);                // Internal 1.1V Voltage Reference
  ADCSRA = _BV(ADEN) | _BV(ADPS0);                // Enable ADC, Clk / 2
}

uint16_t ReadADC(uint8_t Channel)
{
  ADMUX = (ADMUX & 0xF0) | (Channel & 0x0F);      // Select channel
  ADCSRA |= _BV(ADSC);                            // start conversion
  while (!(ADCSRA & _BV(ADIF)));                  // wait for completion
  ADCSRA |= _BV(ADIF);                            // clear flag
  return ADC;                                     // return the converted value
}


uint16_t ReadADC(uint8_t Channel)
{
  ADMUX = (ADMUX & 0xF0) | (Channel & 0x0F);      // Select channel
  ADCSRA |= _BV(ADSC);                            // start conversion
  while (!(ADCSRA & _BV(ADIF)));                  // wait for completion
  ADCSRA |= _BV(ADIF);                            // clear flag
  return ADC;                                     // return the converted value
}

void SPI_init()
{
  SPCR = 0x50;                                    // SPI enable, mode 0, MSB first
}

uint8_t SPI_Transmit(uint8_t data)
{
  SPDR = data;                                    // send data
  while (!(SPSR & _BV(SPIF)));                    // wait for completion
  return SPDR;                                    // return received data
}

void nRF_regWrite(uint8_t Address, uint8_t data)
{
  CSN_SELECT;
  SPI_Transmit((Address & 0x1F) | W_REGISTER);     // send address with Write mode
  SPI_Transmit(data);                              // data to be written
  CSN_DESELECT;                                    // CSN high
}

uint8_t nRF_regRead(uint8_t Address)
{
  uint8_t data;
  CSN_SELECT;                                       // CSN low
  SPI_Transmit(Address & 0x1F);                     // address to read from
  data = SPI_Transmit(NOP);                         // read the value
  CSN_DESELECT;                                     // CSN high
  return data;                                      // return the value
}

void nRF_init(uint8_t mode)
{
  nRF_regWrite(CONFIG, PWR_UP | mode);              // PWR_UP bit = 1, mode (PRX, PTX)
  if (mode == RX)
    CE_SELECT;
  else
    CE_DESELECT;
  nRF_regWrite(DYNPD, DPL_P0);                       // enable dyanamic payload for data pipe 0
  nRF_regWrite(FEATURE, EN_ACK_PAY | EN_DPL);        // enable Acknowledgement payload and dynamic payload
}

void nRF_sendData(uint8_t mode, char *buffer, uint8_t length)
{
  CSN_SELECT;
  SPI_Transmit(mode);                               // command to sent data to fifo
  while (length--)  
    SPI_Transmit(*buffer++);                        // send all data to fifo
  CSN_DESELECT;
  CE_SELECT;
  _delay_ms(10);                                    // enable CE for 10mS
  CE_DESELECT;
}

uint8_t nRF_readData(char *buffer, uint8_t length)
{
  if (nRF_regRead (STATUS) & RX_DR)                 // check if data is received
  {
    CSN_SELECT;
    SPI_Transmit(R_RX_PAYLOAD);                     // command to read data from fifo  
    while (length--)
      *buffer++ = SPI_Transmit(NOP);                // read all data
    CSN_DESELECT;
    nRF_regWrite(STATUS, RX_DR);                    // clear the flag    
    return 1;
  }
  return 0;
}
