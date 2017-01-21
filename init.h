// init.h

#ifndef _H_LNINIT_
#define _H_LNINIT_
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#ifdef __linux__ 
    //linux code goes here
#elif _WIN32
    // windows code goes here
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows
#else
#endif

#include "LNArrayOfBlock.h"
#include "LNFrame.h"
#include "Magick++.h"

//const globals.
const int nPrint = 0;
const int nPrintPostProcess = 0;
const int nNoOfBlockRow = 10;
const int nNoOfBlockCol = 10;
const int nTotalBlocks = nNoOfBlockRow*nNoOfBlockCol;

const bool bEnableShiftAndDiff = 1;
const bool bDeleteHuman = true;
const int nDeleteHumanTh = 100; // Threshold no. of white pixels
const bool bShowImage = false;

//const global for image compression
const int nImageCompressionPercent = 95; //Making percent very high. Actual compression
										 //will happen in LectureNoteWaterMark

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

//other globals
extern std::string sVideoPath;
extern cv::VideoCapture capVideo;
extern int frameWidth;
extern int frameHeight;
extern int totalFrames;
extern int fps;
extern int nNoOfPixelsOfBlockRow;
extern int nNoOfPixelsOfBlockCol;
extern int nIgnoreNextFrames;
extern LNArrayOfBlock LNArrayOfBlockObj[nNoOfBlockRow][nNoOfBlockCol];
extern int nNoOfFramesProcessed;

//globals related to selected frames.
extern std::vector<LNFramesOfBlocks> arrayOfFramesOfBlocks;
extern int nFramesDeletedNonMatch;
extern int nFramesDeletedSubset;

//globals related to matching of previous frame to current
extern int nTotalNMB; //NMB = Not Matching Block
extern int nTotalNMBPrev;
extern int nTotalNMBPrevPrev;
extern int nTotalBWithThresh; //Total blocks having some threshold
extern int nTotalHumanPnts;

//globals related to adaptive setting of precision of frame rate
extern int nStartFrame;
extern int frameCount;
extern int nCurrFrameNum;
extern int nCurrTime;
extern int nIgnoreNextFrames;
extern int nIgnoreNextFramesMin;
extern int nCurrFrameNumCache;
extern int nIgnoreNextFramesCache;
extern int nPrecisionToggleCount;

//functions declarations.
extern void initialize();
extern void printStatistics();

#endif    // _H_LNINIT_
