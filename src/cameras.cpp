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

void printCharArray(char *arr, int size) {
	for (int i = 0; i < size; i++) {
		printf("%c\0", (int) arr[i]);
	}
	printf("\n");

	for (int i = 0; i < size; i++) {
		printf("0x%x \0", arr[i]);
	}
}

void *cameras_captureImage_r(void *arg) {
	thdata * data;
	data = (thdata *) arg;

	char fileNameTmp[] =  "img_right_tmp.jpg";
	char fileName[] =  "img_right.jpg";

	int camera_port = camera_port_r;
	char response[4096]; //Create a character array to store the cameras response to commands
		int count = 0; //Count is used to store the number of characters in the response string.
		unsigned int size = 0;	//Size will be set to the size of the jpeg image.
		int address = 0;//This will keep track of the data address being read from the camera
		int eof = 0;//eof is a flag for the sketch to determine when the end of a file is detected

		FILE *fp;
		char command[100];

		printf("\nStart %d\n", camera_port);

		printf("\Continue %d\n", camera_port);

		RS232_FlushComport(camera_port);

		//sleep(1);
		printCharArray(response, count);

		RS232_FlushComport(camera_port);

		//while (1) {
		//		*data->picture_ready = 0;
		//		while (*data->take_picture == 0);
		//		*data->take_picture = 0;

		//Take a picture
		count = JPEGCamera_takePicture(camera_port, response);
		usleep(10000);

		printCharArray(response, count);

		//Get the size of the picture
		count = JPEGCamera_getSize(camera_port, response, &size);
		printCharArray(response, count);
		if (count != 9) {
			//err = 1;
		}

		printf("%d Size: %d", camera_port, size);

		fp = fopen(fileNameTmp, "wb");

		////////////////////////////////////////////////////////////////

		int percent = 0;
		while (address < size) {
			percent = 100 * address / size;
			printf("%d   %d -  %d / %d\n", camera_port, percent, address, size);
			count = JPEGCamera_readData(camera_port, response, address);
			//Store all of the data that we read to the SD card
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
		printf("Image acquired.");
		sprintf(command,
				"echo \"copying...\" \n\
					rm %s\n\
					cp %s %s \n\
					echo \"Image saved\" ",
				fileName, fileNameTmp, fileName);
		//return 1;
		system(command);
}

void *cameras_captureImage_l(void *arg) {
	thdata * data;
	data = (thdata *) arg;

	//captureImage(camera_port_l, "img_left_tmp.jpg", "img_left.jpg");
	char fileNameTmp[] =  "img_left_tmp.jpg";
	char fileName[] =  "img_left.jpg";

	int camera_port = camera_port_l;
	char response[4096]; //Create a character array to store the cameras response to commands
		int count = 0; //Count is used to store the number of characters in the response string.
		unsigned int size = 0;	//Size will be set to the size of the jpeg image.
		int address = 0;//This will keep track of the data address being read from the camera
		int eof = 0;//eof is a flag for the sketch to determine when the end of a file is detected

		FILE *fp;
		char command[100];

		printf("\nStart %d\n", camera_port);

		printf("\Continue %d\n", camera_port);

		RS232_FlushComport(camera_port);

		//Reset the camera
		//count = JPEGCamera_reset(camera_port, response);
		//usleep(1000000);
		//sleep(1);
		printCharArray(response, count);

		RS232_FlushComport(camera_port);

		//while (1) {
		//		*data->picture_ready = 0;
		//		while (*data->take_picture == 0);
		//		*data->take_picture = 0;

		//Take a picture
		count = JPEGCamera_takePicture(camera_port, response);
		usleep(10000);

		printCharArray(response, count);

		//Get the size of the picture
		count = JPEGCamera_getSize(camera_port, response, &size);
		printCharArray(response, count);
		if (count != 9) {
			//err = 1;
		}

		printf("%d Size: %d", camera_port, size);

		fp = fopen(fileNameTmp, "wb");

		////////////////////////////////////////////////////////////////

		int percent = 0;
		while (address < size) {
			percent = 100 * address / size;
			printf("%d   %d -  %d / %d\n", camera_port, percent, address, size);
			count = JPEGCamera_readData(camera_port, response, address);
			//Store all of the data that we read to the SD card
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
		printf("Image acquired.");
		sprintf(command,
				"echo \"copying...\" \n\
					rm %s\n\
					cp %s %s \n\
					echo \"Image saved\" ",
				fileName, fileNameTmp, fileName);
		//return 1;
		system(command);
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


	printf("Initialize ports, 115200\n");
	if (JPEGCamera_begin(camera_port_r, 115200)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 115200)) {
		return 1;
	}

	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);

	RS232_CloseComport(camera_port_r);
	RS232_CloseComport(camera_port_l);
	printf("Initialize ports, 38400\n");
	if (JPEGCamera_begin(camera_port_r, 38400)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 38400)) {
		return 1;
	}

	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);


	count = JPEGCamera_Set_Resolution_320(camera_port_r, response);
	count = JPEGCamera_Set_Resolution_320(camera_port_l, response);

	count = JPEGCamera_reset(camera_port_r, response);
	count = JPEGCamera_reset(camera_port_l, response);
	usleep(500000);

	RS232_FlushComport(camera_port_r);
	RS232_FlushComport(camera_port_l);

	JPEGCamera_SetMaxBaud(camera_port_r, response);
	JPEGCamera_SetMaxBaud(camera_port_l, response);
	usleep(500000);

	RS232_FlushComport(camera_port_r);
	RS232_FlushComport(camera_port_l);

	RS232_CloseComport(camera_port_r);
	RS232_CloseComport(camera_port_l);
	printf("Initialize ports, 115200\n");
	if (JPEGCamera_begin(camera_port_r, 115200)) {
		return 1;
	}
	if (JPEGCamera_begin(camera_port_l, 115200)) {
		return 1;
	}

	return 0;
}
