/*
 * cameras.h
 *
 *  Created on: Jul 29, 2013
 *      Author: dexter
 */



	#ifndef CAMERAS_H_
	#define CAMERAS_H_

	typedef struct str_thdata
	{
		int thread_no;
		//char message[100];
		int *take_picture;
		int *picture_ready;
	} thdata;


		void *cameras_captureImage_r(void *arg);
		void *cameras_captureImage_l(void *arg);
		int cameras_init();

	#endif /* CAMERAS_H_ */

