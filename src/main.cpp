#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <GL/gl.h>

int main(int argc, char** argv) {
    // OpenGL test
    glClear(GL_COLOR_BUFFER_BIT);

    // OpenCV test
    cv::Mat images;

    std::cout << "Hello World" << std::endl;
    return 0;
}