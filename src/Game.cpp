#include "Game.hpp"

//#if __APPLE__
//    #include <OpenGL/gl.h>
//#else
//    #include <GL/gl.h>
//#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

#include "Image.hpp"

using namespace std;
using namespace cv;

void Game::start(int argc, char **argv) {
    // Init Game variables
//    cv::Mat src = cv::imread("MAZEtte.png");
    Image src;
    Image gray;
    Image bw;

    Image dst;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> approx;

    // Get WebCam
    VideoCapture capture(0);

    // Loop
    while (cvWaitKey(30) != 'q') {
        // Refresh webcam image
        capture >> src;

        // Convert to grayscale
        cv::cvtColor(src, gray, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        blur(gray, bw, Size(3, 3));
        cv::Canny(gray, bw, 80, 240, 3);

        // Display canny image
        cv::imshow("bw", bw);

        // Find contours
        cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        src.copyTo(dst);

        for (auto &contour : contours) {
            // Approximate contour with accuracy proportional
            // to the contour perimeter
            cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);

            // Skip small or non-convex objects
            if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
                continue;
            }

            if (approx.size() == 3) {
                Image::setLabel(dst, "TRI", contour);  // Triangles
            }
            else if (approx.size() >= 4 && approx.size() <= 6) {
                // Number of vertices of polygonal curve
                int vtc = static_cast<int>(approx.size());

                // Use the degrees obtained above and the number of vertices
                // to determine the shape of the contour
                if (vtc == 4) {
                    Image::setLabel(dst, "RECKT", contour);
                }
                else if (vtc == 5) {
                    Image::setLabel(dst, "PENTA", contour);
                }
                else if (vtc == 6) {
                    Image::setLabel(dst, "HEXA", contour);
                }
            }
            else {
                // Detect and label circles
                double area = cv::contourArea(contour);
                cv::Rect r = cv::boundingRect(contour);
                int radius = r.width / 2;

                if (std::abs(1 - ((double) r.width / r.height)) <= 0.2 &&
                    std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2)
                    Image::setLabel(dst, "CIR", contour);
            }
        }

        // Draw game
        cv::imshow("dst", dst);
    }

    // Stop the game
    stop();
}

void Game::stop() {

}