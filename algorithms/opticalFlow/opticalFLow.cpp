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

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/sift/phone3.jpg");
    string rImage("resources/sift/phone4.jpg");
    if (num == 1)
    {
        LOGD("-l <image input path 1> -r <image input path 2> defaultPath lImage.jpg rImage.jpg");
    }
    int threashold = 10;
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
            }
        }
    }
    cv::Mat imageL = cv::imread(lImage);
    cv::Mat imageR = cv::imread(rImage);
    LOGD("imageL w %d h %d", imageL.cols, imageL.rows);
    LOGD("imageR w %d h %d", imageR.cols, imageR.rows);
    float resizeRatio = 4;
    cv::resize(imageL, imageL, cv::Size(imageL.cols / resizeRatio, imageL.rows / resizeRatio), 0.5, 0.5);
    cv::resize(imageR, imageR, cv::Size(imageR.cols / resizeRatio, imageR.rows / resizeRatio), 0.5, 0.5);
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

    int pyramidLayer = 8;
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
    for (int i = pyramidLayer - 1; i >= 0; i--)
    {
        if (i != (pyramidLayer - 1))
        {
            Mat movedL = Mat::zeros(pyramidL[i].size(), pyramidL[i].type());
            ;
            pyrUp(pyramidOffsetX[i + 1], pyramidOffsetX[i], pyramidL[i].size());
            pyrUp(pyramidOffsetY[i + 1], pyramidOffsetY[i], pyramidL[i].size());
            for (int r = 0; r < pyramidOffsetX[i].rows - 1; r += 1)
            {
                for (int c = 0; c < pyramidOffsetX[i].cols - 1; c += 1)
                {
                    // pyramidOffsetX[i].at<float(r, c)>
                    // pyramidOffsetY[i].at<float(r, c)>
                    int newr = (r + (int)(pyramidOffsetY[i].at<float>(r, c))) > (pyramidOffsetY[i].rows - 2) ? (pyramidOffsetY[i].rows - 2) : (r + (int)(pyramidOffsetY[i].at<float>(r, c)));
                    newr = newr > 0 ? newr : 0;
                    int newc = (c + (int)(pyramidOffsetX[i].at<float>(r, c))) > (pyramidOffsetX[i].cols - 2) ? (pyramidOffsetX[i].cols - 2) : (c + (int)(pyramidOffsetX[i].at<float>(r, c)));
                    newc = newc > 0 ? newc : 0;
                    movedL.at<float>(newr, newc) = pyramidL[i].at<float>(r, c);
                }
            }
            pyramidL[i] = movedL.clone();
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
        cv::Mat kernelMedian = (cv::Mat_<float>(3, 3) << 1 / 9, 1 / 9, 1 / 9,
                                1 / 9, 1 / 9, 1 / 9,
                                1 / 9, 1 / 9, 1 / 9);
        // cv::Mat kernelMedian = (cv::Mat_<float>(3, 3) << 1,1,1,
        //                         1,1,1,
        //                         1,1,1);
        // cv::Mat kernelX = (cv::Mat_<float>(3, 3) << 0, 0, 0,
        //                    0, -1, 1,
        //                    0, -1, 1);
        // cv::Mat kernelY = (cv::Mat_<float>(3, 3) << 0, 0, 0,
        //                    0, -1, -1,
        //                    0, 1, 1);
        // cv::Mat kernelMedian = (cv::Mat_<float>(3, 3) << 0, 0, 0,
        //                         0, 1 / 4, 1 / 4,
        //                         0, 1 / 4, 1 / 4);
        cv::filter2D(pyramidL[i], pyramidLX[i], pyramidL[i].depth(), kernelX);
        cv::filter2D(pyramidL[i], pyramidLY[i], pyramidL[i].depth(), kernelY);
        cv::filter2D(pyramidL[i], pyramidLM[i], pyramidL[i].depth(), kernelMedian);
        cv::filter2D(pyramidR[i], pyramidRX[i], pyramidR[i].depth(), kernelX);
        cv::filter2D(pyramidR[i], pyramidRY[i], pyramidR[i].depth(), kernelY);
        cv::filter2D(pyramidR[i], pyramidRM[i], pyramidR[i].depth(), kernelMedian);
        pyramidDeltaX[i] = pyramidLX[i] + pyramidRX[i];
        pyramidDeltaY[i] = pyramidLY[i] + pyramidRY[i];
        pyramidDeltaT[i] = pyramidLM[i] - pyramidRM[i];
        // pyramidOffsetX[i] = pyramidDeltaT[i].clone();
        // pyramidOffsetY[i] = pyramidDeltaT[i].clone();
        LOGD("size %d %d  pyramidDeltaX %d %d", pyramidDeltaT[i].size().width, pyramidDeltaT[i].size().height, pyramidDeltaX[i].size().width, pyramidDeltaX[i].size().height);
        LOGD("size %d %d", pyramidDeltaT[i].cols, pyramidDeltaT[i].rows);
        int regionSize = 10;
        for (int r = regionSize + 1; r < pyramidDeltaX[i].rows - regionSize - 2; r += 1)
        {
            for (int c = regionSize + 1; c < pyramidDeltaX[i].cols - regionSize -2; c += 1)
            {
                // Mat A = (cv::Mat_<float>(4, 2) << pyramidDeltaX[i].at<float>(r, c), pyramidDeltaY[i].at<float>(r, c),
                //          pyramidDeltaX[i].at<float>(r, c + 1), pyramidDeltaY[i].at<float>(r, c + 1),
                //          pyramidDeltaX[i].at<float>(r + 1, c), pyramidDeltaY[i].at<float>(r + 1, c),
                //          pyramidDeltaX[i].at<float>(r + 1, c + 1), pyramidDeltaY[i].at<float>(r + 1, c + 1));
                // Mat B = (cv::Mat_<float>(4, 1) << pyramidDeltaT[i].at<float>(r, c),
                //          pyramidDeltaT[i].at<float>(r, c + 1),
                //          pyramidDeltaT[i].at<float>(r + 1, c),
                //          pyramidDeltaT[i].at<float>(r + 1, c + 1));
                // Mat A = (cv::Mat_<float>(2, 2) << pyramidDeltaX[i].at<float>(r, c), pyramidDeltaY[i].at<float>(r, c),
                //          pyramidDeltaX[i].at<float>(r, c + 1), pyramidDeltaY[i].at<float>(r, c + 1));
                // Mat B =(cv::Mat_<float>(2, 1) << pyramidDeltaT[i].at<float>(r, c),
                //          pyramidDeltaT[i].at<float>(r, c + 1));
                // LOGD("%d,%d", r + 1, c + 1);
                Mat A = Mat::zeros({2, 2}, CV_32FC1);
                Mat B = Mat::zeros({2, 1}, CV_32FC1);
                A = (cv::Mat_<float>(2, 2) << 0, 0, 0, 0);
                B = (cv::Mat_<float>(2, 1) << 0, 0);
                
                for (int offr = -regionSize; offr <= regionSize; offr++)
                {
                    for (int offc = -regionSize; offc <= regionSize; offc++)
                    {
                        A = A + (cv::Mat_<float>(2, 2) << pyramidDeltaX[i].at<float>(r + offr, c + offc) * pyramidDeltaX[i].at<float>(r + offr, c + offc), pyramidDeltaX[i].at<float>(r + offr, c + offc) * pyramidDeltaY[i].at<float>(r + offr, c + offc),
                                 pyramidDeltaX[i].at<float>(r + offr, c + offc) * pyramidDeltaY[i].at<float>(r + offr, c + offc), pyramidDeltaY[i].at<float>(r + offr, c + offc) * pyramidDeltaY[i].at<float>(r + offr, c + offc));
                        B = B + (cv::Mat_<float>(2, 1) << pyramidDeltaX[i].at<float>(r + offr, c + offc) * pyramidDeltaT[i].at<float>(r, c),
                                 pyramidDeltaY[i].at<float>(r + offr, c + offc) * pyramidDeltaT[i].at<float>(r, c + 1));
                    }
                }
                Mat result = (cv::Mat_<float>(2, 1) << 0, 0);

                solve(A, B, result, DECOMP_QR);
                // print(format(A, Formatter::FormatType::FMT_DEFAULT));
                // print(format(B, Formatter::FormatType::FMT_DEFAULT));
                if (fabs(result.at<float>(0, 0)) > 10 || fabs(result.at<float>(1, 0)) > 10)
                    LOGD("result %f %f", result.at<float>(0, 0), result.at<float>(1, 0));
                pyramidOffsetX[i].at<float>(r, c) = pyramidOffsetX[i].at<float>(r, c) + result.at<float>(0, 0);
                pyramidOffsetY[i].at<float>(r, c) = pyramidOffsetY[i].at<float>(r, c) + result.at<float>(1, 0);
            }
        }
        pyramidOffsetX[i] = pyramidOffsetX[i] * 2;
        pyramidOffsetY[i] = pyramidOffsetY[i] * 2;
        LOGD("print Process");
        cv::imshow("opticalFlow" + to_string(i), pyramidOffsetX[i]);
    }
    pyramidOffsetX[0] = pyramidOffsetX[0] / 2;
    pyramidOffsetY[0] = pyramidOffsetY[0] / 2;
    for (int r = 0; r < pyramidDeltaX[0].rows - 2; r += 1)
    {
        for (int c = 0; c < pyramidDeltaX[0].cols - 2; c += 1)
        {
            if (fabs(pyramidOffsetX[0].at<float>(r, c)) > 10 || fabs(pyramidOffsetY[0].at<float>(r, c)) > 10)
                arrowedLine(imageL, {r, c}, {(int)(r + pyramidOffsetX[0].at<float>(r, c)), (int)(c + pyramidOffsetY[0].at<float>(r, c))}, Scalar(255, 0, 0), 1);
        }
    }
    cv::log(pyramidOffsetX[0], pyramidOffsetX[0]);
    cv::normalize(pyramidOffsetX[0], pyramidOffsetX[0], 0, 255, cv::NORM_MINMAX);
    cv::imshow("opticalFlow", pyramidOffsetX[0]);
    imwrite("opticalFlow.jpg", pyramidOffsetX[0]); // Feature point matching relationship diagram between the two images
    cv::imshow("imageL", imageL);
    imwrite("imageL.jpg", imageL); // Feature point matching relationship diagram between the two images
    // Mat points[2];
    // vector<uchar> status;
    // vector<float> err;
    // Size subPixWinSize(10, 10), winSize(31, 31);
    // grayL.convertTo(gray32FL, CV_8U);
    // grayR.convertTo(gray32FR, CV_8U);
    // goodFeaturesToTrack(gray32FL, points[0], 100, 0.01, 10);
    // calcOpticalFlowPyrLK(gray32FL, gray32FR, points[0], points[1], status, err, winSize, 6);
    // for (int r = 0; r < points[0].rows; r += 1)
    // {
    //     if (status[r] && (fabs(points[0].at<Point2f>(r).x - points[1].at<Point2f>(r).x) > 10 || fabs(points[0].at<Point2f>(r).y - points[1].at<Point2f>(r).y) > 10))
    //         arrowedLine(imageL, {(int)points[0].at<Point2f>(r).x, (int)points[0].at<Point2f>(r).y}, {(int)points[1].at<Point2f>(r).x, (int)points[1].at<Point2f>(r).y}, Scalar(255, 0, 0), 1);
    // }
    // cv::imshow("opticalFlowLK", imageL);
    // imwrite("opticalFlowLK.jpg", imageL); // Feature point matching relationship diagram between the two images
    // UMat pre, next, Flow;
    // Mat flow, cflow;
    // cv::cvtColor(imageL, pre, cv::COLOR_RGB2GRAY);
    // cv::cvtColor(imageR, next, cv::COLOR_RGB2GRAY);
    // // calcOpticalFlowFarneback(grayL, grayR, Flow, 0.5, 6, 5, 6, 5, 1.5, OPTFLOW_FARNEBACK_GAUSSIAN);
    // calcOpticalFlowFarneback(pre, next, Flow, 0.5, 6, 31, 6, 5, 1.2, 0);
    // cvtColor(pre, cflow, COLOR_GRAY2BGR);
    // Flow.copyTo(flow);
    // drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(0, 255, 0));
    // imshow("flow", cflow);
    // imwrite("flow.jpg", cflow); // Feature point matching relationship diagram between the two images
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}