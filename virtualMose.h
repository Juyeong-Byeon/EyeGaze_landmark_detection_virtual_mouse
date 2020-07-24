#include <dlib/image_processing.h>
#include <opencv2/opencv.hpp>

#pragma once

struct Cordination {
	double X;
	double Y;
};

void doSelectedmouseAction(int SELECTED_BUTTON_ID, int x, int y);

void moveCursor(double newX, double newY, Cordination* precord, int POINTER_SENSITIVE);

bool isPosExceededThresh(std::vector<cv::Point> upperLipPoints, std::vector<cv::Point> LowerLipPoints, double thresh);


bool isyPixelExceededThresh(std::vector<cv::Point> upperLipPoints, std::vector<cv::Point> LowerLipPoints, double thresh);
