#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <vector>
#include "Ball.hpp"
#include "Wall.hpp"
#include "Ground.hpp"
#include "GL.hpp"

class Map {
public:
    Map();
    cv::Point2i& start();
    void start(const cv::Point2i& start);
	void setStart(int x, int y);
    cv::Point2i& finish();
    void finish(const cv::Point2i& finish);
	void setFinish(int x, int y);
    void createPhysics();
    Wall* wall() const;
    void draw(GL* glWindow, cv::Mat rvec_decomp);
private:
    World* m_world;
    Ball* m_ball;
    Wall* m_wall;
    Ground* m_ground;
    cv::Point2i m_startPoint;
    cv::Point2i m_finishPoint;
	int m_startPointx;
	int m_startPointy;
	int m_finishPointx;
	int m_finishPointy;
};

#endif /* __MAP_HPP__ */
