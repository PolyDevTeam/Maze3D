#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <vector>
#include "Ball.hpp"
#include "Wall.hpp"

class Map {
public:
    Map();
private:
    unsigned length;
    unsigned width;
    Ball ball;
    std::vector<Wall> walls;
};

#endif /* __MAP_HPP__ */
