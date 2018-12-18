#ifndef __GROUND_HPP__
#define __GROUND_HPP__

#include <opencv2/core/mat.hpp>
#include "World.hpp"

class Ground {
public:
    void createPhysics(cv::Mat walls, btVector3 origin, World* world);
private:
    btRigidBody* physics;
};

#endif /* __GROUND_HPP__ */
