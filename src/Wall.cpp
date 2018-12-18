#include <opencv/cv.hpp>

#include "Wall.hpp"

using namespace cv;

Wall::Wall() = default;

Wall::~Wall() = default;

void Wall::update(Image src, Rect zone, Rect boundingTriangle, Rect boundingCircle) {
    Mat src_canny;

    src.copyTo(src_canny);

    // On retire de la zone de jeu le triangle et le cercle matérialisants le départ et l'arrivée
    rectangle(src_canny, boundingCircle, Scalar(0, 0, 0), FILLED);
    rectangle(src_canny, boundingTriangle, Scalar(0, 0, 0), FILLED);

    //	cut the canny img to take the maze only
    points = src_canny(zone);
    imshow("zonecanny", points);
}

void Wall::createPhysics(btVector3 pillardDims, float z, World *world) {
    physics = new btCompoundShape();
    std::vector<btRigidBody *> vect;

    for (int row = 0; row < points.rows; row++) {
        for (int col = 0; col < points.cols; col++) {
            uchar intensity = points.at<uchar>(row, col);

            if (intensity != 0) {
                //	Creation de la Forme
                btCollisionShape *pillardShape = new btBoxShape(pillardDims);

                btTransform myTransform;
                myTransform.setIdentity();

                // Position du pillier
                myTransform.setOrigin(btVector3(col - points.cols / 2, -row + points.rows / 2, z));

                physics->addChildShape(myTransform, pillardShape);
            }
        }
    }

    btTransform wallTransform;
    wallTransform.setIdentity();
    wallTransform.setOrigin(btVector3(0, 0, 0));
    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState *wallMotionState;
    wallMotionState = new btDefaultMotionState(wallTransform);

    btRigidBody::btRigidBodyConstructionInfo infoWall(0, wallMotionState, physics, localInertia);
    btRigidBody *bodyWall = new btRigidBody(infoWall);

    //	Activation du mode KINEMATIC pour le body
    bodyWall->setCollisionFlags(bodyWall->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    bodyWall->setActivationState(DISABLE_DEACTIVATION);

    // On ajoute le mur dans le monde Bullet
    world->getInstance()->addRigidBody(bodyWall);
}

cv::Mat Wall::getPoints() const {
    return this->points;
}