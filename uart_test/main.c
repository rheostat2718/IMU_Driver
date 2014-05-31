//  UH CubeSat Relative Attitude Determination
/*

	The attitude determination program proceeds in the following manner:

	1)	Setup the Tiva System Clock and Tiva Peripheral Devices
	2)  Configure the Inertial Measurement Unit and initialize variables
	3)  Configure a Timer-based Interrupt that actually does the frame rotation calculations
	4)  Spin forever in a infinite loop

	The timer interrupt service routine is Timer0IntHandler()

	This project makes use of Texas Instruments Tivaware 2.0 Library, see
	http://www.ti.com/tool/sw-tm4c

 */
//





#define PART_TM4C123GH6PM 1

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "attitude.h"
#include "IMU.h"
#include "USB.h"
#include "utils/uartstdio.h"
#include "ritual.h"


#define XGYRO_OUT 0x0400
#define YGYRO_OUT 0x0600
#define ZGYRO_OUT 0x0800
#define XACCL_OUT 0x0A00
#define YACCL_OUT 0x0C00
#define ZACCL_OUT 0x0E00


int arrayk;
char string[10];


#define PI 3.141592653
float Power(float x, int n);
void ftoa(char *cptr,float x);

unsigned int led_state;

int main(void)
{
	uint32_t Period;
	uint32_t data;
	uint32_t k1;

	//Set system clock
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	UARTinit();

	led_state = 0;

	// Timer and GPIO Initialization
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);

	// the Period is the number of clock ticks that should occur between successive calls to the ISR
	Period = ( (SysCtlClockGet()*1) /1); // this calls ISR every 1 [s]
	TimerLoadSet(TIMER0_BASE,TIMER_A,Period-1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE,TIMER_A);
	// End Timer and GPIO Initialization



	while(1)
	{
		// Spin forever, the ISR will be called to handle the frame rotation calculations
	}
}


void Timer0IntHandler( void )
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	switch(led_state)
	{
	case 0:
		GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3  ,  0);
		led_state = 1;
		break;
	case 1:
		GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3  ,  GPIO_PIN_1);
		led_state = 2;
		break;
	case 2:
		GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3  ,  GPIO_PIN_2);
		led_state = 3;
		break;
	case 3:
		GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3  ,  GPIO_PIN_3);
		led_state = 0;
		break;

	}


	BT_UART_StrPut("Timer0IntHandler() called");
	BT_UART_NLPut();


}


float Power(float x, int n)
{
	int k1;
	float temp;
	temp = 1;

	for( k1 = 1; k1<=n; ++k1)
	{
		temp *= x;
	}
	return temp;
}


