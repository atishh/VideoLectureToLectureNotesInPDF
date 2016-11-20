// ObjectTrackingCPP.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows

#include "Blob.h"
#include "LNArrayOfBlock.h"

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

const int nPrint = 0;
const int nNoOfBlockRow = 10;
const int nNoOfBlockCol = 10;

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

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
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

	cv::VideoCapture capVideo;

	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	std::vector<Blob> blobs;

	capVideo.open("../mod03lec10.mp4");
	//capVideo.open("../Lecture14.mp4");

	int nStartFrame = 50500;
	capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame);
	int frameCount = 2;
	int nCurrFrameNum = nStartFrame;

	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "error reading video file" << std::endl << std::endl;      // show error message
		_getch();                    // it may be necessary to change or remove this line if not using Windows
		return(0);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: video file must have at least two frames";
		_getch();
		return(0);
	}

	int frameWidth = capVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	int frameHeight = capVideo.get(CV_CAP_PROP_FRAME_HEIGHT);
	int totalFrames = capVideo.get(CV_CAP_PROP_FRAME_COUNT);

	int nNoOfPixelsOfBlockRow = frameHeight / nNoOfBlockRow;
	int nNoOfPixelsOfBlockCol = frameWidth / nNoOfBlockRow;

	std::cout << "frame width = " << frameWidth << "frame height = " << frameHeight << "\n";
	std::cout << "nNoOfPixelsOfBlockRow = " << nNoOfPixelsOfBlockRow << "\n";
	std::cout << "nNoOfPixelsOfBlockCol = " << nNoOfPixelsOfBlockCol << "\n";

	LNArrayOfBlock LNArrayOfBlockObj[nNoOfBlockRow][nNoOfBlockRow];
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			LNArrayOfBlockObj[r][c].nStartRow = r*nNoOfPixelsOfBlockRow;
			LNArrayOfBlockObj[r][c].nStartCol = c*nNoOfPixelsOfBlockCol;
			LNArrayOfBlockObj[r][c].nStartRow = (r+1)*nNoOfPixelsOfBlockRow;
			LNArrayOfBlockObj[r][c].nStartCol = (c+1)*nNoOfPixelsOfBlockCol;
			LNArrayOfBlockObj[r][c].nNoOfBlocks = 0;
		}
	}

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

		cv::threshold(imgFrame1CopyLN, imgFrame1CopyLN, 30, 255.0, CV_THRESH_BINARY);
		
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

		//Create line to separate sentences of different line.
		int continuousDiff = 0;
		for (int i = 0; i < frameHeight; i++) {
			float diff = 0;
			cv::Scalar intensity1 = imgFrame1CopyLN.at<uchar>(i, 0);
			for (int j = 1; j < frameWidth; j++) {
				cv::Scalar intensity2 = imgFrame1CopyLN.at<uchar>(i, j);
				diff += abs(intensity1.val[0] - intensity2.val[0]);
				intensity1 = intensity2;
			}
			if (nPrint) std::cout << "diff = " << diff << "\n";
			if (diff < 5000) {
				continuousDiff++;
				if (continuousDiff < 2) {
					for (int j = 0; j < frameWidth; j++) {
						imgFrame1CopyLN.at<uchar>(i, j) = 128;
					}
				}
				i = i + 10;
			} 
			else {
				continuousDiff = 0;
			}
			
		}
		
		cv::imshow("imgFrame1CopyLN", imgFrame1CopyLN);

		//cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

		// now we prepare for the next iteration

		frameCount++;
		std::cout << "frame count = " << frameCount << "\n";
		capVideo.set(CV_CAP_PROP_POS_FRAMES, nStartFrame+100*frameCount);
		nCurrFrameNum = nStartFrame + 100 * frameCount;
		if (nCurrFrameNum >= totalFrames) {
			break;
		}
		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES)) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame1);
		}
		else {
			std::cout << "end of video\n";
			break;
		}
		chCheckForEscKey = cv::waitKey(1);
	}

	int nNoOfFramesProcessed = LNArrayOfBlockObj[0][0].nNoOfBlocks;
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			std::cout << "WhitePixels[" << r << "][" << c << "] ";
			for (int i = 0; i < nNoOfFramesProcessed; i++) {
				std::cout << (LNArrayOfBlockObj[r][c].arrayOfBlock[i]).nNoOfPoints << " ";
			}
			std::cout << "\n";
		}
	}

	
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








