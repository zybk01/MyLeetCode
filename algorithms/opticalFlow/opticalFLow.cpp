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
    imageR = imageR(cv::Rect(10, 10, imageR.cols / 2, imageR.rows / 2));
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

    int pyramidLayer = 5;
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
    Size subPixWinSize(10, 10), winSize(31, 31);
    grayL.convertTo(gray32FL, CV_8U);
    grayR.convertTo(gray32FR, CV_8U);
    goodFeaturesToTrack(gray32FL, points[0], 100, 0.01, 10);
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
        cv::Mat kernelX = (cv::Mat_<float>(3, 3) << -1, 0, 1,
                           -2, 0, 2,
                           -1, 0, 1);
        cv::Mat kernelY = (cv::Mat_<float>(3, 3) << -1, -2, -1,
                           0, 0, 0,
                           1, 2, 1);
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
        // regionSize *= 2;
        // step *= 2;
        regionSize = 10;
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
            for (int offr = -regionSize; offr <= regionSize; offr++)
            {
                for (int offc = -regionSize; offc <= regionSize; offc++)
                {
                    if ((r + offr) >= pyramidOffsetY[i].rows || (c + offc) >= pyramidOffsetY[i].cols || (c + offc) < 0 || (r + offr) < 0)
                    {
                        continue;
                    }

                    float newr = (r + offr + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetY[i]))) > (pyramidOffsetY[i].rows - 2) ? (pyramidOffsetY[i].rows - 2) : (r + offr + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetY[i])));
                    newr = newr > 0 ? newr : 0;
                    float newc = (c + offc + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetX[i]))) > (pyramidOffsetX[i].cols - 2) ? (pyramidOffsetX[i].cols - 2) : (c + offc + (Interpolate(Point_<float>(r + offr, c + offc), pyramidOffsetX[i])));
                    newc = newc > 0 ? newc : 0;
                    int p1r = floor(newr), p1c = floor(newc), p2r = floor(newr), p2c = floor(newc) + 1, p3r = floor(newr) + 1, p3c = floor(newc), p4r = floor(newr) + 1, p4c = floor(newc) + 1;
                    int lp1r = floor(r + offr), lp1c = floor(c + offc), lp2r = floor(r + offr), lp2c = floor(c + offc) + 1, lp3r = floor(r + offr) + 1, lp3c = floor(c + offc), lp4r = floor(r + offr) + 1, lp4c = floor(c + offc) + 1;
                    // float deltaX = pyramidLX[i].at<float>(r + offr, c + offc);
                    // float deltaY = pyramidLY[i].at<float>(r + offr, c + offc);
                    float deltaX = Interpolate(Point_<float>(r + offr, c + offc), pyramidLX[i]);
                    float deltaY = Interpolate(Point_<float>(r + offr, c + offc), pyramidLY[i]);
                    float deltaT = Interpolate(Point_<float>(r + offr, c + offc), pyramidLM[i]) - Interpolate(Point_<float>(newr, newc), pyramidRM[i]);         
                    // deltaT = -pyramidRM[i].at<float>(r + offr, c + offc) + pyramidLM[i].at<float>(r + offr, c + offc);
                    A = A + (cv::Mat_<float>(2, 2) << deltaX * deltaX, deltaX * deltaY, deltaX * deltaY, deltaY * deltaY);
                    B = B + (cv::Mat_<float>(2, 1) << deltaX * deltaT, deltaY * deltaT);
                }
            }
            Mat result = (cv::Mat_<float>(2, 1) << 0, 0);
            float D = A.at<float>(0, 0) * A.at<float>(1, 1) - A.at<float>(1, 0) * A.at<float>(0, 1);
            if (D > 0.000001)
            {
                solve(A, B, result, DECOMP_QR);
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
        }
        pyramidOffsetX[i] = pyramidOffsetXClone * 2;
        pyramidOffsetY[i] = pyramidOffsetYClone * 2;
        // pyramidOffsetX[i] = pyramidOffsetX[i] * 2;
        // pyramidOffsetY[i] = pyramidOffsetY[i] * 2;

        LOGD("print Process");
        cv::imshow("opticalFlow" + to_string(i), pyramidOffsetX[i]);
        pyramidL[i].convertTo(pyramidL[i], CV_8U);
        cv::imshow("pyramidlFlow" + to_string(i), pyramidL[i]);
    }
    pyramidOffsetX[0] = pyramidOffsetX[0] * 2;
    pyramidOffsetY[0] = pyramidOffsetY[0] * 2;

    LOGD("points[0].rows %d", points[0].rows);
    Mat resultME = imageL.clone();
    for (int r = 0; r < points[0].rows; r += 1)
    {
        int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetX[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetY[0].at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
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
    grayL.convertTo(gray32FL, CV_8U);
    grayR.convertTo(gray32FR, CV_8U);
    // goodFeaturesToTrack(gray32FL, points[0], 100, 0.01, 10);
    calcOpticalFlowPyrLK(gray32FL, gray32FR, points[0], points[1], status, err, winSize, 6);
    for (int r = 0; r < points[0].rows; r += 1)
    {
        if (status[r] && (fabs(points[0].at<Point2f>(r).x - points[1].at<Point2f>(r).x) > 10 || fabs(points[0].at<Point2f>(r).y - points[1].at<Point2f>(r).y) > 10))
        {
            arrowedLine(imageL, {(int)points[0].at<Point2f>(r).x, (int)points[0].at<Point2f>(r).y}, {(int)points[1].at<Point2f>(r).x, (int)points[1].at<Point2f>(r).y}, Scalar(255, 0, 0), 1);
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
    cv::imshow("opticalFlowLK", imageL);
    imwrite("opticalFlowLK.jpg", imageL); // Feature point matching relationship diagram between the two images
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
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}