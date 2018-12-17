#ifndef __WALL_HPP__
#define __WALL_HPP__

#include "Pos.hpp"

class Wall {
public:
    Wall(Pos begin, Pos end);
    ~Wall();
private:
    Pos begin;
    Pos end;
};

#endif /* __WALL_HPP__ */
