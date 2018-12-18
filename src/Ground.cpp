#include "Ground.hpp"

void Ground::createPhysics(cv::Mat walls, btVector3 origin, World* world) {
    //	Creation de la Forme
    btVector3 dims(walls.cols / 2.0f, walls.rows / 2.0f, 50);
    btCollisionShape *groundShape = new btBoxShape(dims);

    btTransform myTransform;
    myTransform.setIdentity();

    // Position du sol
    myTransform.setOrigin(origin);
    btVector3 localInertiaGround(0, 0, 0);

    btDefaultMotionState *myMotionState;
    myMotionState = new btDefaultMotionState(myTransform);

    btRigidBody::btRigidBodyConstructionInfo infoGround(0, myMotionState, groundShape, localInertiaGround);

    //	Création du body
    physics = new btRigidBody(infoGround);

    //	Activation du mode KINEMATIC pour le body
    physics->setCollisionFlags(physics->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    physics->setActivationState(DISABLE_DEACTIVATION);

    // On ajoute le sol dans le monde Bullet
    world->getInstance()->addRigidBody(physics);
}