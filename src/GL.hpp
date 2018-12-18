#ifndef __GL_HPP__
#define __GL_HPP__

#include <opencv2/opencv.hpp>
//#include "btBulletDynamicsCommon.h"

#include <GLFW/glfw3.h>
#include <GL/glu.h>

//void init_Lumiere(void);
//int draw_GL(GLFWwindow *GL_window, cv::Mat homography, cv::Mat cloud, cv::Point2i start, cv::Point2i finish, btDiscreteDynamicsWorld* world, btCompoundShape* wall);

//void def_cube(void);
//void def_carre(void);
//void def_axes(void);
//void def_walls(cv::Mat cloud);
//static void draw_frame();

//std::vector<uchar> Mat_to_Array(cv::Mat img);

#define GL_WIDTH 640
#define GL_HEIGHT 480

class GL {
public:
    GL();

    void init();

    static void keyboard_callback(GLFWwindow *windows, int key, int scancode, int action, int mods);

    static void resize_callback(GLFWwindow *window, int new_width, int new_height);

    void draw();

private:
    GLFWwindow *windows;
};

#endif /* __GL_HPP__ */