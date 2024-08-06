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

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/IMG_8533.jpg");
    if (num == 1)
    {
        LOGD("-l <image input path 1> -s <threasholdSpace> -c <threasholdColor> -luma <threasholdluma> defaultPath IMG_8533.jpg  10 10 10");
    }
    int threasholdColor = 10;
    int threasholdSpace = 10;
    int threasholdluma = 10;
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
                else if (strcmp(args[i], "-c") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    threasholdColor = atoi(input);
                }
                else if (strcmp(args[i], "-s") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    threasholdSpace = atoi(input);
                }
                else if (strcmp(args[i], "-luma") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    threasholdluma = atoi(input);
                }
            }
        }
    }
    cv::Mat imageL;
    try
    {
        /* code */
        imageL = cv::imread(lImage);
        int resizeRatio = 1;
        cv::resize(imageL, imageL, cv::Size(imageL.cols / resizeRatio, imageL.rows / resizeRatio));
        LOGD("imageL w %d h %d", imageL.cols, imageL.rows);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    cv::imshow("imageL", imageL);

    cv::Mat imageResult;
    cv::bilateralFilter(imageL, imageResult, 6*threasholdSpace + 1, threasholdColor, threasholdSpace);
    LOGD("imageL threasholdColor %d, threasholdSpace %d", threasholdColor, threasholdSpace);
    cv::namedWindow("bilateralFilter");
    cv::moveWindow("bilateralFilter", 100, 100);
    cv::imshow("bilateralFilter", imageResult);

    cv::Mat imageYuv;
    vector<cv::Mat> imageYuvCh;
    vector<cv::Mat> imageYuvChResult(3);
    cv::cvtColor(imageL, imageYuv, cv::COLOR_RGB2YUV);
    cv::split(imageYuv, imageYuvCh);
    cv::bilateralFilter(imageYuvCh[0], imageYuvChResult[0], 6*threasholdSpace + 1, threasholdluma, threasholdSpace);
    cv::bilateralFilter(imageYuvCh[1], imageYuvChResult[1], 6*threasholdSpace + 1, threasholdColor, threasholdSpace);
    cv::bilateralFilter(imageYuvCh[2], imageYuvChResult[2], 6*threasholdSpace + 1, threasholdColor, threasholdSpace);
    cv::merge(imageYuvChResult, imageYuv);
    cv::cvtColor(imageYuv, imageYuv, cv::COLOR_YUV2RGB);
    LOGD("imageL threasholdColor %d, threasholdluma %d, threasholdSpace %d", threasholdColor, threasholdluma, threasholdSpace);
    cv::namedWindow("bilateralFilterYuv");
    cv::moveWindow("bilateralFilterYuv", 200, 200);
    cv::imshow("bilateralFilterYuv", imageYuv);

    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}