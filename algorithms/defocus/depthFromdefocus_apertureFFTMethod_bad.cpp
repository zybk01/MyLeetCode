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

struct DepthParam {
    float aperture[Cam_num];
    float focalLenght[Cam_num];
    float lensPos[Cam_num];
    // float lensPos[Cam_num];
};

cv::Mat calDft(cv::Mat img, int display = 0)
{
    int M = cv::getOptimalDFTSize( img.rows );
    int N = cv::getOptimalDFTSize( img.cols );
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

    if (display) {
        mag += cv::Scalar::all(1);
        cv::log(mag, mag);

        // crop the spectrum, if it has an odd number of rows or columns
        mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));

        int cx = mag.cols/2;
        int cy = mag.rows/2;

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
    else {
        mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));
    }

    return mag;
}

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/defocus/lImage1.jpg");
    string rImage("resources/defocus/rImage1.jpg");
    if (num == 1) {
        LOGD("-l <image input path 1> -r <image input path 2> defaultPath lImage.jpg rImage.jpg");
    }
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
            }
        }
    }
    DepthParam depthParam;
    depthParam.focalLenght[0] = 24;
    depthParam.focalLenght[1] = 24;
    depthParam.aperture[0] = depthParam.focalLenght[0] / 4.5;
    depthParam.aperture[1] = depthParam.focalLenght[0] / 16;
    depthParam.lensPos[0] = 25;
    depthParam.lensPos[1] = 25;
    // depthParam.focalLenght[0] = 18;
    // depthParam.focalLenght[1] = 18;
    // depthParam.aperture[0] = depthParam.focalLenght[0] / 11;
    // depthParam.aperture[1] = depthParam.focalLenght[0] / 3.5;
    // depthParam.lensPos[0] = 20;
    // depthParam.lensPos[1] = 20;
    cv::Mat imageL = cv::imread(lImage);
    cv::Mat imageR = cv::imread(rImage);
    LOGD("imageL w %d h %d", imageL.cols, imageL.rows);
    LOGD("imageR w %d h %d", imageR.cols, imageR.rows);
    cv::resize(imageL, imageL, cv::Size(imageL.cols / 8 ,imageL.rows / 8));
    cv::resize(imageR, imageR, cv::Size(imageR.cols / 8 ,imageR.rows / 8));
    // cv::imshow(lImage, imageL);

    cv::Mat grayL;
    cv::cvtColor(imageL, grayL, cv::COLOR_RGB2GRAY);
    cv::Mat grayR;
    cv::cvtColor(imageR, grayR, cv::COLOR_RGB2GRAY);
    int dft_size = cv::getOptimalDFTSize(grayR.cols);

    // # 对图像进行零填充
    // padding = dft_size - gray.shape[0]
    // padding = np.pad(gray, ((0, padding), (0, padding)), mode='constant', constant_values=0)

    // # 执行傅里叶变换
    cv::Mat grayfftL;
    cv::Mat gray32FL;
    cv::Mat grayfftR;
    cv::Mat gray32FR;
    grayL.convertTo(gray32FL, CV_32FC1);
    grayR.convertTo(gray32FR, CV_32FC1);
    int kernel_size = 3;
    // cv::Mat kernel = (cv::Mat_<float>(kernel_size, kernel_size) << 
    //     0, -1, 0,
    //     -1, 4, -1,
    //     0, -1, 0);
    // kernel = calDft(kernel);
    // cv::imshow("kernel", kernel);
    kernel_size = 7;
    int sigma = 3;
    // // 进行卷积操作
    // cv::Mat dst;
    // cv::filter2D(grayR, grayR, grayR.depth(), kernel);
    cv::Mat kernelGaussian = cv::getGaussianKernel(kernel_size, sigma, CV_64F);
    // cv::Mat roiR(grayR, cv::Rect(0, 0, 400, 400));
    // cv::filter2D(roiR, roiR, roiR.depth(), kernelGaussian);
    // cv::Mat roiL(grayL, cv::Rect(0, 0, 400, 400));
    // cv::filter2D(roiL, roiL, roiL.depth(), kernelGaussian);
    LOGD("%d %f", grayL.at<byte>(0, 0), gray32FL.at<float>(0, 0));
    cv::imshow("grayL", grayL);
    cv::imshow("grayR", grayR);
    cv::dft(gray32FL, grayfftL, cv::DFT_REAL_OUTPUT);
    cv::imshow("grayfftL", grayfftL);
    cv::dft(gray32FR, grayfftR, cv::DFT_REAL_OUTPUT);
    cv::imshow("grayfftR", grayfftR);
    grayfftR = calDft(gray32FR, 1);
    cv::imshow("grayfftR", grayfftR);
    // cv::imshow("gray32FL", gray32FL);
    // cv::imshow("gray32FR", gray32FR);
    cv::Mat grayResult(cv::Size(gray32FL.cols,gray32FL.rows), CV_32FC1);
    float scanWindow = 8;
    float limitWindow = 8;
    float pixelSize = 0.00391;
    float fU = scanWindow / 2;
    float fV = scanWindow / 2;
    float fUV2 = fU * fU + fV * fV;
    float aperatureRatio =  1 - depthParam.aperture[1] / depthParam.aperture[0];
    float offset = depthParam.lensPos[0] * depthParam.focalLenght[0] / (depthParam.lensPos[0] - depthParam.focalLenght[0] + 2 * scanWindow * pixelSize *(depthParam.focalLenght[0] / depthParam.aperture[0]));
    float offset1 = depthParam.lensPos[0] * depthParam.focalLenght[0] / (depthParam.lensPos[0] - depthParam.focalLenght[0] + 2 * limitWindow * pixelSize * (depthParam.focalLenght[0] / depthParam.aperture[0]));
    for (int i = scanWindow / 2; i < gray32FL.rows - scanWindow / 2; i++) {
        for (int j = scanWindow / 2; j < gray32FL.cols - scanWindow / 2; j++) {
            cv::Mat graySubL = gray32FL(cv::Rect(j - scanWindow / 2,i - scanWindow / 2,scanWindow, scanWindow));
            cv::Mat graySubR = gray32FR(cv::Rect(j - scanWindow / 2,i - scanWindow / 2,scanWindow, scanWindow));
            // cv::dft(graySubL, grayfftL, cv::DFT_REAL_OUTPUT);
            // cv::dft(graySubR, grayfftR, cv::DFT_REAL_OUTPUT);
            grayfftL = calDft(graySubL);
            grayfftR = calDft(graySubR);
            // cv::imshow("grayfftL", grayfftL);
            // cv::imshow("grayfftR", grayfftR);
            if (grayfftL.at<float>(fU, fV) > 0 && grayfftR.at<float>(fU, fV) > 0)
            grayResult.at<float>(i, j) = log(grayfftL.at<float>(fU, fV)) - log(grayfftR.at<float>(fU, fV)) / 2 / PI / PI;
            grayResult.at<float>(i, j) = grayResult.at<float>(i, j) / fUV2;
            grayResult.at<float>(i, j) = fabs(grayResult.at<float>(i, j) / aperatureRatio);
            grayResult.at<float>(i, j) = sqrtf(grayResult.at<float>(i, j));
            grayResult.at<float>(i, j) = depthParam.lensPos[0] * depthParam.focalLenght[0] / (depthParam.lensPos[0] - depthParam.focalLenght[0] + 2 * grayResult.at<float>(i, j) * (depthParam.focalLenght[0] / depthParam.aperture[0]));
            if(i % 100 == 0 && j %100 == 0) {
                LOGD("debug1 %f", grayResult.at<float>(i, j));
            }
            grayResult.at<float>(i, j) = (grayResult.at<float>(i, j) - 0) * 100/ 1000;
            // grayResult.at<float>(i, j) = (grayResult.at<float>(i, j) - offset1) * 100/ 1000;
            if(i % 100 == 0 && j %100 == 0) {
                LOGD("debug %f", grayResult.at<float>(i, j));
            }
        }
    }
    
    cv::Mat grayResult8U;
    grayResult.convertTo(grayResult8U, CV_8UC1);
    LOGD("grayResult8U %d offset %f offset1 %f",grayResult8U.at<uchar>(5, 5), offset, offset1);
    cv::imshow("grayResult", grayResult8U);

    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}