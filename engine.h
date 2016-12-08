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
extern void createBlocksOfFrame(cv::Mat& imgFrame1CopyLN, int nCurrFrameNum);
extern void drawDiagRectanges(cv::Mat& imgFrame1CopyLN, int nCurrentBlockTemp);


#endif    // _H_LNENGINE_