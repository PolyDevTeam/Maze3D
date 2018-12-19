#ifndef __GAME_HPP__
#define __GAME_HPP__

//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/opencv.hpp>

#include "Map.hpp"
#include "GL.hpp"

class Game {
public:
    void start(int argc, char **argv);
private:
    void getCalibration() const;

    void initialise();

    void run();

    void resetDetection(cv::VideoCapture capture);

    GL *glWindows;
    std::vector<cv::Point2i> objRef;

    Map* map;

    cv::Point2i pTopRight;
    cv::Point2i pTopLeft;
    cv::Point2i pBottomLeft;
    cv::Point2i pBottomRight;

    bool quit;
};

#endif /* __GAME_HPP__ */
