#ifndef __BALL_HPP__
#define __BALL_HPP__

#include "Pos.hpp"

class Ball {
public:
    Ball();
    Ball(Pos pos, unsigned radius);
    ~Ball();
private:
    unsigned radius;
    Pos pos;
};

#endif /* __BALL_HPP__ */
