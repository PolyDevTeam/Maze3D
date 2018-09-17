#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#if __APPLE__
    #include <OpenGL/gl.h>
#elif defined(_WIN32)
	#include <GLFW/glfw3.h>
	#include <gl/GL.h>
	#include <opencv2/core/opengl.hpp>
#else
	#include <GL/gl.h>
#endif

#include "Pos.hpp"

int main(int argc, char** argv) {
    // OpenGL test
    glClear(GL_COLOR_BUFFER_BIT);

    // OpenCV test
    cv::Mat img;

    std::cout << "Hello World" << std::endl;


	img = cv::imread("MAZEtte.png", cv::IMREAD_COLOR); // Read the file
	if (img.empty()) // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}



	cv::namedWindow("Hello World");
	cv::imshow("Hello World", img); // Show our image inside it.

	cv::waitKey(0); // Wait for a keystroke in the window


	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}