#ifndef __GL_HPP__
#define __GL_HPP__

#include <opencv2/opencv.hpp>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <GL/glu.h>

#define GL_WIDTH 640
#define GL_HEIGHT 480

class GL {
public:
    GL();

    void init();

    void initLight();

    static void keyboard_callback(GLFWwindow *windows, int key, int scancode, int action, int mods);

    static void resize_callback(GLFWwindow *window, int new_width, int new_height);

    GLFWwindow *windows();

private:
    GLFWwindow *m_windows;
};

#endif /* __GL_HPP__ */