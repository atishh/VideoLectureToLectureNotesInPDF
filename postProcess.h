// postProcess.h

#ifndef _H_LNPOSTPROCESS_
#define _H_LNPOSTPROCESS_
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "init.h"
#include "engine.h"

//functions declarations.
extern void printWhitePixelsForDiag();
extern bool isBlockDifferentFromPrevBlock(int nFrame, int r, int c);
extern LNFramesOfBlocks matchingFramesOfBlocks(LNArrayOfBlock LNArrayOfBlockObj[][nNoOfBlockCol]
	, int nCurrLNOutputBlockNum
	, int nPrevLNOutputBlockNum);

#endif    // _H_LNPOSTPROCESS_