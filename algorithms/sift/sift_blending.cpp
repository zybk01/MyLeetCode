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
    float resizeRatio = 4;
    cv::resize(imageL, imageL, cv::Size(imageL.cols / resizeRatio, imageL.rows / resizeRatio), 0.5, 0.5);
    cv::resize(imageR, imageR, cv::Size(imageR.cols / resizeRatio, imageR.rows / resizeRatio), 0.5, 0.5);
    imageL = imageL(cv::Rect(0, 0, imageL.cols, imageL.rows / 2));
    imageR = imageR(cv::Rect(0, 0, imageR.cols, imageR.rows / 2));
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
    // Find homography calculates Homography, RANSAC random sampling consistency algorithm
    // Mat h = findHomography(points1, points2, USAC_DEFAULT);
    Mat h = findHomography(points1, points2, RANSAC);
    // points1.clear();
    // points2.clear();
    // for (size_t i = 0; i < pairwise_matches_[0].matches.size(); i++)
    // {
    //     // queryIdx is the descriptor of the aligned image and the subscript of the feature point.
    //     points1.push_back(features[0].keypoints[pairwise_matches_[0].matches[i].queryIdx].pt);
    //     // queryIdx is the descriptor of the sample image and the subscript of the feature point.
    //     points2.push_back(features[1].keypoints[pairwise_matches_[0].matches[i].trainIdx].pt);
    // }
    // Mat h2 = findHomography(points1, points2, RANSAC);
    // Use homography to warp image mapping
    cv::Mat resultWarp;
    print(format(h, Formatter::FormatType::FMT_DEFAULT));
    print(format(pairwise_matches_[0].H, Formatter::FormatType::FMT_DEFAULT));
    // print(format(h2, Formatter::FormatType::FMT_DEFAULT));
    warpPerspective(grayL, resultWarp, h, grayL.size());
    // warpPerspective(grayL, resultWarp, pairwise_matches_[0].H, grayL.size());

    cv::imshow("gray32FL", grayL);
    cv::imshow("gray32FR", grayR);
    cv::imshow("warpPerspective", resultWarp);

    vector<cv::Mat> images;
    images.push_back(imageL);
    images.push_back(imageR);
    cv::Mat result;
    cv::Ptr<cv::Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);
    auto status = stitcher->stitch(images, result);
    Ptr<WarperCreator> w = makePtr<SphericalWarper>();

    Ptr<detail::RotationWarper> wR = w->create(1);
    {
        stitcher->estimateTransform(images);

        Mat_<float> K;
        Mat R;
        stitcher->cameras()[0].R.convertTo(R, CV_32F);
        stitcher->cameras()[0].R = R;
        stitcher->cameras()[0].K().convertTo(K, CV_32F);
        K(0, 0) *= (float)2;
        K(0, 2) *= (float)2;
        K(1, 1) *= (float)2;
        K(1, 2) *= (float)2;

        wR->warp(grayL, K, stitcher->cameras()[0].R, INTER_LINEAR, BORDER_REFLECT, resultWarp);
    }
    cv::imshow("warpPerspective1", resultWarp);
    if (status == Stitcher::Status::OK)
    {
        cv::imshow("stitcher", result);
    }
    LOGD("status %d", status);

    // lapalacian blending
    cv::Rect roi(gray32FL.cols / 3, gray32FL.rows / 1.5, gray32FL.cols /  15, gray32FL.rows / 15); // 定义ROI的位置和大小
    Mat Mask = imageL.clone();
    Mask.setTo(cv::Scalar(0, 0, 0));
    LOGD("status %d", status);
    rectangle(Mask, roi, Scalar(1, 1, 1), FILLED);

    // Mask(roi).setTo(cv::Scalar(1));
    LOGD("status %d", status);
#define pyramidLayer 5
    if (pyramidLayer >= 2)
    {
        vector<Mat> pyramidL(pyramidLayer);
        vector<Mat> pyramidR(pyramidLayer);
        vector<Mat> pyramidMask(pyramidLayer);
        vector<Mat> LapPyramidL(pyramidLayer);
        vector<Mat> LapPyramidR(pyramidLayer);
        warpPerspective(imageL, imageL, h, imageL.size());
        imageL.convertTo(imageL, CV_32FC3);
        imageR.convertTo(imageR, CV_32FC3);
        Mask.convertTo(Mask, CV_32FC3);
        buildPyramid(imageL, pyramidL, pyramidLayer);
        buildPyramid(imageR, pyramidR, pyramidLayer);
        buildPyramid(Mask, pyramidMask, pyramidLayer);
        LOGD("status %d", status);
        for (int i = 0; i < (pyramidLayer - 1); i++)
        {
            pyrUp(pyramidL[i + 1], LapPyramidL[i], pyramidL[i].size());
            LapPyramidL[i] = pyramidL[i] - LapPyramidL[i];
            pyrUp(pyramidR[i + 1], LapPyramidR[i], pyramidR[i].size());
            LapPyramidR[i] = pyramidR[i] - LapPyramidR[i];
        }
        LOGD("status %d %d", status, pyramidMask[0].type());
        LapPyramidL[(pyramidLayer - 1)] = pyramidL[(pyramidLayer - 1)];
        LapPyramidR[(pyramidLayer - 1)] = pyramidR[(pyramidLayer - 1)];
        vector<Mat> lapBlendResult(pyramidLayer);
        for (int i = (pyramidLayer - 1); i >= 0; i--)
        {
            Mat temp = LapPyramidL[i].mul(pyramidMask[i]) + LapPyramidR[i].mul((pyramidMask[i] * -1 + Scalar(1, 1, 1)));
            lapBlendResult[i] = temp;
        }
        LOGD("status %d", status);
        cv::imshow("Mask", pyramidMask[0]);
        Mat blendResult = lapBlendResult[(pyramidLayer - 1)];
        for (int i = (pyramidLayer - 2); i >= 0; i--)
        {
            pyrUp(blendResult, blendResult, lapBlendResult[i].size());
            blendResult = lapBlendResult[i] + blendResult;
        }
        blendResult.convertTo(blendResult, CV_8UC3);
        cv::imshow("laplacianblendResult", blendResult);
    }
    {
        Mat blendResult = imageL.clone();
        Mat laplacianMat;

        Laplacian(imageR, laplacianMat, CV_32FC3);
        Mat MaskState = imageL.clone();
        int cnt = 0;
        map<pair<int, int>, int> idxMap;
        MaskState.setTo(cv::Scalar(0, 0, 0));
        for (int i = 0; i < Mask.rows; i++)
        {
            for (int j = 0; j < Mask.cols; j++)
            {
                if (Mask.at<Vec3f>(i, j)[0] == 1)
                {
                    if (i == 0 || j == Mask.cols - 1 || j == 0 || i == Mask.rows - 1)
                    {
                        // MaskState.at<Scalar>(i, j) = Scalar(1, 1, 1);
                    }
                    else if (Mask.at<Vec3f>(i + 1, j)[0] || Mask.at<Vec3f>(i, j + 1)[0] || Mask.at<Vec3f>(i - 1, j)[0] || Mask.at<Vec3f>(i, j - 1)[0])
                    {
                        idxMap[make_pair(i, j)] = cnt++;
                    }
                }
            }
        }
        LOGD("cnt %d", cnt);
        int dim[2] = {cnt, cnt};
        Mat solveaMat(cnt, cnt, CV_32F);
        // cv::SparseMat solveaMat(2, dim, CV_32FC3);
        Mat solvelapcianMat(cnt, 1, CV_32FC3);
        // solveaMat.setTo(0);
        // solve()
        for (auto &itr : idxMap)
        {
            int i = itr.first.first, j = itr.first.second;
            // *solveaMat.ptr(itr.second, idxMap[make_pair(itr.second, itr.second)], true) = -4;
            solveaMat.at<float>(itr.second, itr.second) = -4;
            solvelapcianMat.at<Vec3f>(itr.second, 0) = laplacianMat.at<Vec3f>(i, j);
            if (idxMap.count(make_pair(i + 1, j)))
            {
                // *solveaMat.ptr(itr.second, idxMap[make_pair(i + 1, j)], true) = 1;
                solveaMat.at<float>(itr.second, idxMap[make_pair(i + 1, j)]) = 1;
            }
            else
            {
                solvelapcianMat.at<Vec3f>(itr.second, 0) -= imageL.at<Vec3f>(i + 1, j);
            }
            if (idxMap.count(make_pair(i - 1, j)))
            {
                // *solveaMat.ptr(itr.second, idxMap[make_pair(i - 1, j)], true) = 1;
                solveaMat.at<float>(itr.second, idxMap[make_pair(i - 1, j)]) = 1;
            }
            else
            {
                solvelapcianMat.at<Vec3f>(itr.second, 0) -= imageL.at<Vec3f>(i + 1, j);
            }
            if (idxMap.count(make_pair(i, j + 1)))
            {
                // *solveaMat.ptr(itr.second, idxMap[make_pair(i, j + 1)], true) = 1;
                solveaMat.at<float>(itr.second, idxMap[make_pair(i, j + 1)]) = 1;
            }
            else
            {
                solvelapcianMat.at<Vec3f>(itr.second, 0) -= imageL.at<Vec3f>(i + 1, j);
            }
            if (idxMap.count(make_pair(i, j - 1)))
            {
                // *solveaMat.ptr(itr.second, idxMap[make_pair(i, j - 1)], true) = 1;
                solveaMat.at<float>(itr.second, idxMap[make_pair(i, j - 1)]) = 1;
            }
            else
            {
                solvelapcianMat.at<Vec3f>(itr.second, 0) -= imageL.at<Vec3f>(i + 1, j);
            }
        }
        vector<Mat> result(3);
        vector<Mat> solvelapcianMatVec;
        split(solvelapcianMat, solvelapcianMatVec);
        solve(solveaMat, solvelapcianMatVec[0], result[0]);
        split(solvelapcianMat, solvelapcianMatVec);
        solve(solveaMat, solvelapcianMatVec[1], result[1]);
        split(solvelapcianMat, solvelapcianMatVec);
        solve(solveaMat, solvelapcianMatVec[2], result[2]);
        merge(result, result[0]);
        // Mat result =  solveaMat.inv() * solvelapcianMat;
        for (int i = 0; i < Mask.rows; i++)
        {
            for (int j = 0; j < Mask.cols; j++)
            {
                if (idxMap.count(make_pair(i, j))) {
                    blendResult.at<Vec3f>(i, j) = result[0].at<Vec3f>(idxMap[make_pair(i, j)], 0);
                }
            }
        }
        blendResult.convertTo(blendResult, CV_8UC3);
        cv::imshow("poissonblendResult", blendResult);
    }
    Mat blendResult = imageL.mul(Mask) + imageR.mul((Mask * -1 + Scalar(1, 1, 1)));
    blendResult.convertTo(blendResult, CV_8UC3);
    cv::imshow("blendResultCat", blendResult);

    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}