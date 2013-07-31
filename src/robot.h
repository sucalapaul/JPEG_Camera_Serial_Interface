/*
 * robot.h
 *
 *  Created on: Jul 31, 2013
 *      Author: root
 */

#ifndef ROBOT_H_
#define ROBOT_H_

	union { // union target
		char serial[8];
		struct {
			float x;
			float y;
		} mystruct;
	} target;

	int Robot_Init();
#endif /* ROBOT_H_ */
