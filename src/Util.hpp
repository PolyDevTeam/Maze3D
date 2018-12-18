#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <vector>
#include <opencv2/opencv.hpp>

class Util {
public:
    static bool squareIsBlack(cv::Mat square);

    static cv::Point2i nearest(std::vector<cv::Point2i> vecRect, cv::Point2i target);

    static double dist(cv::Point2i p1, cv::Point2i p2);
};

#endif /* __UTIL_HPP__ */
