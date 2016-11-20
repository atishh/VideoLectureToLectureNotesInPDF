// LNArrayOfBlock.h

#ifndef _H_LNARRAYOFBLOCK_
#define _H_LNARRAYOFBLOCK_

#include "LNBlock.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
class LNArrayOfBlock {
public:
	// member variables
	std::vector<LNBlock> arrayOfBlock;
	int nStartRow;
	int nStartCol;
	int nFinalRow;
	int nFinalCol;
	int nNoOfBlocks;
};

#endif    // _H_LNARRAYOFBLOCK_