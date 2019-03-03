/*
 * DA1B.c
 *
 * Created: 2.24.2019. 22:12:51
 * Author : Ali Asadi
 */ 

#include <avr/io.h>


int main()
{
	volatile char* b = 0x0700;
	*b = 11;
	
	volatile char* a = (char*)0x0200;
	volatile char* divisable = (char*)0x0400;
	volatile char* notDivisable = (char*)0x0600;
	volatile short int* sumA = (short int*)16;
	volatile short int* sumB = (short int*)18;
	int sumBb;

	
	*sumA = 0;
	*sumB = 0; 

	*a = 11;
	
	while(*a <= 109)
	{
		
		if(*a%3 == 0)
		{
		
			*sumA += *a;
			*divisable++ = *a;
		}
		else
		{
			sumBb += *a;
			*notDivisable++ = *a;
		}

		a++;
		(*a) = ++(*b);
	}
	*a = 0;

*sumB = sumBb;
	volatile int end;
}


