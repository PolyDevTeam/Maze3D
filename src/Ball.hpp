#ifndef __BALL_HPP__
#define __BALL_HPP__

#include "World.hpp"
#include "GL.hpp"

#define BALL_RADIUS 10.0f
#define BALL_MASS 1000000

class Ball {
public:
    Ball(float radius);

    ~Ball();

    void createPhysics(btVector3 origin, float mass, World *world);

    void draw(const btScalar mat[]);

private:
    btCollisionShape *physics;
    float radius;
};

#endif /* __BALL_HPP__ */
