#include <opencv2/opencv.hpp>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

void controls_GL(GLFWwindow* GL_window, int key, int scancode, int action, int mods);
void resize_GL(GLFWwindow* window, int new_width, int new_height);

GLFWwindow* init_GL(int width, int height);
int draw_GL(GLFWwindow *GL_window, cv::Mat homography, cv::Mat cloud);


void def_carre(void);
void def_axes(void);
void def_walls(cv::Mat cloud);
static void draw_frame();


std::vector<uchar> Mat_to_Array(cv::Mat img);
