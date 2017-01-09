// postProcess.h

#ifndef _H_LNPOSTPROCESS_
#define _H_LNPOSTPROCESS_
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include "init.h"
#include "engine.h"

#if defined _MSC_VER
#include <direct.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <fstream>

//functions declarations.
extern void printWhitePixelsForDiag();
extern bool isBlockDifferentFromPrevBlock(int nFrame, int r, int c);
extern LNFramesOfBlocks matchingFramesOfBlocks(LNArrayOfBlock LNArrayOfBlockObj[][nNoOfBlockCol]
	, int nCurrLNFrameIndex
	, int nPrevLNFrameIndex);
extern void findLNOutputFrames(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks);
extern void deleteOverlappingFrames(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks);
extern void writeFramesToPdf(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks);

#endif    // _H_LNPOSTPROCESS_