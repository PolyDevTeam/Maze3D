

//#if __APPLE__
//    #include <OpenGL/gl.h>
//#else
//    #include <GL/gl.h>
//#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "Image.hpp"
#include "Game.hpp"

using namespace std;
using namespace cv;



void glDraw(Mat Labirinth) {

}





void Game::start(int argc, char **argv) {
    // Init Game variables
//    cv::Mat src = cv::imread("MAZEtte.png");
    Image src;
    Image gray;
    Image bw;

    Image dst;

	Mat toad;
	Mat final;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> approx;

	Point2i p0;
	Point2i p1;
	Point2i p2;
	Point2i p3;
	Point2i p4;
	Point2i p11;
	Point2i p12;
	Point2i p13;
	Point2i p14;
	Rect r;
	std::vector<Point2i> objCapture;
	std::vector<Point2i> objRef;
	std::vector<Point2i> objRef2;
	Mat h;

	int eps = 100;
	int eps2 = 50;

    // Get WebCam
    VideoCapture capture(0);
	//480 x 640

	toad = imread("bad_toad.png", 1);

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

	p0.x = 199;
	p0.y = 100;
	objRef.push_back(p0);
	p0.x = 100;
	p0.y = 100;
	objRef.push_back(p0);
	p0.x = 100;
	p0.y = 160;
	objRef.push_back(p0);
	p0.x = 199;
	p0.y = 160;
	objRef.push_back(p0);


	p1 = Point2i(0, 1);
	p2 = Point2i(0, 2);
	p3 = Point2i(0, 3);
	p4 = Point2i(0, 4);

	bool initialisation;

	bool bool1 = false;
	bool bool2 = false;
	bool bool3 = false;
	bool bool4 = false;
	int compteur = 0;
	
	Size t1, t2, t3, t4;
	Mat zone2Jeu, zone2Jeu_gray, zone2Jeu_gauss, zone2Jeu_canny;
	Mat zone2Jeu_corner, zone2Jeu_corner_normalized, zone2Jeu_corner_scaled;


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

		bool bool1 = false;
		bool bool2 = false;
		bool bool3 = false;
		bool bool4 = false;

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
				r = boundingRect(contour);
				t3 = Size(r.width / 2, r.height / 2);
				//if ((r.x > p3.x - eps && r.x<p3.x + eps && r.y>p3.y - eps && r.y < p3.y + eps) || p3.x == 0) {
				p3.x = r.x + r.width / 2;
				p3.y = r.y + r.height / 2;
				bool3 = true;
				//}
			}
			else if (approx.size() >= 4 && approx.size() <= 6) {
				// Number of vertices of polygonal curve
				int vtc = static_cast<int>(approx.size());

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4) {
					Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);
					t1 = Size(r.width / 2, r.height / 2);
					//if ((r.x > p1.x - eps && r.x<p1.x + eps && r.y>p1.y - eps && r.y < p1.y + eps) || p1.x == 0) {
					p1.x = r.x + r.width / 2;
					p1.y = r.y + r.height / 2;
					bool1 = true;

					//}
				}
				else if (vtc == 5) {
					Image::setLabel(dst, "PENTA", contour);
				}
				else if (vtc == 6) {
					Image::setLabel(dst, "HEXA", contour);
					r = boundingRect(contour);
					t2 = Size(r.width / 2, r.height / 2);
					//if ((r.x > p2.x - eps && r.x<p2.x + eps && r.y>p2.y - eps && r.y < p2.y + eps) || p2.x == 0) {
					p2.x = r.x + r.width / 2;
					p2.y = r.y + r.height / 2;
					bool2 = true;

					//}
				}
			}
			else {
				// Detect and label circles
				double area = cv::contourArea(contour);
				cv::Rect r = cv::boundingRect(contour);
				int radius = r.width / 2;
				if (radius > 10) {
					if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2)
						Image::setLabel(dst, "CIR", contour);
					r = boundingRect(contour);
					t4 = Size(r.width / 2, r.height / 2);
					//if ((r.x > p4.x - eps && r.x<p4.x + eps && r.y>p4.y - eps && r.y < p4.y + eps) || p4.x==0) {
					p4.x = r.x + r.width / 2;
					p4.y = r.y + r.height / 2;
					bool4 = true;
				}


				// }
			}
		}



		// Draw game
		cv::imshow("dst", dst);

		if (bool1 && bool2 && bool3 &bool4) {
			
			p11 = p1;
			p12 = p2;
			p13 = p3;
			p14 = p4;

			/*p0.x = p1.x;
			p0.y = p1.y;
			objRef.push_back(p0);
			p0.x = p2.x;
			p0.y = p2.y;
			objRef.push_back(p0);
			p0.x = p3.x;
			p0.y = p3.y;
			objRef.push_back(p0);
			p0.x = p4.x;
			p0.y = p4.y;
			objRef.push_back(p0);*/


			zone2Jeu = dst(Rect(Point(p11.x + t1.width,p11.y + t1.height), Point(p13.x - t3.width, p13.y - t3.height)));
			cv::cvtColor(zone2Jeu, zone2Jeu_gray, CV_BGR2GRAY);
			GaussianBlur(zone2Jeu_gray, zone2Jeu_gauss, Size(3,3), 0, 0);
			Canny(zone2Jeu_gauss, zone2Jeu_canny, 100, 30, 3);



			cv::imshow("zonecanny", zone2Jeu_canny);
			// Use Canny instead of threshold to catch squares with gradient shading
			cv::imshow("zone", zone2Jeu);



			initialisation = true;
			cout<<"FIN INITIALISATION"<<endl;
		}

		
	}


	Mat zone;
	zone2Jeu.copyTo(zone);

	int iSliderValue1 = 20;
	int iSliderValue2 = 0;
	int iSliderValue3 = 40;

	int iSliderValue4 = 255;
	int iSliderValue5 = 12;

	int iSliderValue6 = 2;
	int iSliderValue7 = 3;
	int iSliderValue8 = 0.04;
	int iSliderValue9 = 200;

	namedWindow("My Window",WINDOW_AUTOSIZE);
	createTrackbar("lines theta", "My Window", &iSliderValue1, 255);
	createTrackbar("lines threshold", "My Window", &iSliderValue2, 255);
	createTrackbar("lines minLineLength", "My Window", &iSliderValue3, 255);
	createTrackbar("circles higher canny threshold", "My Window", &iSliderValue4, 255);
	createTrackbar("circles accumulator threshold", "My Window", &iSliderValue5, 255);
	createTrackbar("corners blockSize", "My Window", &iSliderValue6, 255);
	createTrackbar("corners apertureSize", "My Window", &iSliderValue7, 255);
	createTrackbar("corners k", "My Window", &iSliderValue8, 255);
	createTrackbar("corners normalisation thresh", "My Window", &iSliderValue9, 255);


	while (cvWaitKey(1) != 'q') {
		zone2Jeu.copyTo(zone);

		//	DETECTION WALL BY CANNY
		for (int row = 0; row < zone2Jeu_canny.rows; row++) {
			for (int col = 0; col < zone2Jeu_canny.cols; col++) {
				uchar color = zone2Jeu_canny.at<uchar>(row, col);
				if (color > 127) {
					circle(zone, Point(col, row), 1, Scalar(0, 255, 0));
				}
			}
		}


		imshow("zone", zone);

	}



    // Loop
    while (cvWaitKey(1) != 'q') {





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

		
		applyMask(bw, high, low, right, left, eps2).copyTo(bw);
        // Display canny image
        cv::imshow("bw", bw);


        // Find contours
        cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        src.copyTo(dst);

		//Haut

		

		bool1 = false;		
		bool2 = false;
		bool3 = false;
		bool4 = false;


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
				r = boundingRect(contour);

				//if ((r.x > p3.x - eps && r.x<p3.x + eps && r.y>p3.y - eps && r.y < p3.y + eps) || p3.x == 0) {
					p3.x = r.x + r.width / 2;
					p3.y = r.y + r.height / 2;
					bool3 = true;
				//}
            }
            else if (approx.size() >= 4 && approx.size() <= 6) {
                // Number of vertices of polygonal curve
                int vtc = static_cast<int>(approx.size());

                // Use the degrees obtained above and the number of vertices
                // to determine the shape of the contour
                if (vtc == 4) {
                    Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);
					//if ((r.x > p1.x - eps && r.x<p1.x + eps && r.y>p1.y - eps && r.y < p1.y + eps) || p1.x == 0) {
						p1.x = r.x + r.width / 2;
						p1.y = r.y + r.height / 2;
						bool1 = true;

					//}
				}
                else if (vtc == 5) {
                    Image::setLabel(dst, "PENTA", contour);
                }
                else if (vtc == 6) {
                    Image::setLabel(dst, "HEXA", contour);
					r = boundingRect(contour);
					//if ((r.x > p2.x - eps && r.x<p2.x + eps && r.y>p2.y - eps && r.y < p2.y + eps) || p2.x == 0) {
						p2.x = r.x + r.width / 2;
						p2.y = r.y + r.height / 2;
						bool2 = true;

					//}
                }
            }
            else {
                // Detect and label circles
                double area = cv::contourArea(contour);
                cv::Rect r = cv::boundingRect(contour);
                int radius = r.width / 2;

                if (std::abs(1 - ((double) r.width / r.height)) <= 0.2 &&
                    std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2)
                    Image::setLabel(dst, "CIR", contour);
					r = boundingRect(contour);
					//if ((r.x > p4.x - eps && r.x<p4.x + eps && r.y>p4.y - eps && r.y < p4.y + eps) || p4.x==0) {
						p4.x = r.x + r.width / 2;
						p4.y = r.y + r.height / 2;
						bool4 = true;

					// }
            }
        }

		line(dst, p11, p12, Scalar(0, 255, 0), 3);
		line(dst, p12, p13, Scalar(0, 255, 0), 3);
		line(dst, p13, p14, Scalar(0, 255, 0), 3);
		line(dst, p14, p11, Scalar(0, 255, 0), 3);

		// Draw game
		cv::imshow("dst", dst);

		if (bool1 && bool2 && bool3 &bool4) {
			




			if (p1.x > p11.x - eps && p1.x<p11.x + eps && p1.y>p11.y - eps && p1.y < p11.y + eps &&
				p2.x>p12.x - eps && p2.x<p12.x + eps && p2.y>p12.y - eps && p2.y < p12.y + eps &&
				p3.x>p13.x - eps && p3.x<p13.x + eps && p3.y>p13.y - eps && p3.y < p13.y + eps &&
				p4.x>p14.x - eps && p4.x<p14.x + eps && p4.y>p14.y - eps && p4.y < p14.y + eps) {
				compteur++;
			}
			else compteur = 0;
			if (compteur == 2) {
				p11 = p1;
				p12 = p2;
				p13 = p3;
				p14 = p4;

				objCapture.clear();
				objCapture.push_back(p11);
				objCapture.push_back(p12);
				objCapture.push_back(p13);
				objCapture.push_back(p14);
				compteur = 0;
			}
		}

		if (objCapture.size() == 4)
		{
			h = findHomography(objCapture, objRef, RANSAC);


			//cout << h << endl << endl;


			try {
				warpPerspective(toad, final, h, final.size());
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

void Game::stop() {

}