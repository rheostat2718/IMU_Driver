

#define PART_TM4C123GH6PM 1

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define DC_MASK 0x3FFF
#define SIGN_MASK 0x2000

#define XGYRO_OUT 0x0400
#define YGYRO_OUT 0x0600
#define ZGYRO_OUT 0x0800
#define XACCL_OUT 0x0A00
#define YACCL_OUT 0x0C00
#define ZACCL_OUT 0x0E00

	uint32_t data;
	unsigned int Data;
	signed int Convert;
	float TrueMeasure;

void  SSIInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);

	GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_4);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet() , SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 40000, 16);
	SSIEnable(SSI0_BASE);
}

void IMUConfig(void)
{

	//  Factory calibration restore
		SSIDataPut(SSI0_BASE,0xBE02);
		SSIDataGet(SSI0_BASE,&data);
		SysCtlDelay(SysCtlClockGet()/20);

	// Sets Dynamic Range
		SSIDataPut(SSI0_BASE,0xB904);
		SSIDataGet(SSI0_BASE,&data);
		SysCtlDelay(SysCtlClockGet()/20);

	// Set Linear-g Gyroscope Bias

		SSIDataPut(SSI0_BASE,0xB4C6);
		SSIDataGet(SSI0_BASE,&data);
		SysCtlDelay(SysCtlClockGet()/20);

	// Set Decimation Rate to 2^4 tau=0.0195313[s]
		SSIDataPut(SSI0_BASE,0xB704);
		SSIDataGet(SSI0_BASE,&data);
		SysCtlDelay(SysCtlClockGet()/20);

		// Sets Bartlett filter to 2^4=16 taps per stage (with 2 stages)
		SSIDataPut(SSI0_BASE,0xB804);
		SSIDataGet(SSI0_BASE,&data);
		SysCtlDelay(SysCtlClockGet()/20);

	// ABC
	//	SSIDataPut(SSI0_BASE,0xBE01);
	//	SSIDataGet(SSI0_BASE,&data);
	//	SysCtlDelay(SysCtlClockGet()/3);

}



void avgconfig(void)
{
//	SSIDataPut(SSI0_BASE,0xBE01);
//	SSIDataGet(SSI0_BASE,&Data);

//	SysCtlDelay( SysCtlClockGet()/3);

	SSIDataPut(SSI0_BASE,0xB804);
	SSIDataGet(SSI0_BASE,&Data);

	SysCtlDelay( SysCtlClockGet()/3);
}

signed int DataConversion(unsigned int Value)
{

	signed int ConvertValue;

	//Mask to zero out don't care bits.
	Value = Value & DC_MASK;

	//If the value returned is over the largest possible positive value, it is negative.
	if(Value >= 8192)
	{
		ConvertValue = (~Value + 1) & DC_MASK;
		return ConvertValue*-1;
	}

	//Otherwise, it is positive. Don't make any changes.
	else
	{
		return Value;
	}

	return 0;
}

signed int xgyroread(void)
{
	SSIDataPut(SSI0_BASE, XGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);
	SSIDataPut(SSI0_BASE, XGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);
	Convert = DataConversion(Data);

	//Convert to degrees per second.
	TrueMeasure = Convert * 0.05;
	return Convert;
}

signed int ygyroread(void)
{
	SSIDataPut(SSI0_BASE, YGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);
	SSIDataPut(SSI0_BASE, YGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);
	Convert = DataConversion(Data);

	//Convert to degrees per second.
	TrueMeasure = Convert * 0.05;
	return Convert;
}

signed int zgyroread(void)
{
	SSIDataPut(SSI0_BASE, ZGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);
	SSIDataPut(SSI0_BASE, ZGYRO_OUT);
	SSIDataGet(SSI0_BASE, &Data);

	Convert = DataConversion(Data);

	//Convert to degrees per second.
	TrueMeasure = Convert * 0.05;
	return Convert;
}


signed int xaccelread(void)
{
	SSIDataPut(SSI0_BASE, XACCL_OUT);
	SSIDataGet(SSI0_BASE,&Data);
	SSIDataPut(SSI0_BASE, 0x0E00);
	SSIDataGet(SSI0_BASE,&Data);
	return Data;
}


signed int yaccelread(void)
{
	SSIDataPut(SSI0_BASE,YACCL_OUT);
	SSIDataGet(SSI0_BASE,&Data);
	SSIDataPut(SSI0_BASE,YACCL_OUT);
	SSIDataGet(SSI0_BASE,&Data);
	return Data;
}

signed int zaccelread(void)
{
	SSIDataPut(SSI0_BASE,ZACCL_OUT);
	SSIDataGet(SSI0_BASE,&Data);
	SSIDataPut(SSI0_BASE,ZACCL_OUT);
	SSIDataGet(SSI0_BASE,&Data);
	return Data;
}



//Multiplies by 0.05 and prints the result to the screen (by doing long division).
void printfloat(signed int number)
{
	signed int leading;
	unsigned int tens, hundreds, signless, remainder, decimal;
	signless = abs(number);

	if(number < 0)
	{
		BT_UART_CharPut('-');
	}

	leading = signless / 20;
	BT_NumPut(leading);
	BT_UART_CharPut('.');

	remainder = signless % 20;
	remainder = (remainder * 100) / 20;
	if (remainder < 10)
	{
		BT_UART_CharPut('0');
	}
	BT_NumPut(remainder);

//	tens = (10*signless) / 20;
//	BT_NumPut(tens);
//
//
//	remainder = 10*((10*signless) - (tens*20));
//	hundreds = remainder / 20;
//	BT_NumPut(hundreds);

//	remainder = (signless % 20)*5;
//	if (remainder < 10)
//	{
//		BT_UART_CharPut('0');
//	}
//	myNumPut(remainder);



}

void printfloata(signed int number)
{
//	signed int leading;
//	unsigned int tens, hundreds, signless, remainder, decimal;
	if ( 0x2000 & number)
	{
		number = number | 0xFFFFC000;
	}
	else
	{
		number &= ~0xC000;
	}
	BT_NumPut(number);

}

void myDegPut( signed int x)
{
	float xf;
	char out[32];
	xf = ( (float) x) * 0.05;



}
