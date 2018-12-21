#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Map.hpp"
#include "GL.hpp"

class Game {
public:
    Game();

    ~Game();

    void start(int argc, char **argv);

private:
    void getCalibration() const;

    void initialise();

    void run();

    void resetDetection(cv::VideoCapture capture);

    GL *glWindows;
    std::vector<cv::Point2i> objRef;

    Map *map;

    cv::Point2i pTopRight;
    cv::Point2i pTopLeft;
    cv::Point2i pBottomLeft;
    cv::Point2i pBottomRight;

    bool quit;
};

#endif /* __GAME_HPP__ */
