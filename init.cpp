//init.cpp

#include "init.h"

std::string sVideoPath;
cv::VideoCapture capVideo;
int frameWidth = 0;
int frameHeight = 0;
int totalFrames = 0;
int fps = 0;
int nNoOfPixelsOfBlockRow = 0;
int nNoOfPixelsOfBlockCol = 0;
LNArrayOfBlock LNArrayOfBlockObj[nNoOfBlockRow][nNoOfBlockCol];
int nNoOfFramesProcessed = 0;

//globals related to selected frames.
std::vector<LNFramesOfBlocks> arrayOfFramesOfBlocks;
int nFramesDeletedNonMatch = 0;
int nFramesDeletedSubset = 0;

//globals related to matching of previous frame to current
int nTotalNMB = 0; //NMB = Not Matching Block
int nTotalNMBPrev = 0;
int nTotalNMBPrevPrev = 0;
int nTotalBWithThresh = 0; //Total blocks having some threshold
int nTotalHumanPnts = 0;

//globals related to adaptive setting of precision of frame rate
int nStartFrame = 0;
int frameCount = 0;
int nCurrFrameNum = 0;
int nIgnoreNextFrames = 300;
int nIgnoreNextFramesMin = 20;
int nCurrFrameNumCache = 0;
int nIgnoreNextFramesCache = 0;
int nPrecisionToggleCount = 0;

void initialize()
{
	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "error reading video file" << std::endl << std::endl;      // show error message
		_getch();                    // it may be necessary to change or remove this line if not using Windows
		exit(-1);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: video file must have at least two frames";
		_getch();
		exit(-1);
	}

	frameWidth = capVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	frameHeight = capVideo.get(CV_CAP_PROP_FRAME_HEIGHT);
	totalFrames = capVideo.get(CV_CAP_PROP_FRAME_COUNT);
	fps = capVideo.get(CV_CAP_PROP_FPS);
	nNoOfPixelsOfBlockRow = frameHeight / nNoOfBlockRow;
	nNoOfPixelsOfBlockCol = frameWidth / nNoOfBlockCol;

	std::cout << "frame width = " << frameWidth << " frame height = " << frameHeight << "\n";
	std::cout << "total frames = " << totalFrames << " fps = " << fps << "\n";
	std::cout << "nNoOfPixelsOfBlockRow = " << nNoOfPixelsOfBlockRow << "\n";
	std::cout << "nNoOfPixelsOfBlockCol = " << nNoOfPixelsOfBlockCol << "\n";

	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			LNArrayOfBlockObj[r][c].nStartRow = r*nNoOfPixelsOfBlockRow;
			LNArrayOfBlockObj[r][c].nStartCol = c*nNoOfPixelsOfBlockCol;
			LNArrayOfBlockObj[r][c].nFinalRow = (r + 1)*nNoOfPixelsOfBlockRow;
			LNArrayOfBlockObj[r][c].nFinalCol = (c + 1)*nNoOfPixelsOfBlockCol;
			LNArrayOfBlockObj[r][c].nNoOfBlocks = 0;
		}
	}

}

void printStatistics()
{
	std::cout << "1. Frame width = " << frameWidth << "\n";
	std::cout << "2. Frame height = " << frameHeight << "\n";
	std::cout << "3. Total frames = " << totalFrames << " fps = " << fps << "\n";
	std::cout << "4. nNoOfPixelsOfBlockRow = " << nNoOfPixelsOfBlockRow << "\n";
	std::cout << "5. nNoOfPixelsOfBlockCol = " << nNoOfPixelsOfBlockCol << "\n";
	std::cout << "6. Total frames processed " << (LNArrayOfBlockObj[0][0].arrayOfBlock).size() 
		<< "\n";
	std::cout << "7. No. of times precision toggles " << nPrecisionToggleCount << "\n";
	std::cout << "8. Total frames selected " << arrayOfFramesOfBlocks.size() << "\n";
	std::cout << "9. Frames deleted due to non match " << nFramesDeletedNonMatch << "\n";
	std::cout << "10. Frames deleted due to subset " << nFramesDeletedSubset << "\n";
}