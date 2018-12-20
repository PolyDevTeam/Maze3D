#ifndef __GROUND_HPP__
#define __GROUND_HPP__

#include <opencv2/core/mat.hpp>
#include "World.hpp"
#include "GL.hpp"

class Ground {
public:
    void createPhysics(cv::Mat walls, btVector3 origin, World* world);
    void draw(btScalar mat[], cv::Mat wall, cv::Point2i finishPoint);
private:
    btRigidBody* physics;
};

#endif /* __GROUND_HPP__ */
