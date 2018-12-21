#include "Map.hpp"

using namespace cv;

Map::Map() {
    m_world = new World();
    m_wall = new Wall();
    m_ground = new Ground;
}

Map::~Map() {
    delete m_world;
    delete m_ball;
    delete m_wall;
    delete m_ground;
}

void Map::start(const cv::Point2i &start) {
    this->m_startPoint = start;
}

cv::Point2i &Map::start() {
    return this->m_startPoint;
}

cv::Point2i &Map::finish() {
    return this->m_finishPoint;
}

void Map::finish(const cv::Point2i &finish) {
    this->m_finishPoint = finish;
}

Wall *Map::wall() const {
    return this->m_wall;
}

void Map::createPhysics() {
    btVector3 gravity(0, 0, -6000);

    // Creer un Monde physique
    m_world->create(gravity);

    // Creer une Balle Physique
    btVector3 ballOrigin(m_startPoint.x - m_wall->getPoints().cols / 2.0f,
                         -m_startPoint.y + m_wall->getPoints().rows / 2.0f,
                         100);

    m_ball = new Ball(BALL_RADIUS);
    m_ball->createPhysics(ballOrigin, BALL_MASS, m_world);

    // Creer un sol physique
    btVector3 origineGround(0, 0, -50);
    m_ground->createPhysics(m_wall->getPoints(), origineGround, m_world);

    // Creer les murs physique
    btVector3 pillardDims(PILLAR_WIDTH, PILLAR_WIDTH, PILLAR_HEIGHT);
    m_wall->createPhysics(pillardDims, 0, m_world);
}

void Map::draw(GL *glWindow, Mat rvec_decomp) {
    //	Scale to window size
    GLint windowWidth, windowHeight;
    glfwGetWindowSize(glWindow->windows(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set PROJECTION
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, (double) windowWidth / (double) windowHeight, 0.1f, 10000.0f);

    // Set MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // init Lumiere
    glWindow->initLight();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    *	DRAW STUFF
    */
    btQuaternion rx(1, 0, 0, static_cast<const btScalar &>(rvec_decomp.at<double>(0, 0) / 2));
    btQuaternion ry(0, 1, 0, static_cast<const btScalar &>(rvec_decomp.at<double>(1, 0) / 2));
    btQuaternion rz(0, 0, 1, -rvec_decomp.at<double>(2, 0) / 2);

    btQuaternion quad = rx * ry * rz;

    m_world->getInstance()->stepSimulation(1.f / 60.f, 10);
    //std::cout << world->getNumCollisionObjects() << std::endl;

    for (int i = m_world->getInstance()->getNumCollisionObjects() - 1; i >= 0; --i) {
        btCollisionObject *obj = m_world->getInstance()->getCollisionObjectArray()[i];
        btRigidBody *body = btRigidBody::upcast(obj);
        btTransform trans;

        // Get Transformation
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        } else {
            trans = obj->getWorldTransform();
        }

        // Matrice de transformation physique
        btScalar mat[16];
        trans.getOpenGLMatrix(mat);

        trans.setRotation(quad);
        body->getMotionState()->setWorldTransform(trans);
        trans.getOpenGLMatrix(mat);

        switch (i) {
            case 0:
                m_ball->draw(mat);
                break;
            case 1:
                m_ground->draw(mat, m_wall->getPoints(), m_finishPoint);
                break;
            case 2:
                m_wall->draw(mat);
                break;
            default:
                break;
        }
    }

    // Update Screen
    glfwSwapBuffers(glWindow->windows());

    // Check for any input, or window movement
    glfwPollEvents();
}

void Map::setStartX(const int point) {
    this->m_startPoint.x = point;
}

void Map::setStartY(const int point) {
    this->m_startPoint.y = point;
}

void Map::setFinishX(const int point) {
    this->m_finishPoint.x = point;
}

void Map::setFInishY(const int point) {
    this->m_finishPoint.y = point;
}