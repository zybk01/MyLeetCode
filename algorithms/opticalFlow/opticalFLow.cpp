#include "ThreadPool.h"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <fstream>
#include <iostream>
#include <opencv2/stitching.hpp>
#include <variant>
#include <windows.h>

using namespace cv;
using namespace std;

struct siftDP
{
    cv::Point pos;
    int dir;
    double sigma;
    double loD;
    bool operator<(const siftDP sift)
    {
        if (pos.x < sift.pos.x)
        {
            return true;
        }
        else if (pos.x == sift.pos.x && pos.y < sift.pos.y)
        {
            return true;
        }
        return false;
    }
    bool operator()(const cv::Point &l, const cv::Point &r) const
    {
        if (l.x < r.x)
        {
            return true;
        }
        else if (l.x == r.x && l.y < r.y)
        {
            return true;
        }
        return false;
    };
};

static void drawOptFlowMap(const Mat &flow, Mat &cflowmap, int step,
                           double, const Scalar &color)
{
    for (int y = 0; y < cflowmap.rows; y += step)
        for (int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f &fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
                 color);
            circle(cflowmap, Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), 2, color, -1);
        }
}

template <class T>
T Interpolate(Point_<T> pos, Mat& mat)
{
    int p1r = floor(pos.y), p1c = floor(pos.x), p2r = floor(pos.y), p2c = floor(pos.x) + 1, p3r = floor(pos.y) + 1, p3c = floor(pos.x), p4r = floor(pos.y) + 1, p4c = floor(pos.x) + 1;
    T value = (1 + (p1r - pos.y)) * (1 + (p1c - pos.x)) * (mat.at<T>(p1r, p1c)) + (1 + (p2r - pos.y)) * (1 - (p2c - pos.x)) * (mat.at<T>(p2r, p2c)) +
              (1 - (p3r - pos.y)) * (1 + (p3c - pos.x)) * (mat.at<T>(p3r, p3c)) + (1 - (p4r - pos.y)) * (1 - (p4c - pos.x)) * (mat.at<T>(p4r, p4c));
    return value;
}

#ifndef LK_TRACKER
#define LK_TRACKER

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

void trace(string out) { 
   
  cout << out << endl;
};

struct TrackedPoint { 
   
  Point point;
  Mat opticalFlow;
  TrackedPoint() { 
   };
  TrackedPoint(const Point p, const Mat of):point(p), opticalFlow(of) { 
   };
};

class LKTracker { 
   
  private:
    int maxPyramidLayer = 3;
    int wx = 2;
    int wy = 2;
    int maxIteration = 50;
    double opticalflowResidual = 0.0001;

  private:
    static int getMatInt(Mat mat, int row, int col);
    static double getMatDouble(Mat mat, int row, int col);
    static double getMatDouble(Mat mat, double row, double col);
    void lowpassFilter(InputArray src, OutputArray dst);
    Mat calcGradientMatrix(InputArray frame, Point2f p);
    Mat calcMismatchVector(InputArray preFrame, InputArray curFrame, Point2f p, Mat g, Mat v);
    vector<Mat> buildPyramid(InputArray src);
    double calcResidual(Mat mat);
    bool isOpticalFlowValid(Mat of);
    double calcHarrisResponse(Mat gradient, double alpha);

  public:
    LKTracker();
    ~LKTracker();
    vector<TrackedPoint> track(InputArray preFrame, InputArray curFrame, vector<Point> keyPoints);
    vector<TrackedPoint> trackAll(InputArray preFrame, InputArray curFrame, double qualityLevel);
};

#endif


LKTracker::LKTracker() { 
   

}

LKTracker::~LKTracker() { 
   

}

int LKTracker::getMatInt(Mat mat, int row, int col) { 
   
	Size size = mat.size();
	if (col < 0 || col >= size.width || 
		  row < 0 || row >= size.height) { 
   
		return 0;
	}
	return mat.at<uchar>(row, col);
}

double LKTracker::getMatDouble(Mat mat, int row, int col) { 
   
	Size size = mat.size();
	if (col < 0 || col >= size.width || 
		  row < 0 || row >= size.height) { 
   
		return 0;
	}
	if (mat.type() == CV_64F) { 
   
		return mat.at<double>(row, col);
	} else { 
   
		return (double)mat.at<uchar>(row, col);
	}
}

double LKTracker::getMatDouble(Mat mat, double row, double col) { 
   
	Size size = mat.size();
	if (col < 0 || col >= size.width || 
		  row < 0 || row >= size.height) { 
   
		return 0;
	}
	int floorRow = floor(row);
	int floorCol = floor(col);
	double fracRow = row - floorRow;
	double fracCol = col - floorCol;
	int ceilRow = floorRow + 1;
	int ceilCol = floorCol + 1;

	return ((1.0 - fracRow) * (1.0 - fracCol) * getMatDouble(mat, floorRow, floorCol) + 
		(fracRow * (1.0 - fracCol) * getMatDouble(mat, ceilRow, floorCol)) + 
		((1.0 - fracRow) * fracCol * getMatDouble(mat, floorRow, ceilCol)) +
		(fracRow * fracCol * getMatDouble(mat, ceilRow, ceilCol))
	);
}

void LKTracker::lowpassFilter(InputArray src, OutputArray dst) { 
   
  Mat srcMat = src.getMat();
	int dstWidth = srcMat.size().width / 2;
	int dstHeight = srcMat.size().height / 2;
	// dst.create(Size(dstWidth, dstHeight), srcMat.type());
	dst.create(Size(dstWidth, dstHeight), CV_64F);
	Mat dstMat = dst.getMat();
	for (int x = 0; x < dstHeight; x++) { 
   
		for (int y = 0; y < dstWidth; y++) { 
   
			double val = 0;
			val += getMatInt(srcMat, 2*x, 2*y) * 0.25;
			val += getMatInt(srcMat, 2*x-1, 2*y) * 0.125;
			val += getMatInt(srcMat, 2*x+1, 2*y) * 0.125;
			val += getMatInt(srcMat, 2*x, 2*y-1) * 0.125;
			val += getMatInt(srcMat, 2*x, 2*y+1) * 0.125;
			val += getMatInt(srcMat, 2*x-1, 2*y-1) * 0.0625;
			val += getMatInt(srcMat, 2*x+1, 2*y-1) * 0.0625;
			val += getMatInt(srcMat, 2*x-1, 2*y+1) * 0.0625;
			val += getMatInt(srcMat, 2*x+1, 2*y+1) * 0.0625;
			// dstMat.at<uchar>(x, y) = (uchar)val;
			dstMat.at<double>(x, y) = val;
		}
	}
}

vector<Mat> LKTracker::buildPyramid(InputArray src) { 
   
	vector<Mat> layers;
	Mat currLayer = src.getMat();
	layers.push_back(currLayer);
	for (int i = 0; i < this->maxPyramidLayer; i++) { 
   
		Mat nextLayer;
		this->lowpassFilter(currLayer, nextLayer);
		layers.push_back(nextLayer);
		currLayer = nextLayer;
	}
	return layers;
}

Mat LKTracker::calcGradientMatrix(InputArray frame, Point2f p) { 
   
	Mat frameMat = frame.getMat();
	Mat mat(2, 2, CV_64F, Scalar(0));
	int pRow = p.y, pCol = p.x;
	for (int x = pRow - wx; x <= pRow + wx; x++) { 
   
		for (int y = pCol - wy; y <= pCol + wy; y++) { 
   
			double ix = (getMatDouble(frameMat, x + 1, y) - getMatDouble(frameMat, x - 1, y)) / 2.0;
			double iy = (getMatDouble(frameMat, x, y + 1) - getMatDouble(frameMat, x, y - 1)) / 2.0;
			mat.at<double>(0, 0) += ix * ix;
			mat.at<double>(0, 1) += ix * iy;
			mat.at<double>(1, 0) += ix * iy;
			mat.at<double>(1, 1) += iy * iy;
		}
	}
	return mat;
}

Mat LKTracker::calcMismatchVector(InputArray preFrame, InputArray curFrame, Point2f p, Mat g, Mat v) { 
   
	Mat preMat = preFrame.getMat();
	Mat curMat = curFrame.getMat();
	Mat mismatchVector(2, 1, CV_64F, Scalar(0));
	int pRow = p.y, pCol = p.x;
	for (int x = pRow - wx; x <= pRow + wx; x++) { 
   
		for (int y = pCol - wy; y <= pCol + wy; y++) { 
   
			double curX = 1.0 * x + g.at<double>(0, 0) + v.at<double>(0, 0);
			double curY = 1.0 * y + g.at<double>(1, 0) + v.at<double>(1, 0);
			double ix = (getMatDouble(preMat, x + 1, y) - getMatDouble(preMat, x - 1, y)) / 2.0;
			double iy = (getMatDouble(preMat, x, y + 1) - getMatDouble(preMat, x, y - 1)) / 2.0;
			double diff = getMatDouble(preMat, x, y) - getMatDouble(curMat, curX, curY);
			mismatchVector.at<double>(0, 0) += diff * ix;
			mismatchVector.at<double>(1, 0) += diff * iy;
		}
	}
	return mismatchVector;
}

double LKTracker::calcResidual(Mat mat) { 
   
	return sqrt(pow(mat.at<double>(0, 0), 2) + pow(mat.at<double>(1, 0), 2));
}

bool LKTracker::isOpticalFlowValid(Mat of) { 
   
	return (abs(of.at<double>(0, 0)) <= 50 && abs(of.at<double>(0, 0)) >= 1 
		&& abs(of.at<double>(1, 0)) <= 50 && abs(of.at<double>(1, 0)) >= 1);
}

vector<TrackedPoint> LKTracker::track(InputArray preFrame, InputArray curFrame, vector<Point> keyPoints) { 
   
	vector<Mat> prePyramid = this->buildPyramid(preFrame);
	vector<Mat> curPyramid = this->buildPyramid(curFrame);
	vector<TrackedPoint> tPoints;
	for (unsigned int i = 0; i < keyPoints.size(); i++) { 
   
		Mat g(2, 1, CV_64F, Scalar(0));
		Point srcPoint = keyPoints[i];
		bool isValid = true;
		for (int j = this->maxPyramidLayer - 1; j > 0; j--) { 
   
			Mat preMat = prePyramid[j];
			Mat curMat = curPyramid[j];
			Point2f prePoint;
			prePoint.x = 1.0 * srcPoint.x / pow(2.0, j);
			prePoint.y = 1.0 * srcPoint.y / pow(2.0, j);
			Mat gradient = calcGradientMatrix(preMat, prePoint);
			Mat gradientInv = gradient.inv();
			Mat v(2, 1, CV_64F, Scalar(0));
			int iterCount = 0;
			double residual = 1;
			while(iterCount < this->maxIteration && residual > this->opticalflowResidual) { 
   
				iterCount++;
				Mat mismatch = calcMismatchVector(preMat, curMat, prePoint, g, v);
				Mat delta = gradientInv * mismatch;
				v += delta;
				residual = calcResidual(delta);
			}

			if (iterCount >= this->maxIteration) { 
   
				isValid = false;
				break;
			}

			if (j == 0) { 
   
				g = g + v;
			} else { 
   
				g = 2 * (g + v);
			}
		}

		if (isValid && isOpticalFlowValid(g)) { 
   
			Point dstPoint;
			dstPoint.x = (int)(srcPoint.x + g.at<double>(1, 0));
			dstPoint.y = (int)(srcPoint.y + g.at<double>(0, 0));
			TrackedPoint tPoint(dstPoint, g);
			tPoints.push_back(tPoint);
		}
	}
	return tPoints;
}

double LKTracker::calcHarrisResponse(Mat gradient, double alpha) { 
   
	double A = gradient.at<double>(0, 0);
	double B = gradient.at<double>(0, 1);
	double C = gradient.at<double>(1, 1);
	double det = A * C - B * B;
	double trace = A + C;
	return det - trace * trace * alpha;
}

vector<TrackedPoint> LKTracker::trackAll(InputArray preFrame, InputArray curFrame, double qualityLevel) { 
   
  Mat preMat = preFrame.getMat();
	vector<Point> keyPoints;
	goodFeaturesToTrack(preFrame, keyPoints, 1000, 0.01, 10, Mat());
	return track(preFrame, curFrame, keyPoints);
}

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/sift/phone11.jpg");
    string rImage("resources/sift/phone12.jpg");
    if (num == 1)
    {
        LOGD("-l <image input path 1> -r <image input path 2> defaultPath lImage.jpg rImage.jpg");
    }
    int threashold = 10;
    float resizeRatio = 4;
    if (args)
    {
        for (int i = 0; i < num; i++)
        {
            if (args[i])
            {
                if (strcmp(args[i], "-l") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    lImage = string(input);
                }
                else if (strcmp(args[i], "-r") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    rImage = string(input);
                }
                else if (strcmp(args[i], "-t") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    threashold = atoi(input);
                }
                else if (strcmp(args[i], "-resize") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    resizeRatio = atoi(input);
                }
            }
        }
    }
    cv::Mat imageL = cv::imread(lImage);
    cv::Mat imageR = cv::imread(rImage);
    LOGD("imageL w %d h %d", imageL.cols, imageL.rows);
    LOGD("imageR w %d h %d", imageR.cols, imageR.rows);

    cv::resize(imageL, imageL, cv::Size(imageL.cols / resizeRatio, imageL.rows / resizeRatio), 0.5, 0.5);
    cv::resize(imageR, imageR, cv::Size(imageR.cols / resizeRatio, imageR.rows / resizeRatio), 0.5, 0.5);
    imageL = imageL(cv::Rect(0, 0, imageL.cols / 2, imageL.rows / 2));
    imageR = imageR(cv::Rect(16, 0, imageR.cols / 2, imageR.rows / 2));
    cv::imshow("imageR ori", imageR);
    cv::Mat grayL;
    cv::cvtColor(imageL, grayL, cv::COLOR_RGB2GRAY);
    cv::Mat grayR;
    cv::cvtColor(imageR, grayR, cv::COLOR_RGB2GRAY);
    cv::imshow("imageL", grayL);
    cv::imshow("imageR", grayR);

    cv::Mat gray32FL;
    cv::Mat gray32FR;
    cv::Mat gradient32FX;
    cv::Mat gradient32FY;
    grayL.convertTo(gray32FL, CV_32FC1);
    grayR.convertTo(gray32FR, CV_32FC1);

    int pyramidLayer = 6;
    vector<Mat> pyramidL(pyramidLayer);
    vector<Mat> pyramidR(pyramidLayer);
    vector<Mat> pyramidLX(pyramidLayer);
    vector<Mat> pyramidLY(pyramidLayer);
    vector<Mat> pyramidLM(pyramidLayer);
    vector<Mat> pyramidRX(pyramidLayer);
    vector<Mat> pyramidRY(pyramidLayer);
    vector<Mat> pyramidRM(pyramidLayer);
    vector<Mat> pyramidDeltaX(pyramidLayer);
    vector<Mat> pyramidDeltaY(pyramidLayer);
    vector<Mat> pyramidDeltaT(pyramidLayer);
    vector<Mat> pyramidOffsetX(pyramidLayer);
    vector<Mat> pyramidOffsetY(pyramidLayer);
    // imageL.convertTo(imageL, CV_32FC3);
    // imageR.convertTo(imageR, CV_32FC3);
    buildPyramid(gray32FL, pyramidL, pyramidLayer);
    buildPyramid(gray32FR, pyramidR, pyramidLayer);
    // buildPyramid(imageL, pyramidL, pyramidLayer);
    // buildPyramid(imageR, pyramidR, pyramidLayer);
    int regionSize = 5;
    int step = 1;
    Mat points[2];
    vector<uchar> status;
    vector<float> err;
    Size subPixWinSize(10, 10), winSize(11, 11);
    grayL.convertTo(gray32FL, CV_8U);
    grayR.convertTo(gray32FR, CV_8U);
    goodFeaturesToTrack(gray32FL, points[0], 100, 0.01, 10);
    Mat pyramidOffsetXCloneFull = pyramidL[0].clone();
    Mat pyramidOffsetYCloneFull = pyramidL[0].clone();
    pyramidOffsetXCloneFull *= 0;
    pyramidOffsetYCloneFull *= 0;
    for (int i = pyramidLayer - 1; i >= 0; i--)
    {
        if (i != (pyramidLayer - 1))
        {
            Mat movedL = Mat::zeros(pyramidL[i].size(), pyramidL[i].type());
            pyrUp(pyramidOffsetX[i + 1], pyramidOffsetX[i], pyramidL[i].size());
            pyrUp(pyramidOffsetY[i + 1], pyramidOffsetY[i], pyramidL[i].size());
        }
        else
        {
            pyramidOffsetX[i] = Mat::zeros(pyramidL[i].size(), CV_32FC1);
            pyramidOffsetY[i] = Mat::zeros(pyramidL[i].size(), CV_32FC1);
        }
        cv::Mat kernelX = (cv::Mat_<float>(3, 3) << -3, 0, 3,
                           -10, 0, 10,
                           -3, 0, 3);
        cv::Mat kernelY = (cv::Mat_<float>(3, 3) << -3, -10, -3,
                           0, 0, 0,
                           3, 10, 3);

        cv::Mat kernelMedian = (cv::Mat_<float>(3, 3) << 0, 0, 0,
                                0, 1, 0,
                                0, 0, 0);
        cv::filter2D(pyramidL[i], pyramidLX[i], pyramidL[i].depth(), kernelX);
        cv::filter2D(pyramidL[i], pyramidLY[i], pyramidL[i].depth(), kernelY);
        cv::filter2D(pyramidL[i], pyramidLM[i], pyramidL[i].depth(), kernelMedian);
        cv::filter2D(pyramidR[i], pyramidRX[i], pyramidR[i].depth(), kernelX);
        cv::filter2D(pyramidR[i], pyramidRY[i], pyramidR[i].depth(), kernelY);
        cv::filter2D(pyramidR[i], pyramidRM[i], pyramidR[i].depth(), kernelMedian);
        pyramidDeltaX[i] = pyramidLX[i] + pyramidRX[i];
        pyramidDeltaY[i] = pyramidLY[i] + pyramidRY[i];
        pyramidDeltaT[i] = pyramidLM[i] - pyramidRM[i];
        LOGD("size %d %d  pyramidDeltaX %d %d", pyramidDeltaT[i].size().width, pyramidDeltaT[i].size().height, pyramidDeltaX[i].size().width, pyramidDeltaX[i].size().height);
        LOGD("size %d %d", pyramidDeltaT[i].cols, pyramidDeltaT[i].rows);
        // regionSize *= 1.4;
        // step *= 2;
        // regionSize += 3;
        step = 15;
        Mat pyramidOffsetXClone = pyramidOffsetX[i].clone();
        Mat pyramidOffsetYClone = pyramidOffsetY[i].clone();
        // pyramidOffsetXClone = pyramidOffsetXClone * 0;
        // pyramidOffsetYClone = pyramidOffsetYClone * 0;
        for (int p = 0; p < points[0].rows; p += 1)
        {
            if (!((points[0].at<Point2f>(p).x < gray32FL.cols - regionSize - 2) && (points[0].at<Point2f>(p).x > regionSize + 1) && (points[0].at<Point2f>(p).y < gray32FL.rows - regionSize - 2) && (points[0].at<Point2f>(p).y > regionSize + 1)))
            {
                continue;
            }
            float r = points[0].at<Point2f>(p).y;
            float c = points[0].at<Point2f>(p).x;
            Mat A = Mat::zeros({2, 2}, CV_32FC1);
            Mat B = Mat::zeros({2, 1}, CV_32FC1);
            A = (cv::Mat_<float>(2, 2) << 0, 0, 0, 0);
            B = (cv::Mat_<float>(2, 1) << 0, 0);

            r = r / pow(2, i);
            c = c / pow(2, i);
            Mat result = (cv::Mat_<float>(2, 1) << 0, 0);
            float resultResidual = 999;
            for (int itrIndex = 0; itrIndex < 50 && resultResidual > 0.0001; itrIndex++) {
                for (int offr = -regionSize; offr <= regionSize; offr++)
                {
                    for (int offc = -regionSize; offc <= regionSize; offc++)
                    {
                        if ((r + offr) >= pyramidOffsetY[i].rows || (c + offc) >= pyramidOffsetY[i].cols || (c + offc) < 0 || (r + offr) < 0)
                        {
                            continue;
                        }
                        float offsetx = pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x);
                        float offsety = pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x);
                        // float newr = (r + offr + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetY[i])) + result.at<float>(1, 0)) > (pyramidOffsetY[i].rows - 2) ? (pyramidOffsetY[i].rows - 2) : (r + offr + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetY[i])) + result.at<float>(1, 0));
                        // newr = newr > 0 ? newr : 0;
                        // float newc = (c + offc + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetX[i])) + result.at<float>(0, 0)) > (pyramidOffsetX[i].cols - 2) ? (pyramidOffsetX[i].cols - 2) : (c + offc + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetX[i])) + result.at<float>(0, 0));
                        // newc = newc > 0 ? newc : 0;
                        float newr = (r + offr + offsety + result.at<float>(1, 0)) > (pyramidOffsetY[i].rows - 2) ? (pyramidOffsetY[i].rows - 2) : (r + offr + offsety + result.at<float>(1, 0));
                        newr = newr > 0 ? newr : 0;
                        float newc = (c + offc + offsetx + result.at<float>(0, 0)) > (pyramidOffsetX[i].cols - 2) ? (pyramidOffsetX[i].cols - 2) : (c + offc + offsetx + result.at<float>(0, 0));
                        newc = newc > 0 ? newc : 0;
                        int p1r = floor(newr), p1c = floor(newc), p2r = floor(newr), p2c = floor(newc) + 1, p3r = floor(newr) + 1, p3c = floor(newc), p4r = floor(newr) + 1, p4c = floor(newc) + 1;
                        int lp1r = floor(r + offr), lp1c = floor(c + offc), lp2r = floor(r + offr), lp2c = floor(c + offc) + 1, lp3r = floor(r + offr) + 1, lp3c = floor(c + offc), lp4r = floor(r + offr) + 1, lp4c = floor(c + offc) + 1;
                        // float deltaX = pyramidLX[i].at<float>(r + offr, c + offc);
                        // float deltaY = pyramidLY[i].at<float>(r + offr, c + offc);
                        // float deltaT = -pyramidRM[i].at<float>(r + offr, c + offc) + pyramidLM[i].at<float>(r + offr, c + offc);
                        float deltaX = Interpolate(Point_<float>(r + offr, c + offc), pyramidLX[i]);
                        float deltaY = Interpolate(Point_<float>(r + offr, c + offc), pyramidLY[i]);
                        float deltaT = Interpolate(Point_<float>(r + offr, c + offc), pyramidLM[i]) - Interpolate(Point_<float>(newr, newc), pyramidRM[i]);
                        A = A + (cv::Mat_<float>(2, 2) << deltaX * deltaX, deltaX * deltaY, deltaX * deltaY, deltaY * deltaY);
                        B = B + (cv::Mat_<float>(2, 1) << deltaX * deltaT, deltaY * deltaT);
                    }
                }
                Mat resultItr = (cv::Mat_<float>(2, 1) << 0, 0);
                float D = A.at<float>(0, 0) * A.at<float>(1, 1) - A.at<float>(1, 0) * A.at<float>(0, 1);
                if (D > 0.1)
                {
                    // solve(A, B, resultItr, DECOMP_LU);
                    solve(A, B, resultItr, cv::DECOMP_CHOLESKY | cv::DECOMP_NORMAL);
                }
                result.at<float>(0, 0) += resultItr.at<float>(0, 0);
                result.at<float>(1, 0) += resultItr.at<float>(1, 0);
                resultResidual = sqrt(pow( resultItr.at<float>(0, 0), 2) + pow( resultItr.at<float>(1, 0), 2));
            }

            if (p < 5 && (fabs(result.at<float>(0, 0)) > 0 || fabs(result.at<float>(1, 0)) > 0))
                LOGD("result %d : %f %f", p, result.at<float>(0, 0), result.at<float>(1, 0));
            for (int offr = -regionSize; offr <= regionSize; offr++)
            {
                for (int offc = -regionSize; offc <= regionSize; offc++)
                {
                    if ((r + offr) >= pyramidOffsetY[i].rows || (c + offc) >= pyramidOffsetY[i].cols || (c + offc) < 0 || (r + offr) < 0)
                    {
                        continue;
                    }
                    pyramidOffsetXClone.at<float>(r + offr, c + offc) = pyramidOffsetX[i].at<float>(r + offr, c + offc) + result.at<float>(0, 0);
                    pyramidOffsetYClone.at<float>(r + offr, c + offc) = pyramidOffsetY[i].at<float>(r + offr, c + offc) + result.at<float>(1, 0);
                    // pyramidOffsetX[i].at<float>(r + offr, c + offc) = pyramidOffsetX[i].at<float>(r + offr, c + offc) + result.at<float>(0, 0);
                    // pyramidOffsetY[i].at<float>(r + offr, c + offc) = pyramidOffsetY[i].at<float>(r + offr, c + offc) + result.at<float>(1, 0);
                }
            }
            pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x) += result.at<float>(0, 0);
            pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x) += result.at<float>(1, 0);
        }
        pyramidOffsetX[i] = pyramidOffsetXClone;
        pyramidOffsetY[i] = pyramidOffsetYClone;
        if (i != 1) {
            pyramidOffsetX[i] = pyramidOffsetX[i] * 2;
            pyramidOffsetY[i] = pyramidOffsetY[i] * 2;
            pyramidOffsetXCloneFull *= 2;
            pyramidOffsetYCloneFull *= 2;
        }

        LOGD("print Process");
        cv::imshow("opticalFlow" + to_string(i), pyramidOffsetX[i]);
        pyramidL[i].convertTo(pyramidL[i], CV_8U);
        cv::imshow("pyramidlFlow" + to_string(i), pyramidL[i]);
    }

    LOGD("points[0].rows %d", points[0].rows);
    Mat resultME = imageL.clone();
    for (int r = 0; r < points[0].rows; r += 1)
    {
        int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetXCloneFull.at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetYCloneFull.at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        // int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        // int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        dstx = dstx > (imageL.cols - 5) ? (imageL.cols - 5) : dstx;
        dstx = dstx > 5 ? dstx : 5;
        dsty = dsty > (imageL.rows - 5) ? (imageL.rows - 5) : dsty;
        dsty = dsty > 5 ? dsty : 5;
        if (fabs(pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x)) > 5 || fabs(pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x)) > 5)
        {
            LOGD("points[0].rows %d %d", dstx, dsty);
            arrowedLine(resultME, {(int)points[0].at<Point2f>(r).x, (int)points[0].at<Point2f>(r).y}, {dstx, dsty}, Scalar(255, 0, 0), 1);
        }
    }
    // cv::log(pyramidOffsetX[0], pyramidOffsetX[0]);
    // cv::normalize(pyramidOffsetX[0], pyramidOffsetX[0], 0, 255, cv::NORM_MINMAX);
    // cv::imshow("opticalFlow", pyramidOffsetX[0]);
    // imwrite("opticalFlow.jpg", pyramidOffsetX[0]); // Feature point matching relationship diagram between the two images
    cv::imshow("imageLMe", resultME);
    imwrite("imageLMe.jpg", resultME); // Feature point matching relationship diagram between the two images
    // Mat points[2];
    // vector<uchar> status;
    // vector<float> err;
    // Size subPixWinSize(10, 10), winSize(31, 31);
    Mat resultLK = imageL.clone();
    grayL.convertTo(gray32FL, CV_8U);
    grayR.convertTo(gray32FR, CV_8U);
    // goodFeaturesToTrack(gray32FL, points[0], 100, 0.01, 10);
    calcOpticalFlowPyrLK(gray32FL, gray32FR, points[0], points[1], status, err, winSize, 6);
    for (int r = 0; r < points[0].rows; r += 1)
    {
        if (status[r] && (fabs(points[0].at<Point2f>(r).x - points[1].at<Point2f>(r).x) > 10 || fabs(points[0].at<Point2f>(r).y - points[1].at<Point2f>(r).y) > 10))
        {
            arrowedLine(resultLK, {(int)points[0].at<Point2f>(r).x, (int)points[0].at<Point2f>(r).y}, {(int)points[1].at<Point2f>(r).x, (int)points[1].at<Point2f>(r).y}, Scalar(255, 0, 0), 1);
            // LOGD("LK dstx, dsty %d, %d", (int)points[1].at<Point2f>(r).x, (int)points[1].at<Point2f>(r).y);
            int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
            int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
            dstx = dstx > (imageL.cols - 5) ? (imageL.cols - 5) : dstx;
            dstx = dstx > 5 ? dstx : 5;
            dsty = dsty > (imageL.rows - 5) ? (imageL.rows - 5) : dsty;
            dsty = dsty > 5 ? dsty : 5;
            // LOGD("dstx, dsty %d, %d", dstx, dsty);
        }
    }
    cv::imshow("opticalFlowLK", resultLK);
    imwrite("opticalFlowLK.jpg", resultLK); // Feature point matching relationship diagram between the two images
    UMat pre, next, Flow;
    Mat flow, cflow;
    cv::cvtColor(imageL, pre, cv::COLOR_RGB2GRAY);
    cv::cvtColor(imageR, next, cv::COLOR_RGB2GRAY);
    // // calcOpticalFlowFarneback(grayL, grayR, Flow, 0.5, 6, 5, 6, 5, 1.5, OPTFLOW_FARNEBACK_GAUSSIAN);
    calcOpticalFlowFarneback(pre, next, Flow, 0.5, 16, 30, 6, 7, 1.5, 0);
    cvtColor(pre, cflow, COLOR_GRAY2BGR);
    Flow.copyTo(flow);
    drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(0, 255, 0));
    imshow("flow", cflow);
    imwrite("flow.jpg", cflow); // Feature point matching relationship diagram between the two images


    Mat resultLKME = imageL.clone();
    LKTracker lktracker;
    auto trackedPoint = lktracker.track(gray32FL, gray32FR, points[0]);
    for (int r = 0; r < trackedPoint.size(); r += 1)
    {
        int dstx = (int)trackedPoint[r].point.x;
        int dsty = (int)trackedPoint[r].point.y;
        // int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        // int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        dstx = dstx > (imageL.cols - 5) ? (imageL.cols - 5) : dstx;
        dstx = dstx > 5 ? dstx : 5;
        dsty = dsty > (imageL.rows - 5) ? (imageL.rows - 5) : dsty;
        dsty = dsty > 5 ? dsty : 5;
        // if (fabs(pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x)) > 5 || fabs(pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x)) > 5)
        {
            LOGD("points[0].rows %d %d", dstx, dsty);
            arrowedLine(resultLKME, {(int)points[0].at<Point2f>(r).x, (int)points[0].at<Point2f>(r).y}, {dstx, dsty}, Scalar(255, 0, 0), 1);
        }
    }
    cv::imshow("imageLLKMe", resultLKME);
    imwrite("imageLLKMe.jpg", resultLKME); // Feature point matching relationship diagram between the two images
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}


