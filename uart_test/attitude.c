/*
 * attitude.c


 */

#include <stdint.h>
#include <stdbool.h>
#include "IMU.h"
#include "USB.h"

extern float xarray[20];
extern float yarray[20];
extern float zarray[20];
extern float xbias,ybias,zbias;
extern float xx0,xy0,xz0,yx0,yy0,yz0,zx0,zy0,zz0;
extern int arrayk;
extern char out0;

//**********************************************************************************************
//
// AttitudeInit( void )
//
// This function initializes all the variables associated with the relative attitude determination
// algorithm.  It also takes samples from the gyroscopes (assuming the IMU is stationary) to
// provide an initial estimate of the gyroscope bias.
//
// Parameters: none
//
// Return: none
//
//**********************************************************************************************
void AttitudeInit( void)
{
	int i; // for loop counter variable used for intialization loops

	// Setup initial axes frames
	// the body frame unit vectors is represented as a set of 3 ordered numbers that represent
	// cartesian coordinates of the terminating point of said unit vector ( the originating point
	// is assumed to be the origin)
	//
	// first character of the variable name refers to the body-frame axes
	// second character is the x, y, or z coordinate of the terminating point of the vector
	//
	xx0=1.0;
	xy0 = xz0 = 0.0;
	yy0=1.0;
	yx0=yz0=0.0;
	zz0=1.0;
	zx0=zy0=0.0;

	out0=0;  // This variable is used to restrict output in the ISR to one in every n calls of ISR
	arrayk=0;  // sets the index of the gyro data buffer to zero


	// Set the xbias, ybias, zbias to zero
	xbias = 0.0;
	ybias = 0.0;
	zbias = 0.0;

	// take a few gyroscope samples and add them to the corresponding variable,
	// then divide by the number of samples to calculate the arithmetic mean (average)
	for(i=0;i<40;i++)
	{
		xbias += xgyroread()*0.05;
		ybias += ygyroread()*0.05;
		zbias += zgyroread()*0.05;
		//delay 1/20sec
		SysCtlDelay(SysCtlClockGet()/10/3);
	}

	xbias/=40.0;
	ybias/=40.0;
	zbias/=40.0;



	//Intialize Gyroscope data arrays to zero


	for(i=0;i<20;i++)
	{
		xarray[i]=0.0;
		yarray[i]=0.0;
		zarray[i]=0.0;
	}

}


//**********************************************************************************************
//
// float xavg( void )
//
// This function computes and returns the moving average of the n previous samples of the X-Gyroscope
//
// Parameters: none
//
// Return: floating point variable containing the result of the moving average of the X-Gyroscope
//         signal
//
//**********************************************************************************************
float xavg(void)
{
	int k;
	float temp;
	temp = 0.0;
	for(k=0;k<20;k++)
	{
		temp+=xarray[k];
	}
	return temp/20.0;
}

//**********************************************************************************************
//
// float yavg( void )
//
// This function computes and returns the moving average of the n previous samples of the Y-Gyroscope
//
// Parameters: none
//
// Return: floating point variable containing the result of the moving average of the Y-Gyroscope
//         signal
//
//**********************************************************************************************
float yavg(void)
{
	int k;
	float temp;
	temp = 0.0;
	for(k=0;k<20;k++)
	{
		temp+=yarray[k];
	}
	return temp/20.0;
}

//**********************************************************************************************
//
// float zavg( void )
//
// This function computes and returns the moving average of the n previous samples of the Z-Gyroscope
//
// Parameters: none
//
// Return: floating point variable containing the result of the moving average of the Z-Gyroscope
//         signal
//
//**********************************************************************************************
float zavg(void)
{
	int k;
	float temp;
	temp = 0.0;
	for(k=0;k<20;k++)
	{
		temp+=zarray[k];
	}
	return temp/20.0;
}

