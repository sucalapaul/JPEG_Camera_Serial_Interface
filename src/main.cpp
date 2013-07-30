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
#include <sys/time.h>

//extern "C" {
#include "cameras.h"
//}

int is_beagle = 0;
int take_picture_r = 0, take_picture_l = 0, picture_ready_r = 0,
		picture_ready_l = 0, process_ready = 0;
pthread_t tid[3];

using namespace std;
using namespace cv;

int main() {
	printf("Started\n"); /* prints !!!Hello World!!! */
	setbuf(stdout, NULL);			//no delay for stdout

	if (cameras_init()) {
		return 0;
	}

	take_picture_r = 1;
	take_picture_l = 1;
	picture_ready_r = 0;
	picture_ready_l = 0;
	process_ready = 0;

	thdata thread_data_r, thread_data_l;
	//thdata.
	//thdata_process thread_data_process;
//	thread_data_r.picture_ready = &picture_ready_r;
//	thread_data_r.take_picture = &take_picture_r;
//	thread_data_l.picture_ready = &picture_ready_l;
//	thread_data_l.take_picture = &take_picture_l;

	//thread_data_process.picture_ready

	int err;
	int *ptr[3];

	struct timeval tv;

	gettimeofday(&tv, NULL);
	long start = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;

	pthread_create(&(tid[0]), NULL, &cameras_captureImage_r, (void *) &thread_data_r);
	pthread_create(&(tid[1]), NULL, &cameras_captureImage_l, (void *) &thread_data_l);

//        if (err != 0)
//            printf("\ncan't create thread ");
//        else
//            printf("\n Thread created successfully\n");

	pthread_join(tid[0], (void**)&(ptr[0]));
	pthread_join(tid[1], (void**)&(ptr[1]));

	gettimeofday(&tv, NULL);
	long now = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	printf("capture lasted %d ms\n", now - start);

	err = pthread_create(&(tid[2]), NULL, &stereo_process, NULL);
	pthread_join(tid[2], (void**) &(ptr[2]));

	gettimeofday(&tv, NULL);
	long now2 = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	printf("processing lasted %d ms\n", now2 - now);

//	while (1) {
//		int c = cvWaitKey(10);
//		if ((char) c == 27)
//			break;
//	}

	return 0;

}
