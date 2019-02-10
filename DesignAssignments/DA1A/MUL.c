
/*
 * MUL.c
 *
 * Created: 10.02.2019. 21:28:39
 * Author : Ali Asadi
 */ 

#include <avr/io.h>

int main(void)
{
	volatile	 unsigned int multiplier = 255;
	volatile	unsigned int multiplicand = 65535;
	volatile unsigned long result;
	result = (long int)multiplicand*multiplier;
	result = result;
}