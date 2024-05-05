#include "ThreadPool.h"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <fstream>
#include <iostream>
#include <windows.h>

#define PI (3.141592653)
#define PI2 (2 * 3.141592653)
enum depthCameraType
{
    Cam_left,
    Cam_right,
    Cam_num,
};

struct DepthParam
{
    float aperture[Cam_num];
    float focalLenght[Cam_num];
    float lensPos[Cam_num];
    // float lensPos[Cam_num];
};

cv::Mat calDft(cv::Mat img, int display = 0)
{
    int M = cv::getOptimalDFTSize(img.rows);
    int N = cv::getOptimalDFTSize(img.cols);
    cv::Mat padded;
    cv::copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);

    cv::dft(complexImg, complexImg);

    // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))
    cv::split(complexImg, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat mag = planes[0];

    if (display)
    {
        mag += cv::Scalar::all(1);
        cv::log(mag, mag);

        // crop the spectrum, if it has an odd number of rows or columns
        mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));

        int cx = mag.cols / 2;
        int cy = mag.rows / 2;

        // rearrange the quadrants of Fourier image
        // so that the origin is at the image center
        cv::Mat tmp;
        cv::Mat q0(mag, cv::Rect(0, 0, cx, cy));
        cv::Mat q1(mag, cv::Rect(cx, 0, cx, cy));
        cv::Mat q2(mag, cv::Rect(0, cy, cx, cy));
        cv::Mat q3(mag, cv::Rect(cx, cy, cx, cy));

        q0.copyTo(tmp);
        q3.copyTo(q0);
        tmp.copyTo(q3);

        q1.copyTo(tmp);
        q2.copyTo(q1);
        tmp.copyTo(q2);

        cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    }
    else
    {
        mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));
    }

    return mag;
}

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/defocus/lImage2.jpg");
    string rImage("resources/defocus/rImage2.jpg");
    if (num == 1)
    {
        LOGD("-l <image input path 1> -r <image input path 2> defaultPath lImage.jpg rImage.jpg");
    }
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
            }
        }
    }
    DepthParam depthParam;
    depthParam.focalLenght[0] = 35;
    depthParam.focalLenght[1] = 35;
    depthParam.aperture[0] = depthParam.focalLenght[0] / 4.5;
    depthParam.aperture[1] = depthParam.focalLenght[0] / 16;
    depthParam.lensPos[0] = 35.6234; // 2000
    depthParam.lensPos[1] = 36.6; // 800

    cv::Mat imageL = cv::imread(lImage);
    cv::Mat imageR = cv::imread(rImage);
    LOGD("imageL w %d h %d", imageL.cols, imageL.rows);
    LOGD("imageR w %d h %d", imageR.cols, imageR.rows);
    cv::resize(imageL, imageL, cv::Size(imageL.cols / 4, imageL.rows / 4));
    cv::resize(imageR, imageR, cv::Size(imageR.cols / 4, imageR.rows / 4));

    cv::Mat grayL;
    cv::cvtColor(imageL, grayL, cv::COLOR_RGB2GRAY);
    cv::Mat grayR;
    cv::cvtColor(imageR, grayR, cv::COLOR_RGB2GRAY);
    int dft_size = cv::getOptimalDFTSize(grayR.cols);
    


    cv::Mat gray32FL;
    cv::Mat gray32FR;
    grayL.convertTo(gray32FL, CV_32FC1);
    grayR.convertTo(gray32FR, CV_32FC1);

    int kernel_size = 7;
    int sigma = 2;
    cv::Mat kernelGaussian = cv::getGaussianKernel(kernel_size, sigma, CV_64F);
    // cv::Mat roiR(grayR, cv::Rect(0, 0, 100, 100));
    // cv::filter2D(roiR, roiR, roiR.depth(), kernelGaussian);

    cv::Mat kernel_prefilter = (cv::Mat_<float>(kernel_size, kernel_size) << -0.143, -0.198, -0.105, -0.071, -0.105, -0.198, -0.143,
                                -0.198, -0.192, 0.017, 0.072, 0.017, -0.192, -0.198 - 0, 105, 0.017, 0.284, 0.460, 0.284, 0.017, -0.105 - 0.071, 0, 072, 0.460, 0.644, 0.460, 0.072, -0.071,
                                -0, 105, 0.017, 0.284, 0.460, 0.284, 0.017, -0.105 - 0.198, -0.192, 0.017, 0.072, 0.017, -0.192, -0.198,
                                -0.143, -0.198, -0.105, -0.071, -0.105, -0.198, -0.143);
    cv::Mat kernel_pM = (cv::Mat_<float>(kernel_size, kernel_size) << -0.001, 0.045, 0.179, 0.297, 0.179, 0.045, -0.001,
                         0.045, 0.400, 0.868, 1.093, 0.868, 0.400, 0.045,
                         0.179, 0.868, 2.957, 4.077, 2.957, 0.868, 0.179,
                         0.297, 1.093, 4.077, 6.005, 4.077, 1.093, 0.291,
                         0.179, 0.868, 2.957, 4.017, 2.957, 0.868, 0.179,
                         0.045, 0.400, 0.868, 1.093, 0.868, 0.400, 0.045,
                         -0.001, 0.045, 0.179, 0.297, 0.179, 0.045, -0.001);
    cv::Mat kernel_p1 = (cv::Mat_<float>(kernel_size, kernel_size) << -0.039, -0.091, -0.198, -0.259, -0.198, -0.091, -0.039,
                         -0.091, -0.327, -0.470, -0.425, -0.470, -0.321, -0.091,
                         -0.198, -0.470, 0.335, 1.393, 0.335, -0.470, -0.198,
                         -0.259, -0.425, 1.394, 3.385, 1.393, -0.425, -0.259,
                         -0.198, -0.470, 0.335, 1.393, 0.335, -0.470, -0.198,
                         -0.091, -0.327, -0.470, -0.425, -0.470, -0.327, -0.091,
                         -0.039, -0.091, -0.198, -0.259, -0.198, -0.091, -0.039);
    cv::Mat kernel_p2 = (cv::Mat_<float>(kernel_size, kernel_size) << 0.056, -0, 020, -0.068, -0.061, -0.068, -0.020, 0.056,
                         -0.020, -0.068, 0.059, 0.145, 0.059, -0.068, -0.020,
                         -0.068, 0.059, 0.176, -0.019, 0.176, 0.059, -0.068,
                         -0.061, 0.145, -0.019, -0.698, -0.019, 0.145, -0.061,
                         -0.068, 0.059, 0.176, -0.019, 0.176, 0.059, -0.068,
                         -0.020, -0.068, 0.059, 0.145, 0.059, -0.068, -0.020,
                         0.056, -0.020, -0.068, -0.061, -0.068, -0.020, 0.056);

    cv::Mat M = gray32FR - gray32FL;
    cv::Mat P = gray32FR + gray32FL;
    cv::filter2D(M, M, M.depth(), kernel_prefilter);
    cv::filter2D(P, P, P.depth(), kernel_prefilter);

    cv::imshow("M", M);
    cv::imshow("P", P);

    cv::Mat CM;
    cv::filter2D(M, CM, M.depth(), kernel_pM);
    cv::Mat CP1;
    cv::filter2D(P, CP1, P.depth(), kernel_p1);
    cv::Mat CP2;
    cv::filter2D(P, CP2, P.depth(), kernel_p2);
    cv::imshow("CM", CM);
    cv::imshow("CP1", CP1);
    cv::imshow("CP2", CP2);
    cv::filter2D(CM, CM, CM.depth(), kernelGaussian);
    cv::filter2D(CP1, CP1, CP1.depth(), kernelGaussian);
    cv::filter2D(CP2, CP2, CP2.depth(), kernelGaussian);


    cv::Mat grayResult(cv::Size(gray32FL.cols, gray32FL.rows), CV_32FC1);
    float e = abs(depthParam.lensPos[0] - depthParam.lensPos [1]) / 2;

    for (int i = 0; i < gray32FL.rows; i++)
    {
        for (int j = 0; j < gray32FL.cols; j++)
        {
            float cp2, cp1, cm;
            cp2 = CP2.at<float>(i,j);
            cp1 = CP1.at<float>(i,j);
            cm = CM.at<float>(i,j);
            float beta = 1 - (cp1 + cp2 - cm) / (cp1 + 3 * cp2);
            if (i % 100 == 0 && j % 100 == 0)
            {
                LOGD("debug1 beta %f  cm %f cp1 %f,cp2 %f", beta, cm, cp1 ,cp2);
            }
            grayResult.at<float>(i, j) = 1 / depthParam.focalLenght[0] - 1 / (depthParam.lensPos[0] + (1 + beta) * e);
            if (i % 100 == 0 && j % 100 == 0)
            {
                // LOGD("debug1 %f", grayResult.at<float>(i, j));
            }
            grayResult.at<float>(i, j) = 1 / grayResult.at<float>(i, j);
            if (i % 100 == 0 && j % 100 == 0)
            {
                LOGD("debug2 %f", grayResult.at<float>(i, j));
            }
            // grayResult.at<float>(i, j) = (grayResult.at<float>(i, j) - 800) / 6;
        }
    }

    cv::Mat grayResult8U;
    grayResult += cv::Scalar::all(1);
    cv::log(grayResult, grayResult);
    cv::normalize(grayResult, grayResult, 0, 255, cv::NORM_MINMAX);
    grayResult.convertTo(grayResult8U, CV_8UC1);
    // cv::resize(grayResult8U, grayResult8U, cv::Size(grayResult8U.cols / 8, grayResult8U.rows / 8));
    cv::imshow("grayResult1", grayResult8U);

    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}