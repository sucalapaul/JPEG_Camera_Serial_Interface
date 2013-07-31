/* Arduino JPEGCamera Library
 * Copyright 2010 SparkFun Electronic
 * Written by Ryan Owens
*/


#include "jpeg_camera.h"
#include "rs232.h"

//Commands for the LinkSprite Serial JPEG Camera
const char GET_SIZE[5] = {0x56, 0x00, 0x34, 0x01, 0x00};
const char RESET_CAMERA[4] = {0x56, 0x00, 0x26, 0x00};
const char TAKE_PICTURE[5] = {0x56, 0x00, 0x36, 0x01, 0x00};
const char STOP_TAKING_PICS[5] = {0x56, 0x00, 0x36, 0x01, 0x03};
char READ_DATA1[8] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00};

const char SET_MAX_BAUD[7] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x0D, 0xA6};
const char SET_RESOLUTION_320_TEMP[5] = {0x56, 0x00, 0x54, 0x01, 0x11};
const char SET_RESOLUTION_320[9] = {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x11};

//We read data from the camera in chunks, this is the chunk size
const int read_size=4064;


//Initialize the serial connection
int JPEGCamera_begin(int port, int baud)
{
	//TODO: set baud using System();
	if(RS232_OpenComport(port, baud, 1))
	{
	  printf("Can not open comport %d\n", port);
	  return(1);
	}
	return 0;
}


//Get the size of the image currently stored in the camera
//pre: response is an empty string. size is a pointer to an integer
//post: response string contains the entire camera response to the GET_SIZE command
//		size is set to the size of the image
//return: number of characters in the response string
//usage: length = camera.getSize(response, &img_size);
int JPEGCamera_getSize(int port, char * response, unsigned int * size)
{
	int count=0;
	//Send the GET_SIZE command string to the camera
	count = JPEGCamera_sendCommand(port, GET_SIZE, response, 5);
	//Read 4 characters from the camera and add them to the response string


	int count2 = 0;

	for(int i=0; i<4; i++)
	{
		do
		{
			count2 = RS232_PollComport(port, response+count+i, 1);
		} while(count2 <= 0);
	}

	//Set the number of characters to return
	count+=4;

	//The size is in the last 2 characters of the response.
	//Parse them and convert the characters to an integer
    *size = (unsigned char)response[count-2]*256;
    *size += (unsigned char)response[count-1] & 0x00FF;
	//Send the number of characters in the response back to the calling function
	return count;
}

//Reset the camera
//pre: response is an empty string
//post: response contains the cameras response to the reset command
//returns: number of characters in the response
//Usage: camera.reset(response);
int JPEGCamera_reset(int port, char * response)
{
	RS232_SendBuf(port, RESET_CAMERA, 4);
	return 0;
}

int JPEGCamera_flush(int port, char * response)
{
	return RS232_PollComport(port, response, 100);
}

//Take a new picture
//pre: response is an empty string
//post: response contains the cameras response to the TAKE_PICTURE command
//returns: number of characters in the response
//Usage: camera.takePicture(response);
int JPEGCamera_takePicture(int port, char * response)
{
	return JPEGCamera_sendCommand(port, TAKE_PICTURE, response, 5);
}

int JPEGCamera_Set_Resolution_320(int port, char * response)
{
	RS232_SendBuf(port, SET_RESOLUTION_320, 9);
	return RS232_PollComport_exact(port, response, 5);
}

int JPEGCamera_SetMaxBaud(int port, char * response)
{
	RS232_SendBuf(port, SET_MAX_BAUD, 7);
	return 0;
}

int JPEGCamera_Set_Resolution_320_temp(int port, char * response)
{
	return JPEGCamera_sendCommand(port, SET_RESOLUTION_320_TEMP, response, 5);
}

//Stop taking pictures
//pre: response is an empty string
//post: response contains the cameras response to the STOP_TAKING_PICS command
//returns: number of characters in the response
//Usage: camera.stopPictures(response);
int JPEGCamera_stopPictures(int port, char * response)
{
	return JPEGCamera_sendCommand(port, STOP_TAKING_PICS, response, 5);
}

//Get the read_size bytes picture data from the camera
//pre: response is an empty string, address is the address of data to grab
//post: response contains the cameras response to the readData command, but the response header is parsed out.
//returns: number of characters in the response
//Usage: camera.readData(response, cur_address);
//NOTE: This function must be called repeatedly until all of the data is read
//See Sample program on how to get the entire image.
int JPEGCamera_readData(int port, char * response, int address)
{
	int count;
	int sss;
	int error;
	char degeaba[100];

	//Flush out any data currently in the serial buffer

	//If received frame was incomplete, retry
	do
	{
		error = 0;
		count=0;
		usleep(10000);
		sss = RS232_PollComport(port, degeaba, 100);
		usleep(10000);

		//Send the command to get read_size bytes of data from the current address
		for(int i=0; i<8; i++)
		{
			RS232_SendByte(port, READ_DATA1[i]);
		}

		RS232_SendByte(port, address>>8);
		RS232_SendByte(port, address);
		RS232_SendByte(port, 0x00);
		RS232_SendByte(port, 0x00);
		RS232_SendByte(port, read_size>>8);
		RS232_SendByte(port, read_size);
		RS232_SendByte(port, 0x00);
		RS232_SendByte(port, 0x0A);

		//Read the response header.
		sss = RS232_PollComport_exact(port, response, 5);
		if (sss != 5){
			error = 1;
		}

		//Now read the actual data and add it to the response string.
		count = RS232_PollComport_exact(port, response, read_size);
		if (count != read_size){
			error = 1;
		}

		//Read the response footer.
		sss = RS232_PollComport(port, degeaba, 10);
		if (sss != 5){
			//error = 1;
		}
	} while (error == 1);

	//Return the number of characters in the response.
	return count;
}

//Send a command to the camera
//pre: cameraPort is a serial connection to the camera set to 3800 baud
//     command is a string with the command to be sent to the camera
//     response is an empty string
//	   length is the number of characters in the command
//post: response contains the camera response to the command
//return: the number of characters in the response string
//usage: None (private function)
int JPEGCamera_sendCommand(int port, const char * command, char * response, int length)
{
	char c=0;
	int count=0;

	usleep(10000);
	RS232_FlushComport(port);
	usleep(10000);

	RS232_SendBuf(port, command, length);

	count = RS232_PollComport_exact(port, response, length);

	//return the number of characters in the response string
	return count;
}
