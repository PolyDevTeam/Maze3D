#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Map.hpp"

class Game {
public:
    void start(int argc, char** argv);
	int highestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);
	int lowestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);
	int rightestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);
	int leftestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);
	cv::Mat applyMask(cv::Mat, int, int, int, int, int);
    void stop();
private:
    Map map;
};

#endif /* __GAME_HPP__ */
