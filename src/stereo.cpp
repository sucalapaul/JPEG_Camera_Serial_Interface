/*
 * FinalStereo.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: dexter
 */

#include <cv.h>
#include <highgui.h>
#include <opencv2/gpu/gpu.hpp>
#include <math.h>
#include <list>
#include <vector>

using namespace std;
using namespace cv;

bool pnpoly(int npol, CvPoint *pt[], float x, float y) {
	int i, j;
	bool c = FALSE;
	for (i = 0, j = npol - 1; i < npol; j = i++) {
		if ((((pt[i]->y <= y) && (y < pt[j]->y))
				|| ((pt[j]->y <= y) && (y < pt[i]->y)))
				&& (x
						< (pt[j]->x - pt[i]->x) * (y - pt[i]->y)
								/ (pt[j]->y - pt[i]->y) + pt[i]->x))
			c = !c;
	}
	return c;
}

void writeFile(FILE *pf, char* type, char* color, int x, int y) {

	fprintf(pf, "%s %s %d %d \n", type, color, x, y);

}
char * getColor(int red, int green, int blue) {

	if (red > 192 && green > 192 && blue > 192)
		return "white";
	else if (red > blue + 64 && red > green + 64)
		return "red";
	else if (green > blue + 64 && green > red + 64)
		return "green";
	else if (blue > red + 64 && blue > green + 64)
		return "blue";
	else if (green > red + 32 && blue > red + 32)
		return "cyan";
	else if (red > green + 64 && blue > green + 32)
		return "magenta";
	else if (red > blue + 32 && green > blue + 32)
		return "yellow";
	else
		return "black";

}

void determineMediumColorPolygons(int nrPoints, CvPoint *pt[], Mat frame,
		Mat &frame_orig, char * &color) {
	float mR = 0, mG = 0, mB = 0;
	int nr = 0;
	int xMin = pt[0]->x, xMax = pt[0]->x, yMin = pt[0]->y, yMax = pt[0]->y;

	for (int i = 1; i < nrPoints; i++) {
		if (pt[i]->x > xMax)
			xMax = pt[i]->x;
		if (pt[i]->x < xMin)
			xMin = pt[i]->x;
		if (pt[i]->y > yMax)
			yMax = pt[i]->y;
		if (pt[i]->y < yMin)
			yMin = pt[i]->y;
	}

	for (int i = xMin; i <= xMax; i++) {
		for (int j = yMin; j <= yMax; j++) {
			//determine color of each pixel from the circle
			if (pnpoly(nrPoints, pt, i, j)) {
				mR += frame.at<Vec3b>(j, i)[2];
				mG += frame.at<Vec3b>(j, i)[1];
				mB += frame.at<Vec3b>(j, i)[0];
				nr++;
			}
		}
	}

	mR = mR / nr;
	mG = mG / nr;
	mB = mB / nr;
	//printf("%f, %f, %f \n", mR, mG, mB);


	color = getColor(mR, mG, mB);

	for (int i = xMin; i <= xMax; i++) {
		for (int j = yMin; j <= yMax; j++) {
			//determine color of each pixel from the circle
			if (pnpoly(nrPoints, pt, i, j)) {
				frame_orig.at<Vec3b>(j, i)[2] = (int) mR;
				frame_orig.at<Vec3b>(j, i)[1] = (int) mG;
				frame_orig.at<Vec3b>(j, i)[0] = (int) mB;

				//nr++;
			}
		}
	}

}

void trackObject(Mat frame_init, Mat frame, vector<char*> &type,
		vector<CvPoint> &centerT) {
	CvSeq* contour; //hold the pointer to a contour
	CvSeq* result; //hold sequence of points of a contour
	CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

	Mat imgCanny;
	Canny(frame_init, imgCanny, 100, 200, 3);

	//finding all contours in the image

	IplImage* imgCannyIpl = new IplImage(imgCanny);
	cvFindContours(imgCannyIpl, storage, &contour, sizeof(CvContour),
			CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	while (contour) {
		char* color;

		//obtain a sequence of points of contour, pointed by the variable 'contour'
		result = cvApproxPoly(contour, sizeof(CvContour), storage,
				CV_POLY_APPROX_DP, cvContourPerimeter(contour) * 0.03, 0);

		//if there are 3  vertices  in the contour(It should be a triangle)
		if (result->total == 3
				&& fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 100) {
			//iterating through each point
			CvPoint *pt[3];
			for (int i = 0; i < 3; i++) {
				pt[i] = (CvPoint*) cvGetSeqElem(result, i);
			}

			determineMediumColorPolygons(3, pt, frame_init, frame, color);

			//drawing lines around the triangle
			line(frame, *pt[0], *pt[1], cvScalar(255, 0, 0), 4);
			line(frame, *pt[1], *pt[2], cvScalar(255, 0, 0), 4);
			line(frame, *pt[2], *pt[0], cvScalar(255, 0, 0), 4);

			//drawing gravity center
			CvPoint center;
			center.x = (int) (pt[0]->x + pt[1]->x + pt[2]->x) / 3;
			center.y = (int) (pt[0]->y + pt[1]->y + pt[2]->y) / 3;
			centerT.push_back(center);

			circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);

			//writeFile(pf, "triangle", color, center.x, center.y);
			type.push_back("triangle");

		}

		//if there are 4 vertices in the contour(It should be a quadrilateral)
		else if (result->total == 4 && cvCheckContourConvexity(result)
				&& fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 100) {
			//iterating through each point
			CvPoint *pt[4];
			for (int i = 0; i < 4; i++) {
				pt[i] = (CvPoint*) cvGetSeqElem(result, i);
			}

			determineMediumColorPolygons(4, pt, frame_init, frame, color);

			//drawing lines around the quadrilateral
			line(frame, *pt[0], *pt[1], cvScalar(0, 255, 0), 4);
			line(frame, *pt[1], *pt[2], cvScalar(0, 255, 0), 4);
			line(frame, *pt[2], *pt[3], cvScalar(0, 255, 0), 4);
			line(frame, *pt[3], *pt[0], cvScalar(0, 255, 0), 4);

			//drawing gravity center
			CvPoint center;
			center.x = (int) (pt[0]->x + pt[1]->x + pt[2]->x + pt[3]->x) / 4;
			center.y = (int) (pt[0]->y + pt[1]->y + pt[2]->y + pt[3]->y) / 4;
			centerT.push_back(center);

			circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);

			//writeFile(pf, "quadrilateral", color, center.x, center.y);
			type.push_back("quadrilateral");
		}

		//if there are 7  vertices  in the contour(It should be a heptagon)
//		else if(result->total ==7  && fabs(cvContourArea(result, CV_WHOLE_SEQ))>400)
//		{
//			//iterating through each point
//			CvPoint *pt[7];
//			for(int i=0;i<7;i++){
//				pt[i] = (CvPoint*)cvGetSeqElem(result, i);
//			}
//
//			//drawing lines around the heptagon
//			line(frame, *pt[0], *pt[1], cvScalar(0,0,255),4);
//			line(frame, *pt[1], *pt[2], cvScalar(0,0,255),4);
//			line(frame, *pt[2], *pt[3], cvScalar(0,0,255),4);
//			line(frame, *pt[3], *pt[4], cvScalar(0,0,255),4);
//			line(frame, *pt[4], *pt[5], cvScalar(0,0,255),4);
//			line(frame, *pt[5], *pt[6], cvScalar(0,0,255),4);
//			line(frame, *pt[6], *pt[0], cvScalar(0,0,255),4);
//
//			//drawing gravity center
//			CvPoint center;
//			center.x=(int) (pt[0]->x + pt[1]->x + pt[2]->x + pt[3]->x + pt[4]->x +pt[5]->x + pt[6]->x)/7;
//			center.y=(int) (pt[0]->y + pt[1]->y + pt[2]->y + pt[3]->y + pt[4]->y +pt[5]->y + pt[6]->y)/7;
//			circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
//		}

		//obtain the next contour
		contour = contour->h_next;
	}

	cvReleaseMemStorage(&storage);
}

void determineMediumColorCircle(Point center, int radius, Mat &frame_orig,
		char * &color, int &colorDeviation) {
	float mR = 0, mG = 0, mB = 0;
	int nr = 0;

	for (int i = center.x - radius < 0 ? 0 : center.x - radius;
			i
					<= (center.x + radius > frame_orig.cols ?
							frame_orig.cols : center.x + radius); i++) {
		for (int j = center.y - radius < 0 ? 0 : center.y - radius;
				j
						<= (center.y + radius > frame_orig.rows ?
								frame_orig.rows : center.y + radius); j++) {
			//determine color of each pixel from the circle
			if (pow(i - center.x, 2) + pow(j - center.y, 2) <= pow(radius, 2)) {
				mR += frame_orig.at<Vec3b>(j, i)[2];
				mG += frame_orig.at<Vec3b>(j, i)[1];
				mB += frame_orig.at<Vec3b>(j, i)[0];
				nr++;
			}
		}
	}

	mR = mR / nr;
	mG = mG / nr;
	mB = mB / nr;
	int mRerr =0,mGerr=0, mBerr=0;

	color = getColor(mR, mG, mB);

	for (int i = center.x - radius < 0 ? 0 : center.x - radius;
			i
					<= (center.x + radius > frame_orig.cols ?
							frame_orig.cols : center.x + radius); i++) {
		for (int j = center.y - radius < 0 ? 0 : center.y - radius;
				j
						<= (center.y + radius > frame_orig.rows ?
								frame_orig.rows : center.y + radius); j++) {
			//rewrite color of each pixel from the circle
			if (pow(i - center.x, 2) + pow(j - center.y, 2) <= pow(radius, 2)) {

				mRerr += abs(mR-frame_orig.at<Vec3b>(j,i)[2]);
				mGerr += abs(mG-frame_orig.at<Vec3b>(j,i)[1]);
				mBerr += abs(mB-frame_orig.at<Vec3b>(j,i)[0]);

				frame_orig.at<Vec3b>(j, i)[2] = (int) mR;
				frame_orig.at<Vec3b>(j, i)[1] = (int) mG;
				frame_orig.at<Vec3b>(j, i)[0] = (int) mB;
				//nr++;
			}
		}
	}

	colorDeviation = (mRerr+mGerr+mBerr)/(nr*3);
}

void trackCircles(Mat frame, Mat &frame_orig, vector<char*> &type,
		vector<CvPoint> &centerT, vector<int> &rad) {

	//convert it to gray
	cvtColor(frame, frame, CV_BGR2GRAY);

//	int erosion_size = 3;
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
//			cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
//			cv::Point(erosion_size, erosion_size));

//	erode(frame, frame, element);
//	dilate(frame, frame, element);
	//Canny(frame, frame, 5, 70, 3);
//	imwrite("canny.jpg", frame);
	//(frame, frame, cv.CV_GAUSSIAN, 15, 15)

	//reduce noise
//	GaussianBlur(frame, frame, Size(11, 11), 4, 4);

	//apply the Hough Transform to find circles
	vector<Vec3f> circles;
	HoughCircles(frame, circles, CV_HOUGH_GRADIENT, 1, frame.rows / 8, 70, 35,
			10, 0);

	// Draw the circles detected
	//int cx, cy; //, center_x_r, center_y_r, center_x_l, center_y_l;
	//int size=0;

	for (size_t i = 0; i < circles.size(); i++) {
		//circles[i].
		//if (isContourConvex(circles[i][0])){//int i=0;
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);

		centerT.push_back(center);
		rad.push_back(radius);

		char* color;
		int colorDeviation;
		determineMediumColorCircle(center, radius, frame_orig, color, colorDeviation);
		printf("%d, dev: %d \n", i, colorDeviation);
		// circle center
		circle(frame_orig, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//printf("%d: CX%d-CY%d    %d, %d\n",i, cx, cy,  center.x, center.y);
		// circle outline
		circle(frame_orig, center, radius, Scalar(0, 0, 255), 3, 8, 0);

		//writeFile(pf, "circle", color, center.x, center.y);
		type.push_back("circle");
	}
	//}
}

void realCircles(vector<CvPoint> &center_l, vector<CvPoint> &center_r, vector<int> &rad_l, vector<int> &rad_r, Mat left, Mat right){
	vector<CvPoint> final_l;
	vector<CvPoint> final_r;
	vector<int> radius_l;
	vector<int> radius_r;

	for (std::vector<int>::size_type i = 0; i != center_l.size(); i++){
		for (std::vector<int>::size_type j = 0; j != center_r.size(); j++){

			if (center_l[i].x>center_r[j].x-50
					&& center_l[i].x<center_r[j].x+50){
				final_l.push_back(center_l[i]);
				final_r.push_back(center_r[j]);
				radius_l.push_back(rad_l[i]);
				radius_r.push_back(rad_r[j]);
			}
		}
	}




	center_l.clear();
	center_r.clear();
	center_l=final_l;
	center_r=final_r;
	rad_l.clear();
	rad_r.clear();
	rad_l = radius_l;
	rad_r = radius_r;



	for (std::vector<int>::size_type i = 0; i != center_l.size(); i++){
		circle(left, center_l[i], 3, Scalar(0, 255, 0), -1, 8, 0);
		circle(left, center_l[i], rad_l[i], Scalar(0, 0, 255), 3, 8, 0);

		circle(right, center_r[i], 3, Scalar(0, 255, 0), -1, 8, 0);
		circle(right, center_r[i], rad_r[i], Scalar(0, 0, 255), 3, 8, 0);
	}

	imwrite("l.jpg", left);
	imwrite("r.jpg", right);


}

void *stereo_process(void *arg) {
	Mat frame_r = imread("img_right.jpg", 1);
	Mat frame_l = imread("img_left.jpg", 1);

	vector<CvPoint> center_l;
	vector<CvPoint> center_r;
	vector<int> radius_l, radius_r;
	vector<char*> type_l;
	vector<char*> type_r;

	Mat frame_orig_l;
	Mat frame_orig_r;

	printf("\nProcessing...\n");

	frame_orig_l = frame_l.clone();
	frame_orig_r = frame_r.clone();

	int c = cvWaitKey(10);

	FILE *pf = fopen("objects.txt", "w");

	trackCircles(frame_l, frame_orig_l, type_l, center_l,radius_l);
	trackObject(frame_l, frame_orig_l, type_l, center_l);
	//imshow("Result_l", frame_orig_l);

	//fclose(pf_l);

	imwrite("left.jpg", frame_orig_l);

	//FILE *pf_r=fopen("objects_r.txt","w");

	trackCircles(frame_r, frame_orig_r, type_r, center_r,radius_r);
	trackObject(frame_r, frame_orig_r, type_r, center_r);
	//imshow("Result_r", frame_orig_r);
	imwrite("right.jpg", frame_orig_r);

	//////////////////////////////////////////////////////////////////////////

	realCircles(center_l,center_r,radius_l,radius_r,frame_l,frame_r);

	double qu[4][4] = { { 1., 0., 0., -2.1951644134521484e+02 }, { 0., 1., 0.,
			-1.0995639705657959e+02 }, { 0., 0., 0., 3.1004862380124985e+02 }, {
			0., 0., 1.1079426723476694e-01, 2.3843288178930724e+00 } };

	/*{ { 1.0, 0.0, 0.0, -1.0384888534545898e+03 }, { 0.0,
	 1.0, 0.0, -8.9958347320556641e+01 }, { 0.0, 0.0, 0.0,
	 9.8627901063924435e+02 }, { 0.0, 0.0, 1.5284315798247780e-01,
	 2.9219960278349775e+00 } };*/

	if (center_l.size() > center_r.size())
		type_l = type_r;

	for (int i = 0; i < min(center_l.size(), center_r.size()); i++) {

		int d = center_r.at(i).x - center_l.at(i).x;

		double X = center_l.at(i).x * qu[0][0] + qu[0][3];

		double Y = center_l.at(i).y * qu[1][1] + qu[1][3];
		double Z = qu[2][3];
		double W = d * qu[3][2] + qu[3][3];

		X = X / W;
		Y = Y / W;
		Z = Z / W;

		int mR = frame_orig_l.at<Vec3b>(center_l.at(i).y, center_l.at(i).x + 5)[2];
		int mG = frame_orig_l.at<Vec3b>(center_l.at(i).y, center_l.at(i).x + 5)[1];
		int mB = frame_orig_l.at<Vec3b>(center_l.at(i).y, center_l.at(i).x + 5)[0];
		char* color = getColor(mR, mG, mB);

		printf("X: %d  Y: %d   Z: %d\n", (int) X, (int) Y, (int) Z);
		writeFile(pf, type_l.at(i), color, X, Y);
	}
	fclose(pf);



	//trackObject(frame, frame_orig);

	//imshow("Original_l", frame_l);
	//imshow("Original_r", frame_r);
	//imshow("Result", frame_orig);

	printf("\nProcessing done.\n");

}
