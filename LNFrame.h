// LNBlock.h

#ifndef _H_LNFRAME_
#define _H_LNFRAME_


///////////////////////////////////////////////////////////////////////////////////////////////////
class LNFramesOfBlocks {
public:
	// member variables
	int** nFrameNumOfBlock;
	int nCurrFrameNum;
	int nPrevFrameNum;
	int nCurrBlockNum;
	int nCurrTime;
	bool bIsDeleted;

	LNFramesOfBlocks()
	{
		nFrameNumOfBlock = NULL;
		nCurrFrameNum = -1;
		nPrevFrameNum = -1;
		nCurrBlockNum = -1;
		nCurrTime = -1;
		bIsDeleted = false;
	}
};

#endif    // _H_LNFRAME_
