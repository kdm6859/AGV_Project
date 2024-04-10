#include "stitching.h"


std::tuple<std::vector<cv::KeyPoint>, cv::Mat> My_Stitching::detect_and_describe(cv::Mat image) {

    cv::Mat image_gray;
    cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);

    cv::Ptr<cv::Feature2D> sift_feature = cv::SIFT::create();

    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    sift_feature->detectAndCompute(image_gray, cv::Mat(), keypoints, descriptors);

    return std::make_tuple(keypoints, descriptors);
}


cv::Mat My_Stitching::match_keypoints(cv::Mat imageA, cv::Mat imageB, std::vector<cv::KeyPoint> keypointA, std::vector<cv::KeyPoint> keypointB, cv::Mat featuresA, cv::Mat featuresB, float ratio, double repojThresh) {

    std::vector<std::vector<cv::DMatch>> rawMatches;
    std::vector<cv::DMatch> goodMatches;
    std::vector<cv::Point2f> pointsA, pointsB;

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    matcher->knnMatch(featuresA, featuresB, rawMatches, 2);

    for (int i = 0; i < rawMatches.size(); i++) {
        if ((rawMatches[i].size() == 2) && (rawMatches[i][0].distance < (rawMatches[i][1].distance * ratio))) {

            goodMatches.push_back(rawMatches[i][0]);
        }
    }

    if (goodMatches.size() > 4) {
        for (int i = 0; i < goodMatches.size(); i++) {

            pointsA.push_back(keypointA[goodMatches[i].queryIdx].pt);
            pointsB.push_back(keypointB[goodMatches[i].trainIdx].pt);
        }

        homography = findHomography(pointsB, pointsA, cv::RANSAC);

        return homography;
    }
}


cv::Mat My_Stitching::stitch(cv::Mat imageA, cv::Mat imageB, std::string mode, float ratio, double repojThresh) {

    std::vector<cv::KeyPoint> keypointA, keypointB;
    cv::Mat featuresA, featuresB;
    cv::Mat matchFeatures, matches;
    std::vector<cv::DMatch> goodMatches;
    cv::Mat warp_image, dst;

    if (homography.empty()) {

        std::tie(keypointA, featuresA) = detect_and_describe(imageA);
        std::tie(keypointB, featuresB) = detect_and_describe(imageB);

        homography = match_keypoints(imageA, imageB, keypointA, keypointB, featuresA, featuresB, ratio, repojThresh);

        std::cout << "homography matrix : " << std::endl << homography << std::endl << std::endl;
    }

    if (mode == "width") {

        cv::warpPerspective(imageB, warp_image, homography, cv::Size(imageA.cols * 2, imageA.rows*2));

        dst = warp_image.clone();
        cv::Mat roi(dst, cv::Rect(0, 0, imageA.cols, imageA.rows));
        imageA.copyTo(roi);
    }
    else if (mode == "height") {

        cv::warpPerspective(imageB, warp_image, homography, cv::Size(imageA.cols*2, imageA.rows * 2));

        dst = warp_image.clone();
        cv::Mat roi(dst, cv::Rect(0, 0, imageA.cols, imageA.rows));
        imageA.copyTo(roi);
    }

    return dst;
}
