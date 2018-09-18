#include "Game.hpp"

//#if __APPLE__
//    #include <OpenGL/gl.h>
//#else
//    #include <GL/gl.h>
//#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "Image.hpp"

using namespace std;
using namespace cv;

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
	Rect r;
	std::vector<Point2i> objCapture;
	std::vector<Point2i> objRef;
	std::vector<Point2i> objRef2;
	Mat h;

    // Get WebCam
    VideoCapture capture(0);

	toad = imread("C:\\Users\\Stéphane\\Pictures\\bad_toad.png", 1);
	final = imread("C:\\Users\\Stéphane\\Pictures\\bad_toad.png", 1);






    // Loop
    while (cvWaitKey(30) != 'q') {
        // Refresh webcam image
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
				p3.x = r.x;
				p3.y = r.y;
            }
            else if (approx.size() >= 4 && approx.size() <= 6) {
                // Number of vertices of polygonal curve
                int vtc = static_cast<int>(approx.size());

                // Use the degrees obtained above and the number of vertices
                // to determine the shape of the contour
                if (vtc == 4) {
                    Image::setLabel(dst, "RECKT", contour);
					r = boundingRect(contour);
					p1.x = r.x;
					p1.y = r.y;
				}
                else if (vtc == 5) {
                    Image::setLabel(dst, "PENTA", contour);
                }
                else if (vtc == 6) {
                    Image::setLabel(dst, "HEXA", contour);
					r = boundingRect(contour);
					p2.x = r.x;
					p2.y = r.y;
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
					p4.x = r.x;
					p4.y = r.y;
            }
        }

		// Draw game
		cv::imshow("dst", dst);

		objCapture.push_back(p1);
		objCapture.push_back(p2);
		objCapture.push_back(p3);
		objCapture.push_back(p4);


		p0.x = 560;
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
		objRef.push_back(p0);

		h = findHomography(objCapture, objRef, RANSAC);
		cout << h<<endl<<endl;

		//perspectiveTransform(objRef, objRef2, h);

		warpPerspective(toad, final, h, final.size());

		//cv::imshow("final", final);
    }

    // Stop the game
    stop();
}

void Game::stop() {

}