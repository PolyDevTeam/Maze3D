#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include <btBulletDynamicsCommon.h>

class World {
public:
    void create(btVector3 gravity);
    btDiscreteDynamicsWorld* getInstance() const;
private:
    btDiscreteDynamicsWorld* instance;
};

#endif /* __WORLD_HPP__ */
