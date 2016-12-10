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


void findLNOutputFrames(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks)
{
	//Find possible LN output Frames
	int nCurrLNFrameIndex = 0;
	int nPrevLNFrameIndex = 0;

	for (int i = 1; i < nNoOfFramesProcessed; i++) {
		findTotalNMB(i);

		int nDiffOfNMBPercent = nTotalNMB - nTotalNMBPrev;
		nDiffOfNMBPercent = (nDiffOfNMBPercent * 100) / nTotalBlocks;
		int nTotalNMBPercent = (nTotalNMB * 100) / nTotalBlocks;
		int nTotalNMBPrevPercent = (nTotalNMBPrev * 100) / nTotalBlocks;
		//If 80% of total block doesn't matches && atmost 20% of previous total block doesn't match
		if (((nTotalNMBPercent > 70) && (nTotalNMBPrevPercent < 30)) ||
			((nDiffOfNMBPercent > 50) && (nTotalNMBPrev < 20))) {
			nPrevLNFrameIndex = nCurrLNFrameIndex;
			nCurrLNFrameIndex = i - 1;
			int nCurrFrameNum = (LNArrayOfBlockObj[0][0].arrayOfBlock[i - 1]).nFrameNum;
			LNFramesOfBlocks LNFramesOfBlocksObj;
			LNFramesOfBlocksObj = matchingFramesOfBlocks(LNArrayOfBlockObj, nCurrLNFrameIndex,
				nPrevLNFrameIndex);
			arrayOfFramesOfBlocks.push_back(LNFramesOfBlocksObj);
		}
	}

	//Check if Last Frame is correct, then add it.
	//findTotalNMB(nNoOfFramesProcessed - 1);

	if ((nTotalNMB * 10 < nTotalBlocks * 3) &&
		(nTotalBWithThresh * 10 > nTotalBlocks)) {
		nPrevLNFrameIndex = nCurrLNFrameIndex;
		nCurrLNFrameIndex = nNoOfFramesProcessed - 1;
		int nCurrFrameNum = (LNArrayOfBlockObj[0][0].arrayOfBlock[nNoOfFramesProcessed - 1]).nFrameNum;
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = matchingFramesOfBlocks(LNArrayOfBlockObj, nCurrLNFrameIndex,
			nPrevLNFrameIndex);
		arrayOfFramesOfBlocks.push_back(LNFramesOfBlocksObj);
	}
}


static void displayFramesOfBlocks(LNFramesOfBlocks LNFramesOfBlocksObj,
	std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks,
	cv::Mat& imgFrame1)
{
	int nLNOutputFrameNum = LNFramesOfBlocksObj.nCurrFrameNum;
	int nPrevFrameNum = LNFramesOfBlocksObj.nPrevFrameNum;
	cv::Mat imgFrame2;
	capVideo.set(CV_CAP_PROP_POS_FRAMES, nPrevFrameNum);
	capVideo.read(imgFrame1);
	std::string finalImageStr = "PreviousImage" + std::to_string(nLNOutputFrameNum) + std::to_string(0);
	//	cv::imshow(finalImage, imgFrame1);
	capVideo.set(CV_CAP_PROP_POS_FRAMES, nLNOutputFrameNum);
	capVideo.read(imgFrame1);
	finalImageStr = "IntermediateImage" + std::to_string(nLNOutputFrameNum) + std::to_string(0);
	//	cv::imshow(finalImage, imgFrame1);

	//Print the frames numbers.
	std::cout << "Possible LN Frames are ";
	for (int i = 0; i < arrayOfFramesOfBlocks.size(); i++) {
		int nLNOutputFrameNum = (arrayOfFramesOfBlocks[i]).nCurrFrameNum;
		std::cout << nLNOutputFrameNum << " ";
	}
	std::cout << "\n";

	std::cout << " Final Frame no " << nLNOutputFrameNum << "\n";
	int** ary = LNFramesOfBlocksObj.nFrameNumOfBlock;
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			int nFrameOfBlock = ary[r][c];
			std::cout << nFrameOfBlock << " ";
			if (nFrameOfBlock != nLNOutputFrameNum) {
				capVideo.set(CV_CAP_PROP_POS_FRAMES, nFrameOfBlock);
				capVideo.read(imgFrame2);
				int nStartRow = LNArrayOfBlockObj[r][c].nStartRow;
				int nStartCol = LNArrayOfBlockObj[r][c].nStartCol;
				int nFinalRow = LNArrayOfBlockObj[r][c].nFinalRow;
				int nFinalCol = LNArrayOfBlockObj[r][c].nFinalCol;
				int nWidth = nFinalCol - nStartCol;
				int nHeight = nFinalRow - nStartRow;
				cv::Mat roiSource(imgFrame2, cv::Rect(nStartCol, nStartRow, nWidth, nHeight)); //Region of interest
				cv::Mat roiDest(imgFrame1, cv::Rect(nStartCol, nStartRow, nWidth, nHeight)); //Region of interest
				roiSource.copyTo(roiDest);
			}
		}
		std::cout << "\n";
	}

}

void writeFramesToPdf(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks)
{
	std::list<Magick::Image> imageList;
	for (int i = 0; i < arrayOfFramesOfBlocks.size(); i++) {
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = arrayOfFramesOfBlocks[i];
		if (LNFramesOfBlocksObj.bIsDeleted) {
			continue;
		}
		int nLNOutputFrameNum = LNFramesOfBlocksObj.nCurrFrameNum;
		cv::Mat imgFrame1;
		displayFramesOfBlocks(LNFramesOfBlocksObj,
			arrayOfFramesOfBlocks, imgFrame1);
		drawDiagRectanges(imgFrame1, LNFramesOfBlocksObj.nCurrBlockNum);
		std::string finalImageStr = "FinalImage" + std::to_string(nLNOutputFrameNum) + std::to_string(0);
		std::string finalImageStr1 = "../tmp/" + finalImageStr + ".jpg";
		cv::imshow(finalImageStr1, imgFrame1);
		cv::imwrite(finalImageStr1, imgFrame1);

		Magick::readImages(&imageList, finalImageStr1);
	}
	std::cout << "\n";
	//Write the final pdf
	std::string finalImageStr3 = "../tmpP/finalImage.pdf";
	Magick::writeImages(imageList.begin(), imageList.end(), finalImageStr3);
}

bool IsSubsetOf(int nFrameIndex1, int nFrameIndex2)
{
	int nNoOfMatchingBlocks = 0;
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			int nNoOfPoints1 = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrameIndex1]).nNoOfPoints;
			int nNoOfPoints2 = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrameIndex2]).nNoOfPoints;
			int diff = abs(nNoOfPoints1 - nNoOfPoints2);
			if (nNoOfPoints1 < 10) {
				nNoOfMatchingBlocks++;
			}
			else if ((diff < (nNoOfPoints1 / 10))) {
				nNoOfMatchingBlocks++;
			}
		}
	}
	std::cout << "IsSubsetOf "<< nFrameIndex1 << " " << nFrameIndex2 <<
		" NoOfMatchingBlock = " << nNoOfMatchingBlocks << "\n";

	if ((nNoOfMatchingBlocks * 10) > (nTotalBlocks * 9)) {
		return true;
	}
	return false;
}

void deleteOverlappingFrames(std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks)
{
	std::list<Magick::Image> imageList;
	for (int i = 1; i < arrayOfFramesOfBlocks.size(); i++) {
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = arrayOfFramesOfBlocks[i - 1];
		int nFrameIndex1 = LNFramesOfBlocksObj.nCurrBlockNum;
		LNFramesOfBlocksObj = arrayOfFramesOfBlocks[i];
		int nFrameIndex2 = LNFramesOfBlocksObj.nCurrBlockNum;
		if (IsSubsetOf(nFrameIndex1, nFrameIndex2)) {
			arrayOfFramesOfBlocks[i - 1].bIsDeleted = true;
			std::cout << "frame = " << arrayOfFramesOfBlocks[i - 1].nCurrFrameNum
				<< " is deleted \n";
		}
		else {
			arrayOfFramesOfBlocks[i - 1].bIsDeleted = false;
		}
		arrayOfFramesOfBlocks[i].bIsDeleted = false;
	}
}