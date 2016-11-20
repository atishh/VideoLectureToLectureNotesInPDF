// LNBlock.h

#ifndef _H_LNBLOCK_
#define _H_LNBLOCK_

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class LNBlock {
public:
	// member variables
	std::vector<cv::Point> whitePixels;
	int nFrameNum;
	int nNoOfPoints;

};

#endif    // _H_LNBLOCK_