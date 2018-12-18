#include "Ball.hpp"

Ball::Ball(float radius) : radius(radius) {

}

Ball::~Ball() = default;

void Ball::createPhysics(btVector3 origin, float mass, World *world) {
    //	Creation de la Forme
    physics = new btSphereShape(radius);

    //	Position & Rotation de l'objet
    btTransform myTransform;
    myTransform.setIdentity();
    myTransform.setOrigin(origin);
    btVector3 localInertia(0, 0, 0);

    //	Objet Dynamique
    if (mass) {
        physics->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState *myMotionState;
    myMotionState = new btDefaultMotionState(myTransform);

    //	Regroupement des informations pour creer le body
    btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, physics, localInertia);
    myBoxRigidBodyConstructionInfo.m_restitution = 0.0f;
    //	On creer la balle physique
    btRigidBody *body;
    body = new btRigidBody(myBoxRigidBodyConstructionInfo);

    world->getInstance()->addRigidBody(body);
}
