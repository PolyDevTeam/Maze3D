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

void Ground::draw(btScalar mat[], cv::Mat wall, cv::Point2i finishPoint) {
    glPushMatrix();
    {
        glMultMatrixf(mat);
        glBegin(GL_QUADS);
        {
            float offset = 25;
            glColor3f(0.23f, 0.12f, 0.09f);
            glVertex3f(-wall.cols/2.0f - offset, wall.rows/2.0f + offset, 0);
            glVertex3f(wall.cols/2.0f + offset, wall.rows/2.0f + offset, 0);
            glVertex3f(wall.cols/2.0f + offset, -wall.rows/2.0f - offset, 0);
            glVertex3f(-wall.cols/2.0f - offset, -wall.rows/2.0f - offset, 0);
        }
        glEnd();
        glPushMatrix();
        glTranslatef(-wall.cols / 2.0f, wall.rows / 2.0f, 0.0f);
        glBegin(GL_QUADS);
        {
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(finishPoint.x, -finishPoint.y, 0.1);
            glVertex3f(finishPoint.x, -finishPoint.y - 20, 0.1);
            glVertex3f(finishPoint.x + 20, -finishPoint.y - 20, 0.1);
            glVertex3f(finishPoint.x + 20, -finishPoint.y, 0.1);
        }
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}