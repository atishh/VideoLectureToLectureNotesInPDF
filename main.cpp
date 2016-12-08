// main.cpp

#include "init.h"
#include "engine.h"
#include "postProcess.h"

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

void displayFramesOfBlocks(cv::VideoCapture &capVideo, LNFramesOfBlocks LNFramesOfBlocksObj,
	int nLNOutputFrameNum, std::vector<LNFramesOfBlocks>& arrayOfFramesOfBlocks,
	LNArrayOfBlock LNArrayOfBlockObj[][nNoOfBlockCol], cv::Mat& imgFrame1)
{
	cv::Mat imgFrame2;
	int nPrevFrameNum = LNFramesOfBlocksObj.nPrevFrameNum;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	//capVideo.open("../mod03lec10.mp4");
	capVideo.open("../Lecture14.mp4");
	//capVideo.open("../MIT3_054S15_L15_300k.mp4");
	//capVideo.open("../MIT6_006F11_lec02_300k.mp4");
	//capVideo.open("../IndianGeography.mp4");
	//capVideo.open("../The Lagrangian.mp4");
	//capVideo.open("../IC_ENGINE.mp4");

	//int nStartFrame = 500;
	int nStartFrame = 50500;

	capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame);
	int frameCount = 2;
	int nCurrFrameNum = nStartFrame;

	initialize();

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		cv::imshow("OrigImage", imgFrame1);

		cv::Mat imgFrame1CopyLN = imgFrame1.clone();

		cv::cvtColor(imgFrame1CopyLN, imgFrame1CopyLN, CV_BGR2GRAY);

		cv::imshow("GrayImage", imgFrame1CopyLN);

		shiftAndDiff(imgFrame1CopyLN);

		cv::imshow("DiffImage", imgFrame1CopyLN);

		cv::threshold(imgFrame1CopyLN, imgFrame1CopyLN, 20, 255.0, CV_THRESH_BINARY);
		
		cv::imshow("ThresholdImage", imgFrame1CopyLN);

		//Populate Array of Block with Block of current frame. 
		createBlocksOfFrame(imgFrame1CopyLN, nCurrFrameNum);

		//Draw rectangles for diagnostics.
		drawDiagRectanges(imgFrame1CopyLN, LNArrayOfBlockObj[0][0].nNoOfBlocks - 1);
		cv::imshow("ImageWithLines", imgFrame1CopyLN);

		//cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

		// now we prepare for the next iteration

		frameCount++;
		std::cout << "frame count = " << frameCount << "\n";
		capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame + nIgnoreNextFrames * frameCount);
		nCurrFrameNum = nStartFrame + nIgnoreNextFrames * frameCount;
		if (nCurrFrameNum >= totalFrames) {
			break;
		}
		if (frameCount > 500)
			break;
		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES)) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame1);
		}
		else {
			std::cout << "end of video\n";
			break;
		}
		chCheckForEscKey = cv::waitKey(1);
	}

	//Post process starts here
	nNoOfFramesProcessed = LNArrayOfBlockObj[0][0].nNoOfBlocks;

	printWhitePixelsForDiag();

	std::vector<LNFramesOfBlocks> arrayOfFramesOfBlocks;

	//Find possible LN output Frames
	int nTotalBlocks = nNoOfBlockRow*nNoOfBlockCol;
	int nTotalNMB  = 0; //NMB = Not Matching Block
	int nTotalNMBPrev = 0;
	int nCurrLNFrameIndex = 0;
	int nPrevLNFrameIndex = 0;

	for (int i = 1; i < nNoOfFramesProcessed; i++) {
		nTotalNMBPrev = nTotalNMB;
		nTotalNMB = 0;
		for (int r = 0; r < nNoOfBlockRow; r++) {
			for (int c = 0; c < nNoOfBlockCol; c++) {
				if (isBlockDifferentFromPrevBlock(i, r, c)) {
					nTotalNMB++;
				}
			}
		}
		 
		std::cout << "Frame = " << (LNArrayOfBlockObj[0][0].arrayOfBlock[i - 1]).nFrameNum  
			<<" totalBlocks = " << nTotalBlocks << " totalNotMatchingBlocks = " 
			<< nTotalNMB << "\n";

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
	if ((nTotalNMB * 10 < nTotalBlocks * 3)) {
		nPrevLNFrameIndex = nCurrLNFrameIndex;
		nCurrLNFrameIndex = nNoOfFramesProcessed - 1;
		int nCurrFrameNum = (LNArrayOfBlockObj[0][0].arrayOfBlock[nNoOfFramesProcessed - 1]).nFrameNum;
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = matchingFramesOfBlocks(LNArrayOfBlockObj, nCurrLNFrameIndex,
			nPrevLNFrameIndex);
		arrayOfFramesOfBlocks.push_back(LNFramesOfBlocksObj);
	}


	std::list<Magick::Image> imageList;
	for (int i = 0; i < arrayOfFramesOfBlocks.size(); i++) {
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = arrayOfFramesOfBlocks[i];
		int nLNOutputFrameNum = LNFramesOfBlocksObj.nCurrFrameNum;
		cv::Mat imgFrame1;
		displayFramesOfBlocks(capVideo, LNFramesOfBlocksObj, nLNOutputFrameNum, 
			arrayOfFramesOfBlocks, LNArrayOfBlockObj, imgFrame1);
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

	//Post process ends here
	
	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

	return(0);
}

