#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <string>
#include <opencv2/core.hpp>

class Image : public cv::Mat {
public:
    void static setLabel(Image img, std::string str, std::vector<cv::Point> &contour);
};

#endif /* __IMAGE_HPP__ */
