#define PART_TM4C123GH6PM 1

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "IMU.h"
#include "USB.h"

#define MY_UART UART1_BASE

char sout1[10];

//**********************************************************************************************
//
// void UARTinit (void )
//
// This function setups the UARTs on the Tiva
//
// Parameters: none
//
// Return: none
//
//**********************************************************************************************
void UARTinit(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
	UARTConfigSetExpClk(UART1_BASE,SysCtlClockGet(),9600,UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
}


//
//**********************************************************************************************
//
// void ftoa( char *cptr, float x )
//
// This function converts an IEEE 32-bit floating point variable to a character string.  This is
// is useful because the uartstdio printf-like functions don't support floating point values
// and the sprintf function is unreliable (this might have something to do with stack overflows)
//
// Parameters: char *cptr is a pointer to a c-style string that should be at least 10 characters in size
//             float x is a floating point variable (-10000,10000)
//
// Return: none
//
//**********************************************************************************************
void ftoa(char *cptr,float x)
{
	float temp;
	unsigned int i,k,j;
	int d;
	char flag;
	flag = 0;
	k=0;
	if ( x < 0)
	{
		cptr[k++]='-';
		x*=-1.0;
	}
	else
	{
		cptr[k++]='+';
	}


	for(j=3;j>0;j--)
	{
		temp=x;
		for(i=0;i<j;i++)
		{
			temp/=10.0;
		}
		d=( (int) temp) % 10;
		if ( (d > 0) || (flag == 1) )
		{
			flag = 1;
			cptr[k++]='0'+ d;
		}
	}

	cptr[k++]= '0' +  ( ( (int) (x) ) % 10 );

	cptr[k++]='.';

	for(j=0;j<4;++j)
	{
		x *= 10.0;
		cptr[k++]= '0' +  ( ( (int) (x) ) % 10 );
	}
	cptr[k++]='\0';
}



char sout[255];

// BT Functions
// These functions should either use UART0 or UART1
// UART0 is the emulated UART over the USB cable
// UART1 uses pins B0,B1


//**********************************************************************************************
//
// bool BT_UART_CharsAvail(void)
//
// This function is a wrapper function for UARTCharsAvail.  It returns true if there are characters in
// the receive FIFO and false if there aren't any.
//
// Parameters: none
//
// Return: none
//
//**********************************************************************************************
bool BT_UART_CharsAvail(void)
{
	return UARTCharsAvail(MY_UART);
}

//**********************************************************************************************
//
// void BT_UART_CharPut(char c)
//
// Puts character out to UART
//
// Parameters: char c ASCII character to be put on UART
//
// Return: none
//
//**********************************************************************************************
void BT_UART_CharPut(char c)
{
	UARTCharPut(MY_UART,c);


}

//**********************************************************************************************
//
// char BT_UART_CharGet(void)
//
// This function returns a character from UART receieve FIFO.  Note you should call BT_UART_CharsAvail
// to see if there are any characters in the receieve FIFO.
//
// Parameters: none
//
// Return: character from FIFO
//
//**********************************************************************************************
char BT_UART_CharGet(void)
{
	return ( (char) UARTCharGet(MY_UART));
}

//**********************************************************************************************
//
// char BT_UART_StrPut(char * cptr)
//
// This function is a wrapper function for UARTCharsAvail.  It returns true if there are characters in
// the receive FIFO and false if there aren't any.
//
// Parameters: char * cptr is a char pointer that points to a null-terminated c-style string
//
// Return: char with a value of 0 if the null terminator is found
//
//**********************************************************************************************
char BT_UART_StrPut(char * cptr)
{
	unsigned int n;

	for (n=0; n<strlen(cptr)+1;++n)
	{
		if ( (*cptr) == '\0')
		{

			return (char) 0;
		}
		BT_UART_CharPut( *(cptr+n) );
	}

	return (char) 0;

}

//**********************************************************************************************
//
// void BT_UART_NLPut(void)
//
// This function put a New Line (Carriage Return + Line Feed) on the UART
//
// Parameters: none
//
// Return: none
//
//**********************************************************************************************
void BT_UART_NLPut(void)
{
	BT_UART_CharPut(0x0D);
	BT_UART_CharPut(0x0A);
}

//**********************************************************************************************
//
// void BT_NL_Put(void)
//
// This function is just a wrapper for BT_UART_NLPut.  This is included as part of code merging
// operation.
//
// Parameters: none
//
// Return: none
//
//**********************************************************************************************
void BT_NL_Put(void)
{
	BT_UART_NLPut();
}

//**********************************************************************************************
//
// void BT_NumPut( unsigned int z)
//
// This function outputs a an unsigned integer over the UART
//
// Parameters:  unsigned int z is the unsigned integer to be output
//
// Return: none
//
//**********************************************************************************************
void BT_NumPut( unsigned int z)
{
	ltoa( (long) z,sout); // typecast an unsigned integer
	BT_UART_StrPut(sout);
}

