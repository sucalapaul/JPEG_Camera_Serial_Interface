/*
 * FinalStereo.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: dexter
 */

#include <cv.h>
#include <highgui.h>
#include <math.h>
#include "stereo.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "jpeg_camera.h"
#include "rs232.h"
#include "stereo.h"
#include "robot.h"
#include <sys/time.h>

//extern "C" {
#include "cameras.h"
//}

int is_beagle = 0;
pthread_t tid[3];

using namespace std;
using namespace cv;

int main() {

	int have_cameras = 1;
	int have_robot = 1;
	int err;
	int *ptr[3];
	struct timeval tv;
	int count = 0;
	thdata thread_data_r, thread_data_l;


	printf("Started\n");
	setbuf(stdout, NULL); //no delay for stdout

	if (cameras_init()) {
		//error in camera initialization
		printf("\nCameras not connected!\n");
		have_cameras = 0;
		//return 0;
	}

	//Robot serial port init
	if (Robot_Init()) {
		//error in STM32 serial initialization
		printf("\nSTM32 serial not connected!\n");
		have_robot = 0;
		//return 0;
	}

	printf("Init done\n\n");

	//Main loop
	while (1) {

		//printf("\n\nPress Enter to capture a new frame...\n");
		//getchar();

		//Compute runtime
		gettimeofday(&tv, NULL);
		long start = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;

		if (have_cameras) {

			//Create a thread for each camera capture
			pthread_create(&(tid[0]), NULL, &cameras_captureImage_r, (void *) &thread_data_r);
			pthread_create(&(tid[1]), NULL, &cameras_captureImage_l, (void *) &thread_data_l);

			pthread_join(tid[0], (void**) &(ptr[0]));
			pthread_join(tid[1], (void**) &(ptr[1]));

		} else {
			printf("Processing without cameras\n");
		}

		//Display run time
		gettimeofday(&tv, NULL);
		long now = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
		printf("Capture lasted %d ms\n", now - start);

		//Create a thread for image processing, wait for it to finish
		pthread_create(&(tid[2]), NULL, &stereo_process, NULL);
		pthread_join(tid[2], (void**) &(ptr[2]));

		//Run time for image processing
		gettimeofday(&tv, NULL);
		long now2 = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
		printf("Image processing lasted %d ms\n", now2 - now);

		//Call prolog program
		//system("./objects");

		//Run time for prolog
		gettimeofday(&tv, NULL);
		long now3 = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
		printf("Prolog lasted %d ms\n", now3 - now2);

		//Send the command to robot
		//TODO: Send the command to robot


		//Wait for response from robot
		//TODO: Wait for response from robot
	}

	return 0;
}
