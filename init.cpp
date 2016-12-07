//init.cpp

#include "init.h"

cv::VideoCapture capVideo;
int frameWidth = 0;
int frameHeight = 0;
int totalFrames = 0;
int fps = 0;
int nNoOfPixelsOfBlockRow = 0;
int nNoOfPixelsOfBlockCol = 0;
LNArrayOfBlock LNArrayOfBlockObj[nNoOfBlockRow][nNoOfBlockCol];

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