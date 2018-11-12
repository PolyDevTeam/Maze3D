//#if __APPLE__
//    #include <OpenGL/gl.h>
//#else
//    #include <GL/gl.h>
//#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>


//#define GLFW_INCLUDE_GLU
//#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <random>

#include "GL.hpp"
#include "Box2D.hpp"

#include "Game.hpp"
#include "Image.hpp"

using namespace std;
using namespace cv;





void Game::start(int argc, char **argv) {

	/*
	*	TESTS DES BRIQUES GL ET BOX2D
	*/
	/*if (test_GL() == 1) {
		cout << "Test GL OK" << endl;
	}
	else stop();
	if (test_Box2D() == 1) {
		cout << "Test BOX2D OK" << endl;
	}
	else stop();
	/*
	*	FIN DES TESTS
	*/


	Image src, gray, src_canny;
    Image dst;


	Image cut, cut_src, cut_gray, cut_src_canny;


	Mat toad;
	Mat wallPoints;
	Mat final;

    vector<std::vector<cv::Point>> contours;
    vector<cv::Point> approx;

	Point2i p0;
	Point2i pRect;


	Point2i p1, p2, p3, p4, p11, p12, p13, p14;
	Point2i start, finish;
	Rect r, zone_jeu;
	vector<Point2i> objCapture;
	vector<Point2i> objRef;
	vector<Point2i> vectRect;
	Mat h;
	Rect r_t, r_c;

	int eps = 100;
	int eps2 = 50;
	int eps3 = 100;

    // Get WebCam
    VideoCapture capture(0);
	//480 x 640

	bool initialisation = false;
	bool quit = false;

	/*
	*	DEBUT INITIALISATION
	*/
	while (!quit  && !initialisation ) {


		//initialisation
		capture >> src;


		// Convert to grayscale
		cv::cvtColor(src, gray, CV_BGR2GRAY);

		// Use Canny instead of threshold to catch squares with gradient shading
		blur(gray, src_canny, Size(3, 3));
		cv::Canny(gray, src_canny, 80, 240, 3);

		// Display canny image
		cv::imshow("src_canny", src_canny);

		// Find contours
		cv::findContours(src_canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
					//Récupération des coordonnées du centre du rectangle détecté
					r = boundingRect(contour);
					pRect.x = r.x + r.width / 2;
					pRect.y = r.y + r.height / 2;
					vectRect.push_back(pRect);

					if (!squareIsBlack(src(r)))
						zone_jeu = r;

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

				if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 && std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2) {
					Image::setLabel(dst, "CIR", contour);
				}


			}
		}

		// Affichage caméra avec label
		cv::imshow("Cam", dst);

		//détection des 4 repères et de la zone de jeu
		if (vectRect.size() >= 5) {

			vector<std::vector<cv::Point>> contours;
			vector<cv::Point> approx;
			Rect r_t, r_c;

			// Find contours
			cv::findContours(src_canny.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

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
					r_t = boundingRect(contour);
				}
				else if (approx.size() >= 4 && approx.size() <= 6) {
					// Number of vertices of polygonal curve
					int vtc = static_cast<int>(approx.size());

					// Use the degrees obtained above and the number of vertices
					// to determine the shape of the contour
					if (vtc == 4) {
						Image::setLabel(dst, "RECKT", contour);

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

					if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 && std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2) {
						Image::setLabel(dst, "CIR", contour);
						r_c = boundingRect(contour);
					}


				}
			}
			//affichage caméra avec label triangle + cercle
			imshow("tri-cercle", dst);

			//détection du triangle et du cercle
			if (r_c.width != 0 && r_t.width != 0) {
				//on détermine les 4 rectangles servant de point de repère pour le labyrinthe puis on les sauvegarde
				p11 = nearest(vectRect, Point2i(640, 0));
				p12 = nearest(vectRect, Point2i(0, 0));
				p13 = nearest(vectRect, Point2i(0, 480));
				p14 = nearest(vectRect, Point2i(640, 480));
				objRef.clear();
				objRef.push_back(p11);
				objRef.push_back(p12);
				objRef.push_back(p13);
				objRef.push_back(p14);

				//sauvegarde du point de depart et d'arrivee
				start = r_c.tl();
				finish = r_t.tl();
				start.x -= p12.x;
				start.y -= p12.y;
				finish.x -= p12.x;
				finish.y -= p12.y;

				if (zone_jeu.width != 0)
				{
					wallPoints = getWallsMat(src_canny, zone_jeu, r_t, r_c);

					//validation de l'initialisation
					char bon = 'k';
					while (bon != 'y' && bon != 'n' && bon != 'r' && bon != ' ' && bon != 'q')
					{
						bon = cv::waitKey(0);
						if (bon == 'y' || bon == ' ')
							initialisation = true;
					}
				}
			}
		}
		//vérifie si l'utilisateur veut quitter
		if (cvWaitKey(10) == 'q')
			quit = true;
	}



    // Loop
    while (!quit) {

		/*if (cvWaitKey(0) == 'r') {
			initialisationDetection(capture, p11, p12, p13, p14);

		}*/

        // Refresh webcam image
        capture >> src;
		

        // Convert to grayscale
        cv::cvtColor(src, gray, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        blur(gray, gray, Size(3, 3));
        cv::Canny(gray, src_canny, 80, 240, 3);

		int high = highestPoint(p11, p12, p13, p14);
		int low = lowestPoint(p11, p12, p13, p14);
		int right = rightestPoint(p11, p12, p13, p14);
		int left = leftestPoint(p11, p12, p13, p14);

		eps2=0.20*dist(p11, p12);
		applyMask(src_canny, high, low, right, left, eps2).copyTo(src_canny);
        // Display canny image
        cv::imshow("src_canny", src_canny);


        // Find contours
        cv::findContours(src_canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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



		cv::line(dst, p11, p12, Scalar(0, 255, 0), 3);
		cv::line(dst, p12, p13, Scalar(0, 255, 0), 3);
		cv::line(dst, p13, p14, Scalar(0, 255, 0), 3);
		cv::line(dst, p14, p11, Scalar(0, 255, 0), 3);

		// Draw game
		cv::imshow("dst", dst);

		if (vectRect.size() >= 4) {

			p1 = nearest(vectRect, p11);
			p2 = nearest(vectRect, p12);
			p3 = nearest(vectRect, p13);
			p4 = nearest(vectRect, p14);
			//cout << "p1 : " << p1 << " " << p2 << " " << p3 << " " << p4 << endl;
	
			circle(dst, p1, 2, Scalar(0, 0, 255), 10);
			circle(dst, p2, 2, Scalar(0, 0, 255), 10);
			circle(dst, p3, 2, Scalar(0, 0, 255), 10);
			circle(dst, p4, 2, Scalar(0, 0, 255), 10);

			// Draw game
			cv::imshow("dst", dst);

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
			cout << "YES" << endl;
			h = findHomography(objCapture, objRef, RANSAC);
			cout << h << endl << endl;

			try {
				warpPerspective(wallPoints, final, h, final.size());
			}
			catch (std::exception const& e)
			{
				cerr << "ERREUR : " << e.what() << endl;
			}

			cv::imshow("final", final);

			GLFWwindow *win = init_GL(640, 480);
			draw_GL(win, h, wallPoints, start, finish);
		}

		//vérifie si l'utilisateur veut quitter
		if (cvWaitKey(10) == 'q')
			quit = true;
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

Mat Game::applyMask(Mat src_canny, int high, int low, int right, int left, int eps) {
	Mat dest;
	Mat mask = Mat::zeros(src_canny.rows, src_canny.cols, src_canny.type());
	rectangle(mask, Rect(left-eps, high-eps, right - left + 2*eps, low - high + 2*eps), Scalar(255, 255, 255), FILLED);
	bitwise_and(src_canny, mask, dest);
	return dest;
}


//retourne le point dans vectRect le plus proche de target
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

bool Game::squareIsBlack(Mat square) {
	int cptNoir = 0;

	for (int i = 0; i < square.rows; i++)
		for(int j = 0; j < square.cols; j++)
			if (square.at<Vec3b>(i, j)[0] < 150 && square.at<Vec3b>(i, j)[1] < 150 && square.at<Vec3b>(i, j)[3] < 150)
				cptNoir++;

	return cptNoir >= 0.80*square.rows*square.cols;
}

Mat Game::getWallsMat(Mat src, Rect zone, Rect rt, Rect rc) {

	Mat zone2Jeu_canny, src_canny;

	src.copyTo(src_canny);

	//on retire de la zone de jeu le triangle et le cercle matérialisants le départ et l'arrivée
	rectangle(src_canny, rc, Scalar(0, 0, 0), FILLED);
	rectangle(src_canny, rt, Scalar(0, 0, 0), FILLED);

	//	cut the canny img to take the maze only
	zone2Jeu_canny = src_canny(zone);

	imshow("zonecanny", zone2Jeu_canny);

	return zone2Jeu_canny;
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

void Game::stop() {

}