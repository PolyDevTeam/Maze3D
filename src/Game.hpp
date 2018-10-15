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
	double dist(cv::Point2i, cv::Point2i);
	cv::Point2i nearest(std::vector<cv::Point2i>, cv::Point2i);
	cv::Mat applyMask(cv::Mat, int, int, int, int, int);
	int random(int, int);
	bool squareIsBlack(cv::Mat, int);
	void initialisationDetection(cv::VideoCapture capture, cv::Point2i &p11, cv::Point2i &p12, cv::Point2i &p13, cv::Point2i &p14);
	cv::Mat getWallsMat(cv::Mat, cv::Point, cv::Point);
    void stop();
private:
    Map map;
};

#endif /* __GAME_HPP__ */
