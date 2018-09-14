#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "Map.hpp"

class Game {
public:
    void start();
    void stop();
private:
    Map map;
};

#endif /* __GAME_HPP__ */
