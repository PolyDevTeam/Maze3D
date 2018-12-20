#ifndef __WALL_HPP__
#define __WALL_HPP__

#include <btBulletDynamicsCommon.h>

#include "Image.hpp"
#include "World.hpp"
#include "GL.hpp"

#define PILLAR_WIDTH 0.5
#define PILLAR_HEIGHT 100

class Wall {
public:
    Wall();
    ~Wall();
    void update(Image src, cv::Rect zone, cv::Rect boundingTriangle, cv::Rect boundingCircle);
    void createPhysics(btVector3 pillardDims, float z, World* world);
    cv::Mat getPoints() const;
    btCompoundShape* getPhysics() const;
    void draw(const btScalar mat[]);
private:
    cv::Mat points;
    btCompoundShape* physics;
};

#endif /* __WALL_HPP__ */
