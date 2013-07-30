/* Arduino JPeg Camera Library
 * Copyright 2010 SparkFun Electronics
 * Written by Ryan Owens
*/

#ifdef __cplusplus
extern "C" {
#endif

	#ifndef jpeg_camera_h
	#define jpeg_camera_h


	int JPEGCamera_begin(int port, int baud);
	int JPEGCamera_reset(int port, char * response);
	int JPEGCamera_getSize(int port, char * response, unsigned int * size);
	int JPEGCamera_takePicture(int port, char * response);
	int JPEGCamera_stopPictures(int port, char * response);
	int JPEGCamera_readData(int port, char * response, int address);
	int JPEGCamera_Set_Resolution_320(int port, char * response);
	int JPEGCamera_Set_Resolution_320_temp(int port, char * response);
	int JPEGCamera_SetMaxBaud(int port, char * response);

	int JPEGCamera_sendCommand(int port, const char * command, char * response, int length);


	#endif

#ifdef __cplusplus
	}
#endif
