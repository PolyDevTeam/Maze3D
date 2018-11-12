#include <opencv2/opencv.hpp>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <Box2D/Box2D.h>

b2World initBox2D(float g_x = 0.0, float g_y = 0.0);
void createCircle(b2World& world, int type, float x, float y, float radius);
void makePhysicWalls(b2World& world, cv::Mat cloudWalls);
int test_box2D_Walls(void);


int test_Box2D(void);