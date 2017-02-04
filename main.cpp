// main.cpp

#include "init.h"
#include "engine.h"
#include "postProcess.h"

#include <time.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

	clock_t tStart = clock();

	cv::Mat imgFrame1;
	cv::Mat imgFrame1Prev;
	cv::Mat imgThresh;
	cv::Mat imgThreshPrev;

	sVideoPath = argv[1];
	//sVideoPath = "../mod03lec10";
	//sVideoPath = "../Lecture14";
	//sVideoPath = "../MIT3_054S15_L15_300k";
	//sVideoPath = "../MIT6_006F11_lec02_300k";
	//sVideoPath = "../IndianGeography";
	//sVideoPath = "../The Lagrangian";
	//sVideoPath = ".. / IC_ENGINE";

	std::string sVideoName = sVideoPath + ".mp4";
	capVideo.open(sVideoName);

	nStartFrame = 100;
	//nStartFrame = 50500;

	capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame);
	frameCount = 1;
	nCurrFrameNum = nStartFrame;

	initialize();

	capVideo.read(imgFrame1Prev);
	capVideo.read(imgFrame1);
	imgThreshPrev = cv::Mat::zeros(imgFrame1.rows, imgFrame1.cols, CV_THRESH_BINARY);

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		if(bShowImage) cv::imshow("OrigImage", imgFrame1);

		findHuman(imgFrame1, imgFrame1Prev, imgThresh, imgThreshPrev);

		cv::Mat imgFrame1CopyLN = imgFrame1.clone();

		convertToBW(imgFrame1CopyLN);

		deleteHuman(imgFrame1CopyLN, imgThresh);

		//Populate Array of Block with Block of current frame. 
		createBlocksOfFrame(imgFrame1CopyLN, nCurrFrameNum, imgThresh);

		//Draw rectangles for diagnostics.
		drawDiagRectanges(imgFrame1CopyLN, LNArrayOfBlockObj[0][0].nNoOfBlocks - 1);
		if(bShowImage) cv::imshow("ImageWithLines", imgFrame1CopyLN);

		//cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

		// now we prepare for the next iteration

		if (isHigherPrecisionNeeded()) {
			setHigherPrecisionFrameRate();
		}
		else if (isLowerPrecisionNeeded()) {
			setLowerPrecisionFrameRate();
		}
		else {
			frameCount++;
		}

		capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame + nIgnoreNextFrames * frameCount);
		nCurrFrameNum = nStartFrame + nIgnoreNextFrames * frameCount;

		if (nCurrFrameNum >= totalFrames) {
			break;
		}
		//if (frameCount > 500)
		//	break;
		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES)) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			if (bDeleteHuman) {
				imgThreshPrev = imgThresh.clone();
				imgFrame1Prev = imgFrame1.clone();
			}
			capVideo.read(imgFrame1);
			cv::Size nSize = imgFrame1.size();
			if (nSize.height <= 0) {
				std::cout << "Breaking because height is 0 \n";
				break;
			}
			if (nSize.width <= 0) {
				std::cout << "Breaking because height is 0 \n";
				break;
			}
			nCurrTime = (int)capVideo.get(CV_CAP_PROP_POS_MSEC)/1000;
			std::cout << "time = " << nCurrTime << " frame count = " << frameCount
				<< " frame no = " << nCurrFrameNum << "\n";
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

	//Find possible LN output Frames
	findLNOutputFrames(arrayOfFramesOfBlocks);

	deleteOverlappingFrames(arrayOfFramesOfBlocks);

	writeFramesToPdf(arrayOfFramesOfBlocks);

	//Post process ends here

	printStatistics();
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		if(bShowImage) cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

	return(0);
}

