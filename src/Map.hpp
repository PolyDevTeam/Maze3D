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
    cv::Point2i& finish();
    void finish(const cv::Point2i& finish);
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
};

#endif /* __MAP_HPP__ */
