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
	bool bIsDeleted;
};

#endif    // _H_LNFRAME_