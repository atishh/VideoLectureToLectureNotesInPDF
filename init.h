// init.h

#ifndef _H_LNINIT_
#define _H_LNINIT_
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows

#include "LNArrayOfBlock.h"
#include "LNFrame.h"
#include "Magick++.h"

//const globals.
const int nPrint = 0;
const int nPrintPostProcess = 0;
const int nNoOfBlockRow = 10;
const int nNoOfBlockCol = 10;
const int nIgnoreNextFrames = 100;
const bool bEnableShiftAndDiff = 0;
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

//other globals
extern cv::VideoCapture capVideo;
extern int frameWidth;
extern int frameHeight;
extern int totalFrames;
extern int fps;
extern int nNoOfPixelsOfBlockRow;
extern int nNoOfPixelsOfBlockCol;
extern LNArrayOfBlock LNArrayOfBlockObj[nNoOfBlockRow][nNoOfBlockCol];
extern int nNoOfFramesProcessed;


//functions declarations.
extern void initialize();

#endif    // _H_LNINIT_