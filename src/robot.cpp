/*
 * robot.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: root
 */

#include "robot.h"
#include "rs232.h"

//Initialize the serial connection
int Robot_Init()
{
	int port = 18;
	if(RS232_OpenComport(port, 9600, 1))
	{
	  printf("Can not open comport %d\n", port);
	  return(1);
	}
	return 0;
}


