#include "GL.hpp"

using namespace cv;
using namespace std;

void controls_GL(GLFWwindow* GL_window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(GL_window, GL_TRUE);
}

void resize_GL(GLFWwindow* window, int new_width, int new_height) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, new_width, new_height);

	//	Set PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (double)new_width / (double)new_height, 0.1f, 10000.0f);

	//	Set MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
};

void def_carre(void)
{
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, -0.5);
	glEnd();

}

void def_axes(void)
{
	//	X axe
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(-1000.0f, 0.0f);
	glVertex2f(1000.0f, 0.0f);
	glEnd();

	//	Y axe
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 1000.0f);
	glVertex2f(0.0f, -1000.0f);
	glEnd();

	//	Z axe
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1000.0f);
	glVertex3f(0.0f, 0.0f, -1000.0f);
	glEnd();
}

void def_walls(Mat cloud) {
	//sol
	glBegin(GL_QUADS);
	glColor3f(0.23f, 0.12f, 0.09f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -cloud.rows / 1.f, 0);
	glVertex3f(cloud.cols / 1.f, -cloud.rows / 1.f, 0);
	glVertex3f(cloud.cols / 1.f, 0, 0);
	glEnd();

	//murs
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.5f, 0.0f);

	for (int row = 0; row < cloud.rows; row++) {
		for (int col = 0; col < cloud.cols; col++) {
			uchar intensity = cloud.at<uchar>(row, col);

			if (intensity != 0) {
				float x = (float)col / 1.0f;
				float y = -(float)row / 1.0f;
				glVertex3f(x, y, 0);
				glVertex3f(x, y, 50);
			}

		}
	}
	glEnd();

}




void draw_frame() {
	glEnable(GL_TEXTURE_2D);

	/* Draw a quad */
	glColor3f(1, 1, 1); // set global color to white, otherwise this color will be (somehow) added to the texture

	glBegin(GL_QUADS);

	float x = 0.5;
	float y = 0.5;

	glTexCoord2f(0.0f, 0.0f); glVertex2f(-x, y);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-x, -y);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x, -y);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x, y);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}



std::vector<uchar> Mat_to_Array(Mat img) {
	std::vector<uchar> vec_img;
	for (int col = 0; col < img.cols; col++) {
		for (int row = 0; row < img.rows; row++) {

			Vec3b color = img.at<Vec3b>(row, col);

			vec_img.push_back(color[2]); // R
			vec_img.push_back(color[1]); // G
			vec_img.push_back(color[0]); // B


		}
	}

	return vec_img;
}




GLFWwindow* init_GL(int width = 1280, int height = 720) {

	//	Init the library
	if (!glfwInit()) {
		cerr << "Initialisation GLFW Failed" << endl;
		return NULL;
	}

	//	Create a windowed mode window and its OpenGL context 
	GLFWwindow* GL_window = glfwCreateWindow(width, height, "GL_window", NULL, NULL);
	if (!GL_window)
	{
		cerr << "Initialisation of GL window Failed" << endl;
		glfwTerminate();
		return NULL;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(GL_window);

	glfwSetKeyCallback(GL_window, controls_GL);
	glfwSetWindowSizeCallback(GL_window, resize_GL);

	// Get info of GPU and supported OpenGL version
	cout << "Renderer : " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version supported : " << glGetString(GL_VERSION) << endl;


	glEnable(GL_DEPTH_TEST); // Depth Testing

	//glDepthFunc(GL_LEQUAL);
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	return GL_window;
};



int draw_GL(GLFWwindow *GL_window, Mat homography, Mat cloud, Point2i start, Point2i finish) {

	//	Scale to window size
	GLint windowWidth, windowHeight;
	glfwGetWindowSize(GL_window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	Set PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (double)windowWidth / (double)windowHeight, 0.1f, 10000.0f);

	//	Set MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// init texture

		// Clear color and depth buffers


	while (!glfwWindowShouldClose(GL_window)) {


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		*	DRAW STUFF
		*/
		//def_axes();

		glPushMatrix();
		glRotatef(-20.0f, 1.0f, 0, 0);
		glTranslatef(-cloud.cols / 2, cloud.rows / 2, 0.0f);
		def_walls(cloud);
		//indication temporaire du départ et de l'arrivee
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(start.x / 1.0f, -start.y / 1.0f, 0);
		glVertex3f(start.x / 1.0f, -start.y / 1.0f, 100);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(finish.x / 1.0f, -finish.y / 1.0f, 0);
		glVertex3f(finish.x / 1.0f, -finish.y / 1.0f, 100);
		glEnd();
		glPopMatrix();

		//glPushMatrix();
		//glTranslatef(-0.5f, 0.5f, 0.0f);
		//draw_frame();
		//glPopMatrix();

		// Update Screen
		glfwSwapBuffers(GL_window);

		// Check for any input, or window movement
		glfwPollEvents();


	}


	glfwDestroyWindow(GL_window);
	glfwTerminate();
	return 0;
}


int test_GL(void) {
	// Init Game variables
	cv::Mat src = cv::imread("MAZEtte.png");

	GLFWwindow *win = init_GL(640, 480);
	Mat a;

	//echequier
	int blockSize = 40;
	int imageSize = blockSize * 8;
	Mat chessBoard(imageSize, imageSize, CV_8UC3, Scalar::all(0));
	unsigned char color = 0;

	for (int i = 0; i < imageSize; i = i + blockSize) {
		color = ~color;
		for (int j = 0; j < imageSize; j = j + blockSize) {
			Mat ROI = chessBoard(Rect(i, j, blockSize, blockSize));
			ROI.setTo(Scalar::all(color));
			color = ~color;
		}
	}

	imshow("test", chessBoard);

	//	Scale to window size
	GLint windowWidth, windowHeight;
	glfwGetWindowSize(win, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	Set PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (double)windowWidth / (double)windowHeight, 0.1f, 10000.0f);

	//	Set MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	GLuint tex;

	// Create Texture
	glGenTextures(1, &tex);

	// Bind the texture to the texture space
	glBindTexture(GL_TEXTURE_2D, tex); // 2d texture (x and y size)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // scale linearly when image smalled than texture

	//cv::Mat texture_cv = cv::imread("MAZEtte.png");
	cv::Mat texture_cv = chessBoard.clone();

	// READ the image and use it at texture
	// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
	// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_cv.cols, texture_cv.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture_cv.data);


	// choose the texture to use.
	glBindTexture(GL_TEXTURE_2D, tex);

	while (!glfwWindowShouldClose(win)) {


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		*	DRAW STUFF
		*/
		def_axes();

		draw_frame();

		// Update Screen
		glfwSwapBuffers(win);

		// Check for any input, or window movement
		glfwPollEvents();


	}
	glDeleteTextures(1, &tex);
	glfwDestroyWindow(win);
	glfwTerminate();
	return 1;
}