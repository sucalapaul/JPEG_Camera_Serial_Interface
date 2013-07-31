/*
 * cameras.c
 *
 *  Created on: Jul 29, 2013
 *      Author: dexter
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "cameras.h"
#include "jpeg_camera.h"
#include "rs232.h"

int camera_port_r;
int camera_port_l;

//Prints a char array, of a given size, even if there is a \0 in middle
void printCharArray(char *arr, int size) {
	for (int i = 0; i < size; i++) {
		printf("%c", (int) arr[i]);
	}
	printf("\n");

	for (int i = 0; i < size; i++) {
		printf("0x%x", arr[i]);
	}
	printf("\n");
}

//Take a picture, download it, and write it to a file
void *cameras_captureImage_r(void *arg) {
	thdata * data;
	data = (thdata *) arg;

	char fileNameTmp[] = "img_right.jpg";
	char fileName[] = "img_right.jpg";

	int camera_port = camera_port_r;
	char response[4096]; //Create a character array to store the cameras response to commands
	int count = 0; //Count is used to store the number of characters in the response string.
	unsigned int size = 0;	//Size will be set to the size of the jpeg image.
	int address = 0;//This will keep track of the data address being read from the camera
	int eof = 0;

	FILE *fp;
	char command[100];

	printf("%d -Taking picture...\n", camera_port);

	RS232_FlushComport(camera_port);

	//Take a picture
	count = JPEGCamera_takePicture(camera_port, response);
	usleep(10000);

	//Get the size of the picture
	count = JPEGCamera_getSize(camera_port, response, &size);
	//printCharArray(response, count);


	//Downloads the image
	printf("%d - Downloading. \t\tFile size: %d bytes\n", camera_port, size);
	fp = fopen(fileNameTmp, "wb");


	int percent = 0;
	while (address < size) {
		percent = 100 * address / size;
		printf("%d - %d%% \t %d/%d bytes\n", camera_port, percent, address, size);

		count = JPEGCamera_readData(camera_port, response, address);
		for (int i = 0; i < count; i++) {

			//Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
			if ((response[i] == (char) 0xD9)
					&& (response[i - 1] == (char) 0xFF))
				eof = 1;
			fwrite(response + i, sizeof(char), 1, fp);

			//If we found the eof character, get out of this loop and stop reading data
			if (eof == 1)
				break;
		}

		//Increment the current address by the number of bytes we read
		address += count;
		//Make sure we stop reading data if the eof flag is set.
		if (eof == 1)
			break;
	}

	fclose(fp);
	JPEGCamera_stopPictures(camera_port, response);
	printf("%d - Image acquired. \n", camera_port);

	//Duplicate the image into another file
//	sprintf(command,
//			"echo \"copying...\" \n\
//					rm %s\n\
//					cp %s %s \n\
//					echo \"Image saved\" ",
//			fileName, fileNameTmp, fileName);
//	//return 1;
//	system(command);

}

void *cameras_captureImage_l(void *arg) {
	thdata * data;
	data = (thdata *) arg;

	//captureImage(camera_port_l, "img_left_tmp.jpg", "img_left.jpg");
	char fileNameTmp[] = "img_left.jpg";
	char fileName[] = "img_left.jpg";

	int camera_port = camera_port_l;
	char response[4096]; //Create a character array to store the cameras response to commands
	int count = 0; //Count is used to store the number of characters in the response string.
	unsigned int size = 0;	//Size will be set to the size of the jpeg image.
	int address = 0;//This will keep track of the data address being read from the camera
	int eof = 0;

	FILE *fp;
	char command[100];

	printf("%d -Taking picture...\n", camera_port);

	RS232_FlushComport(camera_port);

	//Take a picture
	count = JPEGCamera_takePicture(camera_port, response);
	usleep(10000);

	//Get the size of the picture
	count = JPEGCamera_getSize(camera_port, response, &size);
	//printCharArray(response, count);


	//Downloads the image
	printf("%d - Downloading. \t\tFile size: %d bytes\n", camera_port, size);
	fp = fopen(fileNameTmp, "wb");


	int percent = 0;
	while (address < size) {
		percent = 100 * address / size;
		printf("%d - %d%% \t %d/%d bytes\n", camera_port, percent, address, size);

		count = JPEGCamera_readData(camera_port, response, address);
		for (int i = 0; i < count; i++) {

			//Check the response for the eof indicator (0xFF, 0xD9). If we find it, set the eof flag
			if ((response[i] == (char) 0xD9)
					&& (response[i - 1] == (char) 0xFF))
				eof = 1;
			fwrite(response + i, sizeof(char), 1, fp);

			//If we found the eof character, get out of this loop and stop reading data
			if (eof == 1)
				break;
		}

		//Increment the current address by the number of bytes we read
		address += count;
		//Make sure we stop reading data if the eof flag is set.
		if (eof == 1)
			break;
	}

	fclose(fp);
	JPEGCamera_stopPictures(camera_port, response);
	printf("%d - Image acquired. \n", camera_port);

	//Duplicate the image into another file
//	sprintf(command,
//			"echo \"copying...\" \n\
//					rm %s\n\
//					cp %s %s \n\
//					echo \"Image saved\" ",
//			fileName, fileNameTmp, fileName);
//	//return 1;
//	system(command);

}

int cameras_init() {

	camera_port_r = 16;		//ttyUSB0
	camera_port_l = 17;		//ttyUSB1
	char response[20];
	int count;

//	char *env;
//	env = getenv ("SERIAL_CAMERA_RIGHT");
//	if (env != NULL){
//		camera_port_r = atoi(env);
//	}
//	else {
//		printf("SERIAL_CAMERA_RIGHT environment not set. Using default value: %d\n", camera_port_r);
//	}
//
//	env = getenv ("SERIAL_CAMERA_LEFT");
//	if (env != NULL){
//		camera_port_l = atoi(env);
//	}
//	else {
//		printf("SERIAL_CAMERA_LEFT environment not set. Using default value: %d\n", camera_port_l);
//	}

	//Open ports at 115200 and try to reset cameras
	printf("\nInitialize ports, 115200\n");
	if (JPEGCamera_begin(camera_port_r, 115200)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 115200)) {
		return 1;
	}

	//Reset cameras
	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);
	RS232_CloseComport(camera_port_r);
	RS232_CloseComport(camera_port_l);

	//Open ports at 38400 and try to reset cameras
	printf("\nInitialize ports, 38400\n");
	if (JPEGCamera_begin(camera_port_r, 38400)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 38400)) {
		return 1;
	}

	//Reset cameras
	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);

	///// NOW CAMERAS ARE AT 38400 baud

	//Change camera's resolution to 320*240
	count = JPEGCamera_Set_Resolution_320(camera_port_r, response);
	if (count == 5) {
		printf("Right - Resolution changed successfully\n");
	} else {
		printf("Right - Unable to change resolution\n");
	}
	count = JPEGCamera_Set_Resolution_320(camera_port_l, response);
	if (count == 5) {
		printf("Left - Resolution changed successfully\n");
	} else {
		printf("Left - Unable to change resolution\n");
	}

	//Reset to apply resolution setting
	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);

	//Clear input (Camera's start message)
	RS232_FlushComport(camera_port_r);
	RS232_FlushComport(camera_port_l);

	//Change baud to 115200
	JPEGCamera_SetMaxBaud(camera_port_r, response);
	JPEGCamera_SetMaxBaud(camera_port_l, response);
	usleep(500000);

	RS232_FlushComport(camera_port_r);
	RS232_FlushComport(camera_port_l);
	RS232_CloseComport(camera_port_r);
	RS232_CloseComport(camera_port_l);

	///// NOW CAMERAS ARE AT 115200 baud, 320*240 resolution

	printf("\nInitialize ports, 115200\n");
	if (JPEGCamera_begin(camera_port_r, 115200)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 115200)) {
		return 1;
	}

	printf("\nCameras initialized!\n");

	return 0;
}
