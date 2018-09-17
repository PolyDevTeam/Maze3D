#include "Image.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

void Image::setLabel(Image img, const std::string str, std::vector<cv::Point> &contour) {
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    cv::Size text = cv::getTextSize(str, fontface, scale, thickness, &baseline);
    cv::Rect r = cv::boundingRect(contour);

    cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    cv::rectangle(img, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255),
                  CV_FILLED);
    cv::putText(img, str, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}
