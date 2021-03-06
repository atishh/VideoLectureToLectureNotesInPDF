#include "engine.h"


static inline cv::Mat translateImg(cv::Mat &img, cv::Mat &imgShift, int offsetx, int offsety)
{
	cv::Mat trans_mat = (cv::Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img, imgShift, trans_mat, img.size());
	return trans_mat;
}

void shiftAndDiff(cv::Mat& imgFrame1CopyLN)
{
	if (bEnableShiftAndDiff) {
		cv::Mat imgFrame1CopyLNShift = cv::Mat::zeros(imgFrame1CopyLN.rows, imgFrame1CopyLN.cols, imgFrame1CopyLN.type());
		translateImg(imgFrame1CopyLN, imgFrame1CopyLNShift, -1, 0);
		cv::absdiff(imgFrame1CopyLN, imgFrame1CopyLNShift, imgFrame1CopyLN);
	}
	else {
		cv::Mat imgFrame1CopyLNOrig = imgFrame1CopyLN.clone();
		cv::GaussianBlur(imgFrame1CopyLNOrig, imgFrame1CopyLNOrig, cv::Size(5, 5), 0);
		if(bShowImage) cv::imshow("GausiaanBlur", imgFrame1CopyLNOrig);
		cv::absdiff(imgFrame1CopyLN, imgFrame1CopyLNOrig, imgFrame1CopyLN);
	}
}

void convertToBW(cv::Mat& imgFrame1CopyLN)
{
	cv::cvtColor(imgFrame1CopyLN, imgFrame1CopyLN, CV_BGR2GRAY);
	if(bShowImage) cv::imshow("GrayImage", imgFrame1CopyLN);

	shiftAndDiff(imgFrame1CopyLN);
	if(bShowImage) cv::imshow("DiffImage", imgFrame1CopyLN);

	cv::threshold(imgFrame1CopyLN, imgFrame1CopyLN, 20, 255.0, CV_THRESH_BINARY);
	if(bShowImage) cv::imshow("ThresholdImage", imgFrame1CopyLN);
}

void createBlocksOfFrame(cv::Mat& imgFrame1CopyLN, int nCurrFrameNum, cv::Mat& imgThresh)
{
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			LNBlock LNBlockObj;
			LNBlockObj.nFrameNum = nCurrFrameNum;
			LNBlockObj.nTime = nCurrTime;
			LNBlockObj.nNoOfPoints = 0;
			for (int i = r*nNoOfPixelsOfBlockRow; i < (r + 1)*nNoOfPixelsOfBlockRow; i++) {
				for (int j = c*nNoOfPixelsOfBlockCol; j < (c + 1)*nNoOfPixelsOfBlockCol; j++) {
					cv::Scalar intensity2 = imgFrame1CopyLN.at<uchar>(i, j);
					double intensity = intensity2.val[0];
					if (intensity == 255) {
						cv::Point pt;
						pt.y = i;
						pt.x = j;
						(LNBlockObj.whitePixels).push_back(pt);
						(LNBlockObj.nNoOfPoints)++;
					}
					if (bDeleteHuman) {
						intensity2 = imgThresh.at<uchar>(i, j);
						intensity = intensity2.val[0];
						if (intensity == 255) {
							(LNBlockObj.nNoOfHumanPoints)++;
						}
					}
				}
			}
			if (nPrint) std::cout << "LNBlockObj[" << r << "][" << c << "] = " << LNBlockObj.nNoOfPoints << "\n";
			(LNArrayOfBlockObj[r][c].arrayOfBlock).push_back(LNBlockObj);
			(LNArrayOfBlockObj[r][c].nNoOfBlocks)++;
		}
	}
}

void drawDiagRectanges(cv::Mat& imgFrame1CopyLN, int nCurrentBlockTemp)
{
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			int startPosY = r*nNoOfPixelsOfBlockRow;
			int endPosY = (r + 1)*nNoOfPixelsOfBlockRow;
			int startPosX = c*nNoOfPixelsOfBlockCol;
			int endPosX = (c + 1)*nNoOfPixelsOfBlockCol;
			cv::rectangle(imgFrame1CopyLN, cv::Point(startPosX, startPosY), cv::Point(endPosX, endPosY), cv::Scalar(110, 220, 0), 2, 8);
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			//	int nCurrentBlock = LNArrayOfBlockObj[r][c].nNoOfBlocks - 1;
			int nCurrentBlock = nCurrentBlockTemp;
			int noOfWhitePixels = (LNArrayOfBlockObj[r][c].arrayOfBlock[nCurrentBlock]).nNoOfPoints;
			int textPointX = (startPosX + endPosX) / 2;
			int textPointY = (startPosY + endPosY) / 2;
			cv::putText(imgFrame1CopyLN, std::to_string(noOfWhitePixels), cv::Point(textPointX, textPointY), intFontFace, 1, cv::Scalar(110, 220, 0), 1);
		}
	}
}

bool isBlockDifferentFromPrevBlock(int nFrame, int r, int c)
{
	if (bDeleteHuman) {
		if ((LNArrayOfBlockObj[r][c].arrayOfBlock[nFrame - 1]).nNoOfHumanPoints > nDeleteHumanTh)
			return false;
	}

	int prevNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrame - 1]).nNoOfPoints;
	int currNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[nFrame]).nNoOfPoints;
	//Ideally the difference should be zero.
	int diff = abs(currNoOfPoints - prevNoOfPoints);
	if ((diff > (currNoOfPoints / 7)) && (diff > 10)) {
		return true;
	}
	return false;
}

//Find total not matching blocks
void findTotalNMB(int fNo)
{
	nTotalNMBPrevPrev = nTotalNMBPrev;
	nTotalNMBPrev = nTotalNMB;
	nTotalNMB = 0;
	nTotalBWithThresh = 0;
	int nTotalPoints = 0;
	nTotalHumanPntsPrev = nTotalHumanPnts;
	nTotalHumanPnts = 0;
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			if (isBlockDifferentFromPrevBlock(fNo, r, c)) {
				nTotalNMB++;
			}
			int nNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[fNo]).nNoOfPoints;
			nTotalPoints += nNoOfPoints;
			if (nNoOfPoints > 50) {
				nTotalBWithThresh++;
			}
			nTotalHumanPnts += ((LNArrayOfBlockObj[r][c].arrayOfBlock[fNo]).nNoOfHumanPoints > nDeleteHumanTh);
		}
	}
	std::cout << "Ti = " << (LNArrayOfBlockObj[0][0].arrayOfBlock[fNo]).nTime
		<< " Fr = " << (LNArrayOfBlockObj[0][0].arrayOfBlock[fNo]).nFrameNum
		<< " ttlBlks = " << nTotalBlocks << " ttlNotMatchBlks = "
		<< nTotalNMB << " nTlPnts = " << nTotalPoints 
		<< " nTtlHumanPnts = " << nTotalHumanPnts << "\n" ;
}

bool isThisPossibleOutputFrame(int fNo, bool bRelax /*= false*/)
{
	bool bReturn = false;
	findTotalNMB(fNo);
	int nDiffOfNMBPercent = nTotalNMB - nTotalNMBPrev;
	nDiffOfNMBPercent = (nDiffOfNMBPercent * 100) / nTotalBlocks;
	int nTotalNMBPercent = (nTotalNMB * 100) / nTotalBlocks;
	int nTotalNMBPrevPercent = (nTotalNMBPrev * 100) / nTotalBlocks;
	//If 80% of total block doesn't matches && atmost 20% of previous total block doesn't match
	if (((nTotalNMBPercent > 70) && (nTotalNMBPrevPercent < 30)) ||
		((nDiffOfNMBPercent > 50) /*&& (nTotalNMBPrev < 20)*/)) {
		bReturn = true;
	}
	else if (bRelax && (nDiffOfNMBPercent > 40))
		bReturn = true;

	if (bDeleteHuman) {
		//less strict conditions than above
		int nDiffOfHumanPercent = nTotalHumanPnts - nTotalHumanPntsPrev;
		nDiffOfHumanPercent = (nDiffOfHumanPercent * 100) / nTotalBlocks;
		
		int nTotalHumanPntsPercent = (nTotalHumanPnts * 100) / nTotalBlocks;
		if ((nTotalNMBPercent > 70) && (nTotalNMBPrevPercent < 30))
			bReturn = true;
		if ((nDiffOfNMBPercent > 25) && (nTotalHumanPntsPercent > 80)) 
			bReturn = true;
		//Very Aggressive. Ideal for slide show. May remove this
		//if (((nDiffOfNMBPercent + nTotalHumanPntsPercent)> 45)) 
		//	bReturn = true;
		if (((nDiffOfNMBPercent + nTotalHumanPntsPercent)> 75)) 
			bReturn = true;
		if((nTotalNMBPrev < 10) && (nTotalHumanPntsPrev < 10)) {
			if((nDiffOfNMBPercent + nDiffOfHumanPercent) > 20)
				bReturn = true;
		}

		if (!bRelax) {
			//If a frame is matched then atleast 2 previous frames should be similar.
			nTotalNMBPrevPercent = (abs(nTotalNMBPrevPrev - nTotalNMBPrev) * 100) / nTotalBlocks;
			if (nTotalNMBPrevPercent > 20)
				bReturn = false;
		}
	}

	return bReturn;
}

int nInsideLowerPrecisionCount = 0;
bool isHigherPrecisionNeeded()
{
	int nCurrLNFrameIndex = LNArrayOfBlockObj[0][0].nNoOfBlocks - 1;
	if ((nIgnoreNextFrames > nIgnoreNextFramesMin) && (nCurrLNFrameIndex > 1)) {
		if(isThisPossibleOutputFrame(nCurrLNFrameIndex, true)) {
			nPrecisionToggleCount++;
			return true;
		}
		nInsideLowerPrecisionCount++;
	} else {
		nInsideLowerPrecisionCount  = 0;
	}
	return false;
}

void deleteLastBlock()
{
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			(LNArrayOfBlockObj[r][c].arrayOfBlock).pop_back();
			(LNArrayOfBlockObj[r][c].nNoOfBlocks)--;
		}
	}
}

void setHigherPrecisionFrameRate()
{
	std::cout << " Ignore next frames changed from " << nIgnoreNextFrames
		<< " to " << nIgnoreNextFramesMin << " \n";
	nCurrFrameNumCache = nCurrFrameNum;
	if(nInsideLowerPrecisionCount == 1) {
		//Start from previous 1 iteration
		nStartFrame = nCurrFrameNum - nIgnoreNextFrames;
		deleteLastBlock();
	}
	else {
		//Start from previous 2 iteration
		nStartFrame = nCurrFrameNum - 2 * nIgnoreNextFrames;
		deleteLastBlock();
		deleteLastBlock();
	}
	frameCount = 1;
	nIgnoreNextFramesCache = nIgnoreNextFrames;
	nIgnoreNextFrames = nIgnoreNextFramesMin;

}

bool isLowerPrecisionNeeded()
{
	if ((nCurrFrameNumCache > 0) && 
		(nCurrFrameNum > (nCurrFrameNumCache + nIgnoreNextFramesCache))) {
		return true;
	}
	return false;
}

void setLowerPrecisionFrameRate()
{
	std::cout << " Reverting Ignore next frames changed from " << nIgnoreNextFrames 
		<< " to " << nIgnoreNextFramesCache << "\n";
	nIgnoreNextFrames = nIgnoreNextFramesCache;
	nStartFrame = nCurrFrameNum;
	frameCount = 1;
	nCurrFrameNumCache = 0;
}

void findHuman(cv::Mat& imgFrame1, cv::Mat& imgFrame1Prev, 
	cv::Mat& imgThresh, cv::Mat& imgThreshPrev)
{
	if (bDeleteHuman == false)
		return;

	cv::Mat imgFrame1PrevCopyLN = imgFrame1Prev.clone();
	cv::Mat imgFrame2CopyLN = imgFrame1.clone();
	cv::cvtColor(imgFrame1PrevCopyLN, imgFrame1PrevCopyLN, CV_BGR2GRAY);
	cv::cvtColor(imgFrame2CopyLN, imgFrame2CopyLN, CV_BGR2GRAY);
	cv::GaussianBlur(imgFrame1PrevCopyLN, imgFrame1PrevCopyLN, cv::Size(5, 5), 0);
	cv::GaussianBlur(imgFrame2CopyLN, imgFrame2CopyLN, cv::Size(5, 5), 0);
	cv::Mat imgDifference;

	cv::absdiff(imgFrame1PrevCopyLN, imgFrame2CopyLN, imgDifference);
	cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);
//	if(bShowImage) cv::imshow("imgThreshForHuman", imgThresh);

	cv::bitwise_and(imgThresh, imgThreshPrev, imgThreshPrev);
	cv::bitwise_xor(imgThresh, imgThreshPrev, imgThresh);
//	if(bShowImage) cv::imshow("imgThreshBitwiseForHuman", imgThresh);

	cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	for (unsigned int i = 0; i < 2; i++) {
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		cv::erode(imgThresh, imgThresh, structuringElement5x5);
	}
	if(bShowImage) cv::imshow("imgDilateForHuman", imgThresh);
}

void deleteHuman(cv::Mat& imgFrame1CopyLN, cv::Mat& imgThresh)
{
	if (bDeleteHuman == false)
		return;

	cv::Mat imgThreshCopy = imgThresh.clone();
	cv::bitwise_not(imgThreshCopy, imgThreshCopy);
	cv::bitwise_and(imgFrame1CopyLN, imgThreshCopy, imgFrame1CopyLN);
	if(bShowImage) cv::imshow("ImageAfterSub", imgFrame1CopyLN);
}
