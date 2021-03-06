// engine.h

#ifndef _H_LNENGINE_
#define _H_LNENGINE_
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "init.h"

//functions declarations.
extern void shiftAndDiff(cv::Mat& imgFrame1CopyLN);
extern void convertToBW(cv::Mat& imgFrame1CopyLN);
extern void createBlocksOfFrame(cv::Mat& imgFrame1CopyLN, int nCurrFrameNum, cv::Mat& imgThresh);
extern void drawDiagRectanges(cv::Mat& imgFrame1CopyLN, int nCurrentBlockTemp);
extern bool isBlockDifferentFromPrevBlock(int nFrame, int r, int c);
extern void findTotalNMB(int fNo);
extern bool isThisPossibleOutputFrame(int fNo, bool bRelax = false);
extern bool isHigherPrecisionNeeded();
extern void setHigherPrecisionFrameRate();
extern bool isLowerPrecisionNeeded();
extern void setLowerPrecisionFrameRate();
extern void findHuman(cv::Mat& imgFrame1, cv::Mat& imgFrame1Prev,
	cv::Mat& imgThresh, cv::Mat& imgThreshPrev);
extern void deleteHuman(cv::Mat& imgFrame1CopyLN, cv::Mat& imgThresh);

#endif    // _H_LNENGINE_