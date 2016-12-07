// main.cpp

#include "init.h"

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

// function prototypes ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);

bool IsIntensityDiffFromSurrounding(cv::Mat &imgLN, int r, int c)
{
	cv::Scalar intensity1 = imgLN.at<uchar>(r, c);
	int intensity = intensity1.val[0];
	int nNoOfIntensityDiff = 0;
	for (int i = r - 1; i < r + 2; i++) {
		for (int j = c - 1; j < c + 2; j++) {
			cv::Scalar intensity2 = imgLN.at<uchar>(i, j);
			if (intensity - intensity2.val[0] > 20) {
				nNoOfIntensityDiff++;
			}
		}
	}
	if (nNoOfIntensityDiff >= 4)
		return true;
	return false;
}

inline cv::Mat translateImg(cv::Mat &img, cv::Mat &imgShift, int offsetx, int offsety)
{
    cv::Mat trans_mat = (cv::Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img,imgShift,trans_mat,img.size());
	return trans_mat;
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

LNFramesOfBlocks calculateFramesOfBlocks(LNArrayOfBlock LNArrayOfBlockObj[][nNoOfBlockCol]
										, int nCurrLNOutputBlockNum 
										, int nPrevLNOutputBlockNum)
{
	LNFramesOfBlocks LNFramesOfBlocksObj;
	int** ary = new int*[nNoOfBlockRow];
	for (int i = 0; i < nNoOfBlockRow; ++i)
		ary[i] = new int[nNoOfBlockCol];
	LNFramesOfBlocksObj.nFrameNumOfBlock = ary;
	LNFramesOfBlocksObj.nPrevFrameNum = 
		(LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNOutputBlockNum-1]).nFrameNum;
	LNFramesOfBlocksObj.nCurrBlockNum = nCurrLNOutputBlockNum;
	LNFramesOfBlocksObj.nCurrFrameNum =
		(LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNOutputBlockNum]).nFrameNum;
	std::cout << " Final Frame no "
		<< (LNArrayOfBlockObj[0][0].arrayOfBlock[nCurrLNOutputBlockNum]).nFrameNum
		<< "\n";
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			int nCorrectFrameNo = (LNArrayOfBlockObj[r][c].arrayOfBlock[nCurrLNOutputBlockNum]).nFrameNum;
			//Iterate backward till matching frame of block is found
			for (int i = nCurrLNOutputBlockNum; i > (nPrevLNOutputBlockNum + 1); i--) {
				int nCurrNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nNoOfPoints;
				int nPrevNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i - 1]).nNoOfPoints;
				int diff = abs(nCurrNoOfPoints - nPrevNoOfPoints);
				if (((nCurrNoOfPoints > 20) && (diff <= (nCurrNoOfPoints / 7))) ||
					((nCurrNoOfPoints <= 20) && (diff < 10)))  {
					nCorrectFrameNo = (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nFrameNum;
					break;
				}
			}
			ary[r][c] = nCorrectFrameNo;
			std::cout << nCorrectFrameNo << " ";
		}
		std::cout << "\n";
	}
	return LNFramesOfBlocksObj;
}

void displayFramesOfBlocks(cv::VideoCapture &capVideo, LNFramesOfBlocks LNFramesOfBlocksObj,
	int nLNOutputFrameNum, std::vector<int> &arrayOfPossibleLNFrame,
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
	for (int i = 0; i < arrayOfPossibleLNFrame.size(); i++) {
		int nLNOutputFrameNum = arrayOfPossibleLNFrame[i];
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

	std::vector<Blob> blobs;

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

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		cv::cvtColor(imgFrame1CopyLN, imgFrame1CopyLN, CV_BGR2GRAY);

		cv::imshow("GrayImage", imgFrame1CopyLN);

		cv::Mat imgFrame1CopyLNShift = cv::Mat::zeros(imgFrame1CopyLN.rows, imgFrame1CopyLN.cols, imgFrame1CopyLN.type());
		translateImg(imgFrame1CopyLN, imgFrame1CopyLNShift, -1, 0);

		cv::Mat imgFrame1CopyLNOrig = imgFrame1CopyLN.clone();
		cv::GaussianBlur(imgFrame1CopyLNOrig, imgFrame1CopyLNOrig, cv::Size(5, 5), 0);
		cv::GaussianBlur(imgFrame1CopyLNOrig, imgFrame1CopyLNOrig, cv::Size(5, 5), 0);
		cv::imshow("GausiaanBlur", imgFrame1CopyLNOrig);

		cv::absdiff(imgFrame1CopyLN, imgFrame1CopyLNShift, imgFrame1CopyLN);
		cv::imshow("DiffImage", imgFrame1CopyLN);

		cv::threshold(imgFrame1CopyLN, imgFrame1CopyLN, 20, 255.0, CV_THRESH_BINARY);
		
		cv::imshow("ThresholdImage", imgFrame1CopyLN);

		//Populate Array of Block with Block of current frame. 
		for (int r = 0; r < nNoOfBlockRow; r++) {
			for (int c = 0; c < nNoOfBlockCol; c++) {

				LNBlock LNBlockObj;
				LNBlockObj.nFrameNum = nCurrFrameNum;
				LNBlockObj.nNoOfPoints = 0;
				for (int i = r*nNoOfPixelsOfBlockRow; i < (r + 1)*nNoOfPixelsOfBlockRow; i++) {
					for (int j = c*nNoOfPixelsOfBlockCol; j < (c + 1)*nNoOfPixelsOfBlockCol; j++) {
						cv::Scalar intensity2 = imgFrame1CopyLN.at<uchar>(i, j);
						int intensity = intensity2.val[0];
						if (intensity == 255) {
							cv::Point pt;
							pt.y = i;
							pt.x = j;
							(LNBlockObj.whitePixels).push_back(pt);
							(LNBlockObj.nNoOfPoints)++;
						}
					}
				}
				if (nPrint) std::cout << "LNBlockObj[" << r << "][" << c << "] = " << LNBlockObj.nNoOfPoints << "\n";
				(LNArrayOfBlockObj[r][c].arrayOfBlock).push_back(LNBlockObj);
				(LNArrayOfBlockObj[r][c].nNoOfBlocks)++;

			}
		}

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
	int nNoOfFramesProcessed = LNArrayOfBlockObj[0][0].nNoOfBlocks;

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

	std::vector<LNFramesOfBlocks> arrayOfFramesOfBlocks;

	//Find possible LN output Frames
	std::vector<int> arrayOfPossibleLNFrame;
	int nTotalBlocks = nNoOfBlockRow*nNoOfBlockCol;
	int nTotalNotMatchingBlock  = 0;
	int nTotalNotMatchingBlockPrev = 0;
	int nCurrLNOutputBlockNum = 0;
	int nPrevLNOutputBlockNum = 0;

	for (int i = 1; i < nNoOfFramesProcessed; i++) {
		nTotalNotMatchingBlockPrev = nTotalNotMatchingBlock;
		nTotalNotMatchingBlock = 0;
		for (int r = 0; r < nNoOfBlockRow; r++) {
			for (int c = 0; c < nNoOfBlockCol; c++) {
				int prevNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i - 1]).nNoOfPoints;
				int currNoOfPoints = (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nNoOfPoints;
				//Ideally the difference should be zero.
				int diff = abs(currNoOfPoints - prevNoOfPoints);
				if ((diff > (currNoOfPoints / 7)) && (diff > 10)) {
					nTotalNotMatchingBlock++;
				}
			}
		}
		 
		std::cout << "Frame = " << (LNArrayOfBlockObj[0][0].arrayOfBlock[i - 1]).nFrameNum  
			<<" totalBlocks = " << nTotalBlocks << " totalNotMatchingBlocks = " 
			<< nTotalNotMatchingBlock << "\n";

		int nDiffOfNMBPercent = nTotalNotMatchingBlock - nTotalNotMatchingBlockPrev;
		nDiffOfNMBPercent = (nDiffOfNMBPercent * 100) / nTotalBlocks;
		int nTotalNMBPercent = (nTotalNotMatchingBlock * 100) / nTotalBlocks;
		int nTotalNMBPrevPercent = (nTotalNotMatchingBlockPrev * 100) / nTotalBlocks;
		//If 80% of total block doesn't matches && atmost 20% of previous total block doesn't match
		if (((nTotalNMBPercent > 70) && (nTotalNMBPrevPercent < 30)) ||
			((nDiffOfNMBPercent > 50) && (nTotalNotMatchingBlockPrev < 20))) {
			nPrevLNOutputBlockNum = nCurrLNOutputBlockNum;
			nCurrLNOutputBlockNum = i - 1;
			int nCurrFrameNum = (LNArrayOfBlockObj[0][0].arrayOfBlock[i - 1]).nFrameNum;
			arrayOfPossibleLNFrame.push_back(nCurrFrameNum);
			LNFramesOfBlocks LNFramesOfBlocksObj;
			LNFramesOfBlocksObj = calculateFramesOfBlocks(LNArrayOfBlockObj, nCurrLNOutputBlockNum,
				nPrevLNOutputBlockNum);
			arrayOfFramesOfBlocks.push_back(LNFramesOfBlocksObj);
		}
	}
	//Check if Last Frame is correct, then add it.
	if ((nTotalNotMatchingBlock * 10 < nTotalBlocks * 3)) {
		nPrevLNOutputBlockNum = nCurrLNOutputBlockNum;
		nCurrLNOutputBlockNum = nNoOfFramesProcessed - 1;
		int nCurrFrameNum = (LNArrayOfBlockObj[0][0].arrayOfBlock[nNoOfFramesProcessed - 1]).nFrameNum;
		arrayOfPossibleLNFrame.push_back(nCurrFrameNum);
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = calculateFramesOfBlocks(LNArrayOfBlockObj, nCurrLNOutputBlockNum,
			nPrevLNOutputBlockNum);
		arrayOfFramesOfBlocks.push_back(LNFramesOfBlocksObj);
	}


	std::list<Magick::Image> imageList;
	for (int i = 0; i < arrayOfPossibleLNFrame.size(); i++) {
		int nLNOutputFrameNum = arrayOfPossibleLNFrame[i];
		LNFramesOfBlocks LNFramesOfBlocksObj;
		LNFramesOfBlocksObj = arrayOfFramesOfBlocks[i];
		cv::Mat imgFrame1;
		displayFramesOfBlocks(capVideo, LNFramesOfBlocksObj, nLNOutputFrameNum, 
			arrayOfPossibleLNFrame, LNArrayOfBlockObj, imgFrame1);
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {
			if (existingBlobs[i].blnStillBeingTracked == true) {
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 1.15) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}








