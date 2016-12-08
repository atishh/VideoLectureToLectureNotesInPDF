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
		cv::imshow("GausiaanBlur", imgFrame1CopyLNOrig);
		cv::absdiff(imgFrame1CopyLN, imgFrame1CopyLNOrig, imgFrame1CopyLN);
	}
}

void convertToBW(cv::Mat& imgFrame1CopyLN)
{
	cv::cvtColor(imgFrame1CopyLN, imgFrame1CopyLN, CV_BGR2GRAY);
	cv::imshow("GrayImage", imgFrame1CopyLN);

	shiftAndDiff(imgFrame1CopyLN);
	cv::imshow("DiffImage", imgFrame1CopyLN);

	cv::threshold(imgFrame1CopyLN, imgFrame1CopyLN, 20, 255.0, CV_THRESH_BINARY);
	cv::imshow("ThresholdImage", imgFrame1CopyLN);
}

void createBlocksOfFrame(cv::Mat& imgFrame1CopyLN, int nCurrFrameNum)
{
	for (int r = 0; r < nNoOfBlockRow; r++) {
		for (int c = 0; c < nNoOfBlockCol; c++) {
			LNBlock LNBlockObj;
			LNBlockObj.nFrameNum = nCurrFrameNum;
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