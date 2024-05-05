#include "ThreadPool.h"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <fstream>
#include <iostream>
#include <windows.h>



int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/defocus/harris.png");
    string rImage("resources/defocus/harris.png");
    if (num == 1) {
        LOGD("-l <image input path 1> -r <image input path 2> defaultPath lImage.jpg rImage.jpg");
    }
    int threashold = 10;
    if (args) {
        for (int i = 0; i < num; i++) {
            if (args[i]) {
                if (strcmp(args[i], "-l") == 0) {
                    snprintf(input, sizeof(input), "%s", args[i]);
                    lImage = string(input);
                }
                else if (strcmp(args[i], "-r") == 0) {
                    snprintf(input, sizeof(input), "%s", args[i]);
                    rImage = string(input);
                }
                else if (strcmp(args[i], "-t") == 0) {
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
    // cv::resize(imageL, imageL, cv::Size(imageL.cols / 8 ,imageL.rows / 8));
    // cv::resize(imageR, imageR, cv::Size(imageR.cols / 8 ,imageR.rows / 8));

    cv::Mat grayL;
    cv::cvtColor(imageL, grayL, cv::COLOR_RGB2GRAY);
    cv::Mat grayR;
    cv::cvtColor(imageR, grayR, cv::COLOR_RGB2GRAY);
    cv::imshow("imageL", grayL);

    cv::Mat gray32FL;
    cv::Mat gray32FR;
    cv::Mat gradient32FX;
    cv::Mat gradient32FY;
    grayL.convertTo(gray32FL, CV_32FC1);
    grayR.convertTo(gray32FR, CV_32FC1);
    int kernel_size = 15;
    int sigma = 50;

    cv::Mat kernelGaussian = cv::getGaussianKernel(kernel_size, sigma, CV_64F);
    // cv::Mat roiR(gray32FL, cv::Rect(0, 0, 400, 400));
    // cv::filter2D(roiR, roiR, roiR.depth(), kernelGaussian);
    // cv::Mat roiL(gray32FR, cv::Rect(0, 0, 400, 400));
    // cv::filter2D(roiL, roiL, roiL.depth(), kernelGaussian);
    kernel_size = 3;
    cv::Mat kernelX = (cv::Mat_<float>(kernel_size, kernel_size) << 
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);
    cv::Mat kernelY = (cv::Mat_<float>(kernel_size, kernel_size) << 
        1, 2, 1,
        0, 0, 0,
        -1, -2, -1);
    // 进行卷积操作
    cv::filter2D(gray32FL, gradient32FX, gray32FL.depth(), kernelX);
    // cv::normalize(gradient32FX, gradient32FX, 0, 1, cv::NORM_MINMAX);
    cv::imshow("gradient32FX", gradient32FX);
    cv::filter2D(gray32FR, gradient32FY, gray32FL.depth(), kernelY);
    // cv::normalize(gradient32FY, gradient32FY, 0, 1, cv::NORM_MINMAX);
    cv::imshow("gradient32FY", gradient32FY);

    float scanWindow = 5;
    float k = 0.04;
    cv::Mat grayResult(cv::Size(gray32FL.cols,gray32FL.rows), CV_32FC1);
    for (int i = scanWindow / 2; i < gray32FL.rows - scanWindow / 2; i++) {
        for (int j = scanWindow / 2; j < gray32FL.cols - scanWindow / 2; j++) {
            cv::Mat graySubX = gradient32FX(cv::Rect(j - scanWindow / 2,i - scanWindow / 2,scanWindow, scanWindow));
            cv::Mat graySubY = gradient32FY(cv::Rect(j - scanWindow / 2,i - scanWindow / 2,scanWindow, scanWindow));
            float a = 0, b = 0, c = 0, lamda1,lamda2;
            for (int w = 0; w < scanWindow; w++) {
                for (int h = 0; h < scanWindow; h++) {
                    a += graySubX.at<float>(w, h) * graySubX.at<float>(w, h);
                    b += 2 * graySubX.at<float>(w, h) * graySubY.at<float>(w, h);
                    c += graySubY.at<float>(w, h) * graySubY.at<float>(w, h);
                }
            }
            lamda1 = 0.5 * (a + c + sqrtf(b * b + (a - c) * (a - c)));
            lamda2 = 0.5 * (a + c + sqrtf(b * b - (a - c) * (a - c)));
            grayResult.at<float>(i, j) = lamda1 * lamda2 + k * (lamda1 + lamda2) * (lamda1 + lamda2);
            if(i % 100 == 0 && j %100 == 0) {
                LOGD("debug1 %f", grayResult.at<float>(i, j));
            }
            // grayResult.at<float>(i, j) = grayResult.at<float>(i, j) > threashold;
        }
    }
    cv::Mat grayResult8U;
    cv::normalize(grayResult, grayResult, 0, 1, cv::NORM_MINMAX);
    // grayResult.convertTo(grayResult8U, CV_8UC1);

    cv::imshow("grayResult", grayResult);
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}