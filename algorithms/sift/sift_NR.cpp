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

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    string lImage("resources/sift/sift6.jpg");
    string rImage("resources/sift/sift5.jpg");
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
    float resizeRatio = 2;
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

    cv::SiftFeatureDetector siftDetetor;
    std::vector<cv::KeyPoint> keypointsL;
    std::vector<cv::KeyPoint> keypointsR;
    cv::Mat mask = cv::Mat::ones(gray32FL.rows, gray32FL.cols, CV_8UC1);
    cv::Mat despL, despR;
    std::vector<DMatch> matches;
    despL.convertTo(despL, CV_32F);
    despR.convertTo(despR, CV_32F);
    // cv::Ptr<cv::SIFT> siftp = siftDetetor.create(50, 6, 0.01, 100, 1.2);
    // // siftp->detect(grayL, keypoints, mask);
    // siftp->detectAndCompute(grayL, mask, keypointsL, despL);
    // siftp->detectAndCompute(grayR, mask, keypointsR, despR);
    // Detect ORB features and compute descriptors. Calculate ORB features and descriptors.
    LOGD("");
    Ptr<Feature2D> orb = ORB::create();
    // Ptr<Feature2D> orb = ORB::create(200, 2, 10, 100, 0, 2, ORB::ScoreType::FAST_SCORE, 100, 10);
    orb->detectAndCompute(grayL, Mat(), keypointsL, despL);
    orb->detectAndCompute(grayR, Mat(), keypointsR, despR);
    vector<Mat> featureinput = {grayL, grayR};
    std::vector<detail::ImageFeatures> features;
    std::vector<std::vector<KeyPoint>> keypointsTotal;
    std::vector<detail::MatchesInfo> pairwise_matches_;
    cv::UMat matching_mask_;
    std::vector<UMat> descriptors;
    // TODO replace with 1 call to new over load of detectAndCompute
    orb->detect(featureinput, keypointsTotal, Mat());
    orb->compute(featureinput, keypointsTotal, descriptors);

    // store to ImageFeatures
    size_t count = featureinput.size();
    features.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        features[i].img_size = featureinput[i].size();
        features[i].keypoints = std::move(keypointsTotal[i]);
        features[i].descriptors = std::move(descriptors[i]);
    }

    // Match features. Feature point matching
    // Hamming distance for feature point matching
    // Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    detail::BestOf2NearestMatcher nmatcher = detail::BestOf2NearestMatcher();
    nmatcher(features, pairwise_matches_, matching_mask_);
    detail::HomographyBasedEstimator estimator = detail::HomographyBasedEstimator();
    std::vector<detail::CameraParams> cameras_;
    estimator(features, pairwise_matches_, cameras_);
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-HammingLUT");
    matcher->match(despL, despR, matches, Mat());

    // Sort matches by score Arrange from best to worst according to feature point matching results
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches Remove bad feature points
    const int numGoodMatches = matches.size() * 0.4;
    matches.erase(matches.begin() + numGoodMatches, matches.end());
    cv::drawKeypoints(grayL, keypointsL, grayL);
    cv::drawKeypoints(grayR, keypointsR, grayR);

    // Draw top matches
    Mat imMatches;
    // Draw feature point matching map
    drawMatches(grayL, keypointsL, grayR, keypointsR, matches, imMatches);
    imwrite("matches.jpg", imMatches); // Feature point matching relationship diagram between the two images
    // Extract location of good matches
    std::vector<Point2f> points1, points2;
    // Save corresponding points
    for (size_t i = 0; i < matches.size(); i++)
    {
        // queryIdx is the descriptor of the aligned image and the subscript of the feature point.
        points1.push_back(keypointsL[matches[i].queryIdx].pt);
        // queryIdx is the descriptor of the sample image and the subscript of the feature point.
        points2.push_back(keypointsR[matches[i].trainIdx].pt);
    }
    Mat h = findHomography(points1, points2, RANSAC);

    cv::Mat resultWarp;
    print(format(h, Formatter::FormatType::FMT_DEFAULT));
    print(format(pairwise_matches_[0].H, Formatter::FormatType::FMT_DEFAULT));
    warpPerspective(grayL, resultWarp, h, grayL.size());

    cv::imshow("gray32FL", grayL);
    cv::imshow("gray32FR", grayR);
    cv::imshow("warpPerspective", resultWarp);
    warpPerspective(imageL, resultWarp, h, imageL.size());
    imageL = (imageL + imageR) / 2;
    cv::imshow("noiseNR", resultWarp);
    imwrite("noiseNR.jpg", resultWarp); // Feature point matching relationship diagram between the two images
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}