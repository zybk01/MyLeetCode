#include "ThreadPool.h"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/types.hpp"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <fstream>
#include <iostream>
#include <variant>
#include <windows.h>

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

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/sift/sift1.jpg");
    string rImage("resources/sift/sift2.jpg");
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
                    snprintf(input, sizeof(input), "%s", args[i]);
                    lImage = string(input);
                }
                else if (strcmp(args[i], "-r") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i]);
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
    int resizeRatio = 8;
    cv::resize(imageL, imageL, cv::Size(imageL.cols / resizeRatio, imageL.rows / resizeRatio));
    cv::resize(imageR, imageR, cv::Size(imageR.cols / resizeRatio, imageR.rows / resizeRatio));

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
    double sigma = 0.7;
    int kernel_size = int(sigma * 6) | 1;
    int laplace_kernel_size = 5;

#define bolbNum 10

    cv::Mat kernelGaussian = cv::getGaussianKernel(kernel_size, sigma, CV_32FC1);
    cv::Mat kernelAverage = cv::Mat::ones(kernel_size, kernel_size, CV_32FC1);
    cv::Mat LogResponseL[bolbNum + 1];
    cv::Mat LogResponseR[bolbNum + 1];
    cv::Mat averageL[bolbNum];
    cv::Mat averageR[bolbNum];
    cv::Mat resultL = cv::Mat::ones(gray32FL.rows, gray32FL.cols, CV_8U) * 3;
    cv::Mat resultR = cv::Mat::ones(gray32FL.rows, gray32FL.cols, CV_8U) * 3;
    LogResponseL[bolbNum] = cv::Mat::zeros(gray32FL.rows, gray32FL.cols, CV_32FC1);
    LogResponseR[bolbNum] = cv::Mat::zeros(gray32FL.rows, gray32FL.cols, CV_32FC1);

    std::vector<siftDP> siftDpVecL;
    std::map<cv::Point, siftDP, siftDP> siftDpMapL;
    std::vector<siftDP> siftDpVecR;
    std::map<cv::Point, siftDP, siftDP> siftDpMapR;
    cv::Mat lapceL;
    cv::Mat lapceR;
    for (int i = 0; i < bolbNum; i++)
    {
        kernelGaussian = cv::getGaussianKernel(kernel_size, sigma, CV_32FC1);

        cv::GaussianBlur(gray32FL, LogResponseL[i], cv::Size(kernel_size, kernel_size), sigma, sigma);
        cv::Laplacian(LogResponseL[i], LogResponseL[i], LogResponseL[i].depth(), laplace_kernel_size);
        cv::filter2D(gray32FL, averageL[i], gray32FL.depth(), kernelAverage);
        cv::divide(LogResponseL[i], averageL[i], LogResponseL[i], sigma * sigma, CV_32F);
        LogResponseL[i] = abs(LogResponseL[i] * 1);

        cv::GaussianBlur(gray32FR, LogResponseR[i], cv::Size(kernel_size, kernel_size), sigma, sigma);
        cv::Laplacian(LogResponseR[i], LogResponseR[i], gray32FR.depth(), laplace_kernel_size);
        cv::filter2D(gray32FR, averageR[i], gray32FR.depth(), kernelAverage);
        cv::divide(LogResponseR[i], averageR[i], LogResponseR[i], sigma * sigma, CV_32F);
        LogResponseR[i] = abs(LogResponseR[i] * 1);

        cv::Point maxIdxL, maxIdxR;
        cv::minMaxLoc(LogResponseL[i], nullptr, nullptr, nullptr, &maxIdxL);
        cv::minMaxLoc(LogResponseR[i], nullptr, nullptr, nullptr, &maxIdxR);
        float maxLoDL = LogResponseL[i].at<float>(maxIdxL.y, maxIdxL.x);
        float maxLoDR = LogResponseR[i].at<float>(maxIdxR.y, maxIdxR.x);
        LOGD("maxLoDL %f maxLoDR %f", maxLoDL, maxLoDR);
        // maxLoDL = 120000;
        // maxLoDR = 120000;

        int window = 26;
        float threshold = 0.1;
        cv::Mat gray32FHeeeianXL;
        cv::Mat gray32FHeeeianYL;
        cv::Mat gray32FHeeeianXYL;
        cv::Sobel(gray32FL,gray32FHeeeianXL,gray32FL.depth(),2,0);
        cv::Sobel(gray32FL,gray32FHeeeianYL,gray32FL.depth(),0,2);
        cv::Sobel(gray32FL,gray32FHeeeianXYL,gray32FL.depth(),1,1);
        
        for (int r = 0; r < gray32FL.rows; r += 1)
        {
            for (int c = 0; c < gray32FL.cols; c += 1)
            {
                cv::Point minIdx, maxIdx;
                double max;
                if (LogResponseL[i].at<float>(r, c) > maxLoDL * threshold)
                {
                    cv::minMaxLoc(LogResponseL[i](cv::Range((r - window) < 0 ? 0 : (r - window), r + window < gray32FL.rows ? (r + window) : (gray32FL.rows - 1)), cv::Range((c - window) < 0 ? 0 : (c - window), (c + window) < gray32FL.cols ? (c + window) : (gray32FL.cols - 1))), nullptr, &max, nullptr, &maxIdx);
                    cv::Mat hessian = (cv::Mat_<double>(2, 2) << gray32FHeeeianXL.at<float>(r, c), gray32FHeeeianXYL.at<float>(r, c), gray32FHeeeianXYL.at<float>(r, c), gray32FHeeeianYL.at<float>(r, c));
                    double det = gray32FHeeeianXL.at<float>(r, c) +  gray32FHeeeianYL.at<float>(r, c) - gray32FHeeeianXYL.at<float>(r, c) * gray32FHeeeianXYL.at<float>(r, c);
                    double trace =  gray32FHeeeianXL.at<float>(r, c) +  gray32FHeeeianYL.at<float>(r, c);
                    double ratio = trace * trace / det;
                    if (maxIdx.x == window && maxIdx.y == window && ratio < 10) {
                        {
                            siftDP sift = {
                                .pos = {c, r},
                                .sigma = sigma,
                                .loD = max,
                            };
                            if (siftDpMapL.count(sift.pos) && siftDpMapL[sift.pos].loD > max)
                            {
                                siftDpMapL[sift.pos] = sift;
                            }
                            else
                            {
                                siftDpMapL[sift.pos] = sift;
                            }
                        }
                    }
                    else {

                        LogResponseL[i].at<float>(r, c) = 0;
                    }
                }
                else
                {
                    LogResponseL[i].at<float>(r, c) = 0;
                }
                if (LogResponseR[i].at<float>(r, c) >= maxLoDR * threshold)
                {
                    cv::minMaxLoc(LogResponseR[i](cv::Range((r - window) < 0 ? 0 : (r - window), r + window < gray32FL.rows ? (r + window) : (gray32FL.rows - 1)), cv::Range((c - window) < 0 ? 0 : (c - window), (c + window) < gray32FL.cols ? (c + window) : (gray32FL.cols - 1))), nullptr, &max, nullptr, &maxIdx);
                    if (maxIdx.x == window && maxIdx.y == window) {

                        siftDP sift = {
                            .pos = {maxIdx.x + c, maxIdx.y + r},
                            .sigma = sigma,
                            .loD = max,
                        };
                        if (siftDpMapR.count(sift.pos) && siftDpMapR[sift.pos].loD > max)
                        {
                            siftDpMapR[sift.pos] = sift;
                        }
                        else
                        {
                            siftDpMapR[sift.pos] = sift;
                        }
                    }
                }
                else
                {
                    LogResponseR[i].at<float>(r, c) = 0;
                }
            }
        }
        LogResponseL[bolbNum] = cv::max(LogResponseL[bolbNum], LogResponseL[i]);
        LogResponseR[bolbNum] = cv::max(LogResponseR[bolbNum], LogResponseR[i]);
        sigma *= 1.414;
        kernel_size = int(sigma * 6) | 1;
    }

    LogResponseL[bolbNum] += cv::Scalar::all(1);
    cv::log(LogResponseL[bolbNum], LogResponseL[bolbNum]);
    LogResponseR[bolbNum] += cv::Scalar::all(1);
    cv::log(LogResponseR[bolbNum], LogResponseR[bolbNum]);
    cv::normalize(LogResponseL[bolbNum], LogResponseL[bolbNum], 0, 1, cv::NORM_MINMAX);
    cv::normalize(LogResponseR[bolbNum], LogResponseR[bolbNum], 0, 1, cv::NORM_MINMAX);
    LogResponseL[bolbNum - 1] += cv::Scalar::all(1);
    cv::log(LogResponseL[bolbNum - 1], LogResponseL[bolbNum - 1]);
    LogResponseR[bolbNum - 1] += cv::Scalar::all(1);
    cv::log(LogResponseR[bolbNum - 1], LogResponseR[bolbNum - 1]);
    cv::normalize(LogResponseL[bolbNum - 1], LogResponseL[bolbNum - 1], 0, 1, cv::NORM_MINMAX);
    cv::normalize(LogResponseR[bolbNum - 1], LogResponseR[bolbNum - 1], 0, 1, cv::NORM_MINMAX);
    cv::imshow("imageL", grayL);
    cv::imshow("imageR", grayR);
    cv::imshow("LogResponseL", LogResponseL[bolbNum]);
    cv::imshow("LogResponseR", LogResponseR[bolbNum]);
    cv::imshow("LogResponseL0", LogResponseL[bolbNum -1]);
    cv::imshow("LogResponseR0", LogResponseR[bolbNum -1]);

    cv::SiftFeatureDetector siftDetetor;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat mask = cv::Mat::ones(gray32FL.rows, gray32FL.cols, CV_8U);
    cv::Ptr<cv::SIFT> siftp = siftDetetor.create(50,3,0.04,10,3);
    cv::Mat desp;
    // siftp->detect(grayL, keypoints, mask);
    siftp->detectAndCompute(grayL, mask, keypoints, desp);
    cv::drawKeypoints(grayL, keypoints, grayL);
    std::map<double, siftDP, std::greater<double>> map;
    int c = 0;
    for (auto s : siftDpMapL)
    {
        map[s.second.loD] = s.second;
    }
    cv::imshow("gray32FL", grayL);
     for (auto s : map){
            c++;
            // if (c > 20)
            //     break;
            cv::circle(grayL, s.second.pos, 5, cv::Scalar(255, 0, 0));
     }
    cv::imshow("gray32FL1", grayL);
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}