//#if __APPLE__
//    #include <OpenGL/gl.h>
//#else
//    #include <GL/gl.h>
//#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>

#include "Game.hpp"
#include "Image.hpp"

using namespace std;
using namespace cv;


void controls_GL(GLFWwindow* GL_window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(GL_window, GL_TRUE);
}


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
	glVertex2f(-1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glEnd();

	//	Y axe
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.0f, -1.0f);
	glEnd();

	//	Z axe
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glEnd();
}

void def_walls(Mat cloud) {
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (int row = 0; row < cloud.rows; row++) {
		for (int col = 0; col < cloud.cols; col++) {
			uchar intensity = cloud.at<uchar>(row, col);
			
			if (intensity != 0) {
				float x = (float)col / (float)cloud.cols;
				float y = -(float)row / (float)cloud.rows;
				//cout << "X : " << x << " Y : " << y << endl;
				glVertex3f(x, y, 0);
			}

		}
	}
	glEnd();

}

vector<uchar> Mat_to_Array(Mat img) {
	vector<uchar> vec_img;
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




GLFWwindow* init_GL(int width = 800, int height = 800) {

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

	// Get info of GPU and supported OpenGL version
	cout << "Renderer : " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version supported : " << glGetString(GL_VERSION) << endl;


	glEnable(GL_DEPTH_TEST); // Depth Testing

	//glDepthFunc(GL_LEQUAL);
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	return GL_window;
};



int draw_GL(GLFWwindow *GL_window, Mat homography, Mat cloud) {

	// Scale to window size
	GLint windowWidth, windowHeight;
	glfwGetWindowSize(GL_window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	Set PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (double)windowWidth / (double)windowHeight, 0.1f, 100.0f);

	//	Set MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	while (!glfwWindowShouldClose(GL_window)) {


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		*	DRAW STUFF
		*/
		def_axes();

		glPushMatrix();
		glTranslatef(-0.5f, 0.5f, 0.0f);
		def_walls(cloud);
		glPopMatrix();

		// Update Screen
		glfwSwapBuffers(GL_window);

		// Check for any input, or window movement
		glfwPollEvents();


	}


	glfwTerminate();
	return 0;
}



void Game::start(int argc, char **argv) {
    // Init Game variables
	//cv::Mat src = cv::imread("MAZEtte.png");

	GLFWwindow *win = init_GL(640,480);
	Mat a;

	// echequier
	int blockSize = 40;
	int imageSize = blockSize * 8;
	Mat chessBoard(imageSize, imageSize, CV_8UC1, Scalar::all(0));
	unsigned char color = 0;

	for (int i = 0; i < imageSize; i = i + blockSize) {
		color = ~color;
		for (int j = 0; j < imageSize; j = j + blockSize) {
			Mat ROI = chessBoard(Rect(i, j, blockSize, blockSize));
			ROI.setTo(Scalar::all(color));
			color = ~color;
		}
	}

	//imshow("test", chessBoard);


	draw_GL(win, a, chessBoard);

    Image src;
    Image gray;
    Image bw;

    Image dst;

	Mat toad;
	Mat wallPoints;
	Mat final;

    vector<std::vector<cv::Point>> contours;
    vector<cv::Point> approx;

	Point2i p0;
	Point2i pRect;


	Point2i p1;
	Point2i p2;
	Point2i p3;
	Point2i p4;
	Point2i p11;
	Point2i p12;
	Point2i p13;
	Point2i p14;
	Rect r;
	vector<Point2i> objCapture;
	vector<Point2i> objRef;
	vector<Point2i> vectRect;
	Mat h;

	int eps = 100;
	int eps2 = 50;
	int eps3 = 100;

    // Get WebCam
    VideoCapture capture(0);
	//480 x 640

	toad = imread("C:\\Users\\Stéphane\\Pictures\\bad_toad.png", 1);

	/*p0.x = 560;
	p0.y = 70;
	objRef.push_back(p0);
	p0.x = 70;
	p0.y = 70;
	objRef.push_back(p0);
	p0.x = 70;
	p0.y = 370;
	objRef.push_back(p0);
	p0.x = 560;
	p0.y = 370;
	objRef.push_back(p0);*/

	//p0.x = 199;
	//p0.y = 100;
	//objRef.push_back(p0);
	//p0.x = 100;
	//p0.y = 100;
	//objRef.push_back(p0);
	//p0.x = 100;
	//p0.y = 160;
	//objRef.push_back(p0);
	//p0.x = 199;
	//p0.y = 160;
	//objRef.push_back(p0);

	bool initialisation;


	while (cvWaitKey(10) != ' ') {
		capture >> src;
		imshow("dst", src);
	}

	initialisation = false;
	while (cvWaitKey(10) != 'q' && initialisation == false) {

		//initialisation
		capture >> src;
		

		// Convert to grayscale
		cv::cvtColor(src, gray, CV_BGR2GRAY);

		// Use Canny instead of threshold to catch squares with gradient shading
		blur(gray, bw, Size(3, 3));
		cv::Canny(gray, bw, 80, 240, 3);

		// Display canny image
		cv::imshow("bw", bw);

		// Find contours
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		src.copyTo(dst);

		vectRect.clear();

		for (auto &contour : contours) {

			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);

			// Skip small or non-convex objects
			if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
				continue;
			}

			if (approx.size() == 3) {
				Image::setLabel(dst, "TRI", contour);  // Triangles
			}
			else if (approx.size() >= 4 && approx.size() <= 6) {
				// Number of vertices of polygonal curve
				int vtc = static_cast<int>(approx.size());

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4) {
					Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);
					
					pRect.x = r.x + r.width / 2;
					pRect.y = r.y + r.height / 2;
					vectRect.push_back(pRect);

				}
				else if (vtc == 5) {
					Image::setLabel(dst, "PENTA", contour);
				}
				else if (vtc == 6) {
					Image::setLabel(dst, "HEXA", contour);
				}
			}
			else {
				// Detect and label circles
				double area = cv::contourArea(contour);
				cv::Rect r = cv::boundingRect(contour);
				int radius = r.width / 2;

				if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
					std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2)
					Image::setLabel(dst, "CIR", contour);
			}
		}



		// Draw game
		cv::imshow("dst", dst);

		if (vectRect.size()>=4) {

			p11 = nearest(vectRect, Point2i(640,0));
			p12 = nearest(vectRect, Point2i(0, 0));
			p13 = nearest(vectRect, Point2i(0, 480));
			p14 = nearest(vectRect, Point2i(640, 480));

			objRef.push_back(p11);
			objRef.push_back(p12);
			objRef.push_back(p13);
			objRef.push_back(p14);
			wallPoints = getWallsMat(src, p12, p14);

			initialisation = true;
			cout<<"FIN INITIALISATION"<<endl;
		}

		
	}





    // Loop
    while (cvWaitKey(1) != 'q') {

		if (cvWaitKey(1) == 'r') {
			initialisationDetection(capture, p11, p12, p13, p14);

		}

        // Refresh webcam image
        capture >> src;
		

        // Convert to grayscale
        cv::cvtColor(src, gray, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        blur(gray, bw, Size(3, 3));
        cv::Canny(gray, bw, 80, 240, 3);

		int high = highestPoint(p11, p12, p13, p14);
		int low = lowestPoint(p11, p12, p13, p14);
		int right = rightestPoint(p11, p12, p13, p14);
		int left = leftestPoint(p11, p12, p13, p14);

		eps2=0.20*dist(p11, p12);
		applyMask(bw, high, low, right, left, eps2).copyTo(bw);
        // Display canny image
        cv::imshow("bw", bw);


        // Find contours
        cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        src.copyTo(dst);

	
		vectRect.clear();


        for (auto &contour : contours) {

            // Approximate contour with accuracy proportional
            // to the contour perimeter
            cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);

            // Skip small or non-convex objects
            if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
                continue;
            }

			if (approx.size() >= 4 && approx.size() <= 6) {
				// Number of vertices of polygonal curve
				int vtc = static_cast<int>(approx.size());

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4) {
					Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);

					/*if (squareIsBlack(src(r), 20)) {
						pRect.x = r.x + r.width / 2;
						pRect.y = r.y + r.height / 2;
						vectRect.push_back(pRect);
					}*/
					pRect.x = r.x + r.width / 2;
					pRect.y = r.y + r.height / 2;
					vectRect.push_back(pRect);
				}
			}
        }

		line(dst, p11, p12, Scalar(0, 255, 0), 3);
		line(dst, p12, p13, Scalar(0, 255, 0), 3);
		line(dst, p13, p14, Scalar(0, 255, 0), 3);
		line(dst, p14, p11, Scalar(0, 255, 0), 3);

		// Draw game
		cv::imshow("dst", dst);

		if (vectRect.size() >= 4) {

			p1 = nearest(vectRect, p11);
			p2 = nearest(vectRect, p12);
			p3 = nearest(vectRect, p13);
			p4 = nearest(vectRect, p14);
	
			if (dist(p1, p11) < eps3 && dist(p2, p12) < eps3 && dist(p3, p13) < eps3 && dist(p4, p14) < eps3) {
				objCapture.clear();
				objCapture.push_back(p1);
				objCapture.push_back(p2);
				objCapture.push_back(p3);
				objCapture.push_back(p4);
				p11 = p1;
				p12 = p2;
				p13 = p3;
				p14 = p4;

				eps3 = 0.25*dist(p1, p2);
			}

		}

		if (objCapture.size() == 4)
		{
			h = findHomography(objCapture, objRef, RANSAC);


			//cout << h << endl << endl;


			try {
				warpPerspective(wallPoints, final, h, final.size());
			}
			catch (std::exception const& e)
			{
				cerr << "ERREUR : " << e.what() << endl;
			}

			cv::imshow("final", final);
		}
    }

    // Stop the game
    stop();
}

int Game::highestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
	int higher = p11.y;
	
	if (p12.y < higher) higher = p12.y;
	if (p13.y < higher) higher = p13.y;
	if (p14.y < higher) higher = p14.y;
	return higher;
}
int Game::lowestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
	int low = p11.y;

	if (p12.y > low) low = p12.y;
	if (p13.y > low) low = p13.y;
	if (p14.y > low) low = p14.y;
	return low;
}
int Game::rightestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
	int right = p11.x;

	if (p12.x > right) right = p12.x;
	if (p13.x > right) right = p13.x;
	if (p14.x > right) right = p14.x;
	return right;
}
int Game::leftestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
	int left = p11.y;

	if (p12.x < left) left = p12.x;
	if (p13.x < left) left = p13.x;
	if (p14.x < left) left = p14.x;
	return left;
}
Mat Game::applyMask(Mat bw, int high, int low, int right, int left, int eps) {
	Mat dest;
	Mat mask = Mat::zeros(bw.rows, bw.cols, bw.type());
	rectangle(mask, Rect(left-eps, high-eps, right - left + 2*eps, low - high + 2*eps), Scalar(255, 255, 255), FILLED);
	bitwise_and(bw, mask, dest);
	return dest;
}



Point2i Game::nearest(vector<Point2i> vecRect, Point2i target) {
	Point2i ref = vecRect[0];
	double distmin = dist(vecRect[0], target);
	for (int i = 1;i < vecRect.size();i++) {
		if (dist(target, vecRect[i]) < distmin) {
			distmin = dist(target, vecRect[i]);
			ref = vecRect[i];
		}
	}

	return ref;
}

double Game::dist(Point2i p1, Point2i p2){
	
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

int Game::random(int min, int max) {
	default_random_engine gen;
	uniform_int_distribution<int> interval(min, max);
	return interval(gen);
}

bool Game::squareIsBlack(Mat square, int nbRandomPoints = 20) {
	int cptNoir = 0;
	for (int i = 0; i < nbRandomPoints; i++) {
		int xRandom = random(1, square.rows);
		int yRandom = random(1, square.cols);
		
		if (square.at<Vec3b>(xRandom, yRandom)[0] < 100 && square.at<Vec3b>(xRandom, yRandom)[1] < 100 && square.at<Vec3b>(xRandom, yRandom)[3] < 100) {
			cptNoir++;
		}
	}

	return cptNoir >= 0.95*nbRandomPoints;
}



void Game::initialisationDetection(VideoCapture capture, Point2i &p11, Point2i &p12, Point2i &p13, Point2i &p14) {
	Image src;
	Image gray;
	Image bw;
	Image dst;
	vector<std::vector<cv::Point>> contours;
	vector<cv::Point> approx;
	vector<Point2i> vectRect;
	Rect r;
	Point2i pRect;

	cout << "reinitialisation" << endl;

	bool initialisation = false;
	while (cvWaitKey(10) != 'q' && initialisation == false) {

		//initialisation
		capture >> src;

		// Convert to grayscale
		cv::cvtColor(src, gray, CV_BGR2GRAY);

		// Use Canny instead of threshold to catch squares with gradient shading
		blur(gray, bw, Size(3, 3));
		cv::Canny(gray, bw, 80, 240, 3);

		// Display canny image
		cv::imshow("bw", bw);

		// Find contours
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		src.copyTo(dst);

		vectRect.clear();

		for (auto &contour : contours) {

			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);

			// Skip small or non-convex objects
			if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
				continue;
			}

			if (approx.size() >= 4 && approx.size() <= 6) {
				// Number of vertices of polygonal curve
				int vtc = static_cast<int>(approx.size());

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4) {
					Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);

					pRect.x = r.x + r.width / 2;
					pRect.y = r.y + r.height / 2;
					vectRect.push_back(pRect);

				}
			}
		}

		// Draw game
		cv::imshow("dst", dst);

		if (vectRect.size() >= 4) {

			p11 = nearest(vectRect, Point2i(640, 0));
			p12 = nearest(vectRect, Point2i(0, 0));
			p13 = nearest(vectRect, Point2i(0, 480));
			p14 = nearest(vectRect, Point2i(640, 480));

			initialisation = true;
			cout << "FIN REINITIALISATION" << endl;
		}
	}
}

Mat Game::getWallsMat(Mat src, Point left_up, Point right_down) {

	//	Cloud of points to return to do GL Walls
	vector<Point> wall_pts;


	//	MAT for Zone2Jeu -> labirinth
	Mat zone2Jeu, zone2Jeu_gray, zone2Jeu_blur, zone2Jeu_canny;

	//	cut the src img to take the labirinth only
	zone2Jeu = src(Rect(left_up, right_down));

	//	init gray, blur and canny of zone2Jeu
	cvtColor(zone2Jeu, zone2Jeu_gray, CV_BGR2GRAY);
	GaussianBlur(zone2Jeu_gray, zone2Jeu_blur, Size(3, 3), 0, 0);
	Canny(zone2Jeu_blur, zone2Jeu_canny, 100, 30, 3);


	imshow("zonecanny", zone2Jeu_canny);
	// Use Canny instead of threshold to catch squares with gradient shading
	imshow("zone", zone2Jeu);

	Image zone;
	zone2Jeu.copyTo(zone);



	// Amelioration de detection des points de depart et darrive
	/*
	*	TODO : USE MASK TO CUT THE CANNY IMG AND FIND DEPART POINT AND ARRIVE POINT
	*/





	//	DETECTION WALL BY CANNY
	for (int row = 0; row < zone2Jeu_canny.rows; row++) {
		for (int col = 0; col < zone2Jeu_canny.cols; col++) {
			uchar color = zone2Jeu_canny.at<uchar>(row, col);
			if (color > 127) {
				circle(zone, Point(col, row), 1, Scalar(0, 255, 0));
				wall_pts.push_back(Point(col, row));
			}
		}
	}

	return zone2Jeu_canny;

}

void Game::stop() {

}