/*
 * stereo.h
 *
 *  Created on: Jul 29, 2013
 *      Author: root
 */

#ifndef STEREO_H_
#define STEREO_H_

	#include <cv.h>

	void trackObject(cv::Mat frame_init, cv::Mat frame, FILE *pf);
	void trackCircles(cv::Mat frame, cv::Mat &frame_orig, FILE *pf);
	void *stereo_process(void *arg);

#endif /* STEREO_H_ */
