#include "btBulletDynamicsCommon.h"
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

int testBullet(void);
int testBullet2(void);

btDiscreteDynamicsWorld* initPhysics(btVector3 gravite, cv::Mat cloud, float radius, btVector3 pillardDims, btVector3 ballOrigine, btScalar ballMass);
int clearPhysics(btDynamicsWorld* dynamicsWorld);


btDiscreteDynamicsWorld* createWorld(btVector3 gravite);
std::vector<btRigidBody*> createWalls(cv::Mat cloud, btVector3 pillardDims, float y, btDiscreteDynamicsWorld* world);
btRigidBody* createBall(float radius, btVector3 origine, btScalar mass, btDiscreteDynamicsWorld* world);
btRigidBody* createGround(cv::Mat cloud, btVector3 origine, btDiscreteDynamicsWorld* world);
btRigidBody* createGround(btVector3 groundDims, btVector3 origine, btDiscreteDynamicsWorld* world);

