/*
 * robot.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: root
 */

#include "robot.h"
#include "rs232.h"

//PROTOCOL DESCRIPTION

//    #? + comanda + string + ?#
//		comanda:
//			P - X, Y
//			A - X, Y, Theta
//			T - Theta
//			V - V, C
//			S
//		string:
//			Union encodat


//raspuns: #? + status + pozitie ?#
//		status:
//			T/F (a ajuns sau nu)
//
//		pozitie:
//			Union encodat cu X,Y, Theta final

const char MESSAGE_HEADER[2] = {'#', '?'};
const char MESSAGE_TAIL[2] = {'?','#'};
int port;


int Robot_Init()
{
	return Robot_Init(18);
}

//Initialize the serial connection
int Robot_Init(int port_number)
{
	port = port_number;
	if(RS232_OpenComport(port, 115200, 1))
	{
	  printf("Can not open comport %d\n", port);
	  return(1);
	}
	return 0;
}

int Robot_SendCommand(char* encoded_data, int size, char command)
{
	char buf[50];
	unsigned int i = 0;
	memcpy(buf + i, MESSAGE_HEADER, sizeof(MESSAGE_HEADER));
	i += sizeof(MESSAGE_HEADER);

	memcpy(buf + i, &command, 1);
	i ++;

	memcpy(buf + i, encoded_data, size);
	i += size;

	memcpy(buf + i, MESSAGE_TAIL, sizeof(MESSAGE_TAIL));
	i += sizeof(MESSAGE_TAIL);

	RS232_FlushComport(port);
	RS232_SendBuf(port, buf, i);

//	char buf2[4000];
//	int count = RS232_PollComport(port, buf2, 4000);
//
//	perror(buf2);

	return 0;
}

int Robot_SendDestination(float x, float y, float theta)
{
	destination_t destination;
	destination.data.x = x;
	destination.data.y = y;
	destination.data.t = theta;

	Robot_SendCommand(destination.encoded_data, sizeof (destination.encoded_data), 'A');

	return 0;
}

int Robot_SendTarget(float x, float y)
{
	target_t target;
	target.data.x = x;
	target.data.y = y;

	Robot_SendCommand(target.encoded_data, sizeof (target.encoded_data), 'P');

	return 0;
}

int Robot_SendDeviation(float x, float y, float theta)
{

	deviation_t deviation;
	deviation.teta = theta;

	//TODO: send command;
	return 0;
}

//se poate face cu TimeOut
int Robot_WaitResponse()
{
	//
	char buf[100];
	//buf = ce o trimis ei
	return RS232_PollComport_exact(port, buf, 3);
}



