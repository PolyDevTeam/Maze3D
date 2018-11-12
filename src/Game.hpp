#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Map.hpp"
#include <opencv2/opencv.hpp>

class Game {
public:
	//	MAIN
	void start(int argc, char** argv);
	void stop(void);

	//	UTILS
	int highestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);


	int lowestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);


	int rightestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);


	int leftestPoint(cv::Point2i, cv::Point2i, cv::Point2i, cv::Point2i);


	int random(int, int);


	double dist(cv::Point2i, cv::Point2i);


	cv::Point2i nearest(std::vector<cv::Point2i>, cv::Point2i);


	cv::Mat applyMask(cv::Mat, int, int, int, int, int);


	bool squareIsBlack(cv::Mat);


	cv::Mat getWallsMat(cv::Mat, cv::Rect, cv::Rect, cv::Rect);

	void initialisationDetection(cv::VideoCapture capture, cv::Point2i &p11, cv::Point2i &p12, cv::Point2i &p13, cv::Point2i &p14);

private:
    Map map;
};

#endif /* __GAME_HPP__ */
