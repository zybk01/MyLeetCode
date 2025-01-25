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
    int regionSize = 1;
    int step = 1;
    Mat points[2];
    vector<uchar> status;
    vector<float> err;
    Size subPixWinSize(10, 10), winSize(5, 5);
    grayL.convertTo(gray32FL, CV_8U);
    grayR.convertTo(gray32FR, CV_8U);
    goodFeaturesToTrack(gray32FL, points[0], 30, 0.01, 10);
    Mat pyramidOffsetXCloneFull = pyramidL[0].clone();
    Mat pyramidOffsetYCloneFull = pyramidL[0].clone();
    pyramidOffsetXCloneFull *= 0;
    pyramidOffsetYCloneFull *= 0;
    Mat interpolateMat = (cv::Mat_<float>(2, 2) << 1, 2, 3, 4);

    float interpolateValue = Interpolate(Point_<float>(0, 0), interpolateMat);
    LOGD("delta : %f" , interpolateValue);
    interpolateValue = Interpolate(Point_<float>(0.5, 0.5), interpolateMat);
    LOGD("delta : %f" , interpolateValue);
    interpolateValue = Interpolate(Point_<float>(0, 0.5), interpolateMat);
    LOGD("delta : %f" , interpolateValue);
    interpolateValue = Interpolate(Point_<float>(0.5, 0.0), interpolateMat);
    LOGD("delta : %f" , interpolateValue);
    for (int i = pyramidLayer - 1; i >= 0; i--)
    {
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

        // normalization
        pyramidLX[i] = pyramidLX[i] / 32;
        pyramidLY[i] = pyramidLY[i] / 32;
        pyramidRX[i] = pyramidRX[i] / 32;
        pyramidRY[i] = pyramidRY[i] / 32;
        LOGD("size %d %d", pyramidLX[i].cols, pyramidLX[i].rows);
        string imageNmae;
        imageNmae = "pyramidL" + to_string(i) + ".jpg";
        imwrite(imageNmae, pyramidL[i]);
        imageNmae = "pyramidR" + to_string(i) + ".jpg";
        imwrite(imageNmae, pyramidR[i]);
        imageNmae = "pyramidLX" + to_string(i) + ".jpg";
        imwrite(imageNmae, pyramidLX[i]);
        imageNmae = "pyramidRX" + to_string(i) + ".jpg";
        imwrite(imageNmae, pyramidRX[i]);
 
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

            r = r / pow(2, i);
            c = c / pow(2, i);
            Mat result = (cv::Mat_<float>(2, 1) << 0, 0);
            Mat preresult = (cv::Mat_<float>(2, 1) << 0, 0);
            float resultResidual = 999;
            for (int itrIndex = 0; itrIndex < 1 && resultResidual > 0.2; itrIndex++) {
                A = (cv::Mat_<float>(2, 2) << 0, 0, 0, 0);
                B = (cv::Mat_<float>(2, 1) << 0, 0);
                for (int offr = -regionSize; offr <= regionSize; offr++)
                {
                    for (int offc = -regionSize; offc <= regionSize; offc++)
                    {
                        if ((r + offr) >= pyramidLX[i].rows || (c + offc) >= pyramidLX[i].cols || (c + offc) < 0 || (r + offr) < 0)
                        {
                            continue;
                        }
                        float offsetx = pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x);
                        float offsety = pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x);
                        float newr = (r + offr + offsety + result.at<float>(1, 0)) > (pyramidLX[i].rows - 2) ? (pyramidLX[i].rows - 2) : (r + offr + offsety + result.at<float>(1, 0));
                        newr = newr > 0 ? newr : 0;
                        float newc = (c + offc + offsetx + result.at<float>(0, 0)) > (pyramidLX[i].cols - 2) ? (pyramidLX[i].cols - 2) : (c + offc + offsetx + result.at<float>(0, 0));
                        newc = newc > 0 ? newc : 0;
                        float deltaX = Interpolate(Point_<float>(c + offc, r + offr), pyramidLX[i]);
                        float deltaY = Interpolate(Point_<float>(c + offc, r + offr), pyramidLY[i]);
                        float deltaT = Interpolate(Point_<float>(c + offc, r + offr), pyramidLM[i]) - Interpolate(Point_<float>(newc, newr), pyramidRM[i]);
                        A = A + (cv::Mat_<float>(2, 2) << deltaX * deltaX, deltaX * deltaY, deltaX * deltaY, deltaY * deltaY);
                        B = B + (cv::Mat_<float>(2, 1) << deltaX * deltaT, deltaY * deltaT);
                        if (offr == 0 && offc == 0 && itrIndex < 3)
                            LOGD("delta : %f %f T %f l:%f %f R:%f %f", deltaX, deltaY, deltaT, r + offr, c + offc, newr, newc);
                    }
                }
                Mat resultItr = (cv::Mat_<float>(2, 1) << 0, 0);
                float D = A.at<float>(0, 0) * A.at<float>(1, 1) - A.at<float>(1, 0) * A.at<float>(0, 1);
                float minEig = (A.at<float>(0, 0) + A.at<float>(1, 1) - std::sqrt((A.at<float>(0, 0) - A.at<float>(1, 1))*(A.at<float>(0, 0) - A.at<float>(1, 1)) +
                        4.f*(A.at<float>(1, 0) * A.at<float>(0, 1)))) / (2*regionSize * regionSize);
                if (D > 0.00001 && minEig > 0.001)
                {
                    // solve(A, B, resultItr, DECOMP_LU);
                    solve(A, B, resultItr, cv::DECOMP_CHOLESKY | cv::DECOMP_NORMAL);
                }
                result.at<float>(0, 0) += resultItr.at<float>(0, 0);
                result.at<float>(1, 0) += resultItr.at<float>(1, 0);
                resultResidual = sqrt(pow( resultItr.at<float>(0, 0), 2) + pow( resultItr.at<float>(1, 0), 2));
                if( itrIndex > 0 && std::abs(resultItr.at<float>(0, 0) + preresult.at<float>(0, 0)) < 0.01 &&
                    std::abs(resultItr.at<float>(1, 0) + preresult.at<float>(1, 0)) < 0.01 )
                {
                    result.at<float>(0, 0) -= resultItr.at<float>(0, 0)*0.5f;
                    result.at<float>(1, 0) -= resultItr.at<float>(1, 0)*0.5f;
                    break;
                }
                preresult = resultItr;
                if (itrIndex < 3 && (fabs(result.at<float>(0, 0)) > 0 || fabs(result.at<float>(1, 0)) > 0))
                    LOGD("result %d : %f %f D %f minEig %f", itrIndex, result.at<float>(0, 0), result.at<float>(1, 0), D, minEig);
            }

            pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x) += result.at<float>(0, 0);
            pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x) += result.at<float>(1, 0);
            if (p < 3 && (fabs(result.at<float>(0, 0)) > 0 || fabs(result.at<float>(1, 0)) > 0))
            {
                LOGD("result level %d : %f %f", p, result.at<float>(0, 0), result.at<float>(1, 0));
                LOGD("result total %d : %f %f", p, pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x), pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(p).y, points[0].at<Point2f>(p).x));
            }

    
        }
        if (i != 1) {
            pyramidOffsetXCloneFull *= 2;
            pyramidOffsetYCloneFull *= 2;
        }

        // cv::imshow("opticalFlow" + to_string(i), pyramidOffsetXCloneFull);
    }

    Mat resultME = imageL.clone();
    for (int r = 0; r < points[0].rows; r += 1)
    {
        float offsetx = pyramidOffsetXCloneFull.at<float>(points[0].at<Point2f>(r).y, points[0].at<Point2f>(r).x);
        float offsety = pyramidOffsetYCloneFull.at<float>(points[0].at<Point2f>(r).y, points[0].at<Point2f>(r).x);
        int dstx = (int)points[0].at<Point2f>(r).x + (int)pyramidOffsetXCloneFull.at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        int dsty = (int)points[0].at<Point2f>(r).y + (int)pyramidOffsetYCloneFull.at<float>((int)points[0].at<Point2f>(r).y, (int)points[0].at<Point2f>(r).x);
        dstx = dstx > (imageL.cols - 5) ? (imageL.cols - 5) : dstx;
        dstx = dstx > 5 ? dstx : 5;
        dsty = dsty > (imageL.rows - 5) ? (imageL.rows - 5) : dsty;
        dsty = dsty > 5 ? dsty : 5;
        if (fabs(offsetx) > 5 || fabs(offsety) > 5)
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

    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}


