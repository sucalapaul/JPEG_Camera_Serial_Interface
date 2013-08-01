/*
 * robot.h
 *
 *  Created on: Jul 31, 2013
 *      Author: root
 */

#ifndef ROBOT_H_
#define ROBOT_H_

	union destination_t { // union dest
		char encoded_data[12];
		struct {
			float x;
			float y;
			float t;
		} data;
	};

	union target_t { // union target
		char encoded_data[8];
		struct {
			float x;
			float y;
		} data;
	};

	union deviation_t{ // union deviatie
		char encoded_data[4];
		float teta;
	};

	int Robot_Init(int port_number);
	int Robot_Init();
	int Robot_SendDestination(float x, float y, float theta);
	int Robot_SendTarget(float x, float y);
	int Robot_SendDeviation(float x, float y, float theta);
	int Robot_WaitResponse();
#endif /* ROBOT_H_ */
