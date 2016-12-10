// main.cpp

#include "init.h"
#include "engine.h"
#include "postProcess.h"

#include <time.h>

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	clock_t tStart = clock();

	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	capVideo.open("../mod03lec10.mp4");
	//capVideo.open("../Lecture14.mp4");
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

		convertToBW(imgFrame1CopyLN);

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
	findLNOutputFrames(arrayOfFramesOfBlocks);

	deleteOverlappingFrames(arrayOfFramesOfBlocks);

	writeFramesToPdf(arrayOfFramesOfBlocks);

	//Post process ends here
	
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

	return(0);
}

