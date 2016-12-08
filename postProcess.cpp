#include "postProcess.h"


void printWhitePixelsForDiag()
{
	if (nPrintPostProcess) {
		for (int r = 0; r < nNoOfBlockRow; r++) {
			for (int c = 0; c < nNoOfBlockCol; c++) {
				std::cout << "WhitePixels[" << r << "][" << c << "] ";
				for (int i = 0; i < nNoOfFramesProcessed; i++) {
					std::cout << (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nNoOfPoints << " ";
				}
				std::cout << "\n";
			}
		}
	}
}

bool isBlockDifferentFromPrevBlock(int nFrame, int r, int c)
{
	int prevNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrame - 1]).nNoOfPoints;
	int currNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrame]).nNoOfPoints;
	//Ideally the difference should be zero.
	int diff = abs(currNoOfPoints - prevNoOfPoints);
	if ((diff > (currNoOfPoints / 7)) && (diff > 10)) {
		return true;
	}
	return false;
}

LNFramesOfBlocks matchingFramesOfBlocks(LNArrayOfBlock LNArrayOfBlockObj[][nNoOfBlockCol]
	, int nCurrLNFrameIndex
	, int nPrevLNFrameIndex)
{
	LNFramesOfBlocks LNFramesOfBlocksObj;
	int** ary = new int*[nNoOfBlockRow];
	for (int i = 0; i < nNoOfBlockRow; ++i)
		ary[i] = new int[nNoOfBlockCol];
	LNFramesOfBlocksObj.nFrameNumOfBlock = ary;
	LNFramesOfBlocksObj.nPrevFrameNum =
		(LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNFrameIndex - 1]).nFrameNum;
	LNFramesOfBlocksObj.nCurrBlockNum = nCurrLNFrameIndex;
	LNFramesOfBlocksObj.nCurrFrameNum =
		(LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNFrameIndex]).nFrameNum;
	std::cout << " Final Frame no "
		<< (LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNFrameIndex]).nFrameNum
		<< "\n";
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			int nCorrectFrameNo = (LNArrayOfBlockObj[r][c].arrayOfBlock[nCurrLNFrameIndex]).nFrameNum;
			int bFound = 0;
			//Iterate backward till matching frame of block is found
			for (int i = nCurrLNFrameIndex; i >(nPrevLNFrameIndex + 1); i--) {
				int nCurrNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nNoOfPoints;
				int nPrevNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i - 1]).nNoOfPoints;
				int diff = abs(nCurrNoOfPoints - nPrevNoOfPoints);
				if (((nCurrNoOfPoints > 20) && (diff <= (nCurrNoOfPoints / 7))) ||
					((nCurrNoOfPoints <= 20) && (diff < 10))) {
					nCorrectFrameNo = (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nFrameNum;
					bFound = 1;
					break;
				}
			}
			ary[r][c] = nCorrectFrameNo;
			std::cout << nCorrectFrameNo << "," << bFound << " ";
		}
		std::cout << "\n";
	}
	return LNFramesOfBlocksObj;
}
