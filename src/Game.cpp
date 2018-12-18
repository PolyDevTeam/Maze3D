#include <iostream>
#include <vector>

//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/opencv.hpp>

//#include <stdlib.h>
//#include <stdio.h>
//#include <random>
//#include <typeinfo>  //for 'typeid' to work

//#include "GL.hpp"
//#include "Bullet3D.hpp"

#include "Game.hpp"
#include "CameraCalibration.hpp"
#include "Util.hpp"

using namespace std;
using namespace cv;

typedef std::vector<std::vector<cv::Point>> Contours;

void Game::getCalibration() const {
    FileStorage fs("out_camera_data.xml", FileStorage::READ);

    if (!fs.isOpened()) {
        CameraCalibration::launchCalibration();
    } else {
        cout << "Voulez-vous relancer la calibration de la camera ? (Y/N)" << endl;

        string answer;
        getline(cin, answer);
        transform(answer.begin(), answer.end(), answer.begin(), ::tolower);

        if (answer == "y" || answer == "yes") {
            CameraCalibration::launchCalibration();
        }
    }

    int width;
    int height;
    Mat cameraMatrix;

    fs["opencv_storage"]["image_Width"] >> width;
    fs["opencv_storage"]["image_Height"] >> height;
    fs["opencv_storage"]["Camera_Matrix"] >> cameraMatrix;

    fs.release();
}

void Game::initialise() {
    // Get WebCam
    // 480 x 640
    VideoCapture capture(0);

    bool initialisation = false;
    quit = false;

    /*
     *	DEBUT INITIALISATION
     */
    while (!quit && !initialisation) {
        Image cam;
        Image dst; // TODO: Maybe remove this variable

        //	Initialisation
        capture >> cam;

        Image grayscale, canny;

        // Convert to grayscale
        cv::cvtColor(cam, grayscale, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        blur(grayscale, canny, Size(3, 3));
        cv::Canny(grayscale, canny, 80, 240, 3);

        //	Display canny image
        cv::imshow("Canny picture", canny);

        // Find contours
        Contours contours;
        cv::findContours(canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        cam.copyTo(dst);

        vector<Point2i> vectRect;
        Rect zone_jeu;

        for (auto &contour : contours) {
            // Approximate contour with accuracy proportional
            //	to the contour perimeter
            vector<cv::Point> approx;
            cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);

            // Skip small or non-convex objects
            if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
                continue;
            }

            if (approx.size() == 4) {
                //	Use the degrees obtained above and the number of vertices
                //	to determine the shape of the contour
                Image::setLabel(dst, "RECT", contour);

                // Récupération des coordonnées du centre du rectangle détecté
                Rect rect = boundingRect(contour);
                Point2i pRect;

                pRect.x = rect.x + rect.width / 2;
                pRect.y = rect.y + rect.height / 2;

                vectRect.push_back(pRect);

                if (!Util::squareIsBlack(cam(rect))) {
                    zone_jeu = rect;
                }
            }
        }

        // Affichage caméra avec label
        cv::imshow("Cam", dst);

        Rect boundingTriangle, boundingCircle;

        // Détection des 4 repéres et de la zone de jeu
        if (vectRect.size() >= 5) {
            contours.clear();

            vector<cv::Point> approx;

            // Find contours
            cv::findContours(canny.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

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
                    boundingTriangle = boundingRect(contour);
                } else if (approx.size() >= 4 && approx.size() <= 6) {
                    // Number of vertices of polygonal curve
                    int vtc = static_cast<int>(approx.size());

                    // Use the degrees obtained above and the number of vertices
                    // to determine the shape of the contour
                    if (vtc == 4) {
                        Image::setLabel(dst, "RECT", contour);
                    } else if (vtc == 5) {
                        Image::setLabel(dst, "PENTA", contour);
                    } else if (vtc == 6) {
                        Image::setLabel(dst, "HEXA", contour);
                    }
                } else {
                    // Detect and label circles
                    double area = cv::contourArea(contour);
                    cv::Rect r = cv::boundingRect(contour);
                    int radius = r.width / 2;

                    if (std::abs(1 - ((double) r.width / r.height)) <= 0.2
                        && std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2) {
                        Image::setLabel(dst, "CIR", contour);
                        boundingCircle = r;
                    }
                }
            }

            // affichage caméra avec label triangle + cercle
            imshow("tri-cercle", dst);

            // détection du triangle et du cercle
            if (boundingCircle.width != 0 && boundingTriangle.width != 0) {
                // on détermine les 4 rectangles servant de point de repère pour le labyrinthe puis on les sauvegarde
                Point2i pTopRight = Util::nearest(vectRect, Point2i(640, 0));
                Point2i pTopLeft = Util::nearest(vectRect, Point2i(0, 0));
                Point2i pBottomLeft = Util::nearest(vectRect, Point2i(0, 480));
                Point2i pBottomRight = Util::nearest(vectRect, Point2i(640, 480));

                vector<Point2i> objRef;
                objRef.push_back(pTopRight);
                objRef.push_back(pTopLeft);
                objRef.push_back(pBottomLeft);
                objRef.push_back(pBottomRight);

                // sauvegarde du point de depart et d'arrivee
                startPoint = boundingCircle.tl();
                finishPoint = boundingTriangle.tl();
                startPoint.x -= pTopLeft.x;
                startPoint.y -= pTopLeft.y;
                finishPoint.x -= pTopLeft.x;
                finishPoint.y -= pTopLeft.y;

                if (zone_jeu.width != 0) {
                    wall->update(canny, zone_jeu, boundingTriangle, boundingCircle);

                    // validation de l'initialisation
                    char bon = 'k';
                    while (bon != 'y' && bon != 'n' && bon != 'r' && bon != ' ' && bon != 'q') {
                        bon = (char) cv::waitKey(0);
                        if (bon == 'y' || bon == ' ')
                            initialisation = true;
                    }
                }
            }
        }

        //	vérifie si l'utilisateur veut quitter
        if (cvWaitKey(10) == 'q') {
            quit = true;
        }
    }
}

void Game::run() {
//	int eps = 100;
//	int eps2 = 50;
//	int eps3 = 100;
//	vector<Mat> Rs_decomp, ts_decomp, normals_decomp;
//	int solutions;
//	Mat h;
//	vector<Point2i> vectRect;
//	Mat final;
//	vector<Point2i> objCapture;

    // INIT PHYSIQUE
    btVector3 gravity(0, 0, -6000);

    // Creer un Monde physique
    world->create(gravity);

    // Creer une Balle Physique
    btVector3 ballOrigin(startPoint.x - wall->getPoints().cols / 2.0f, -startPoint.y + wall->getPoints().rows / 2.0f,
                         100);

    ball = new Ball(BALL_RADIUS);
    ball->createPhysics(ballOrigin, BALL_MASS, world);

    // Creer un sol physique
    btVector3 origineGround(0, 0, -50);
    ground->createPhysics(wall->getPoints(), origineGround, world);

    // Creer les murs physique
    btVector3 pillardDims(PILLAR_WIDTH, PILLAR_WIDTH, PILLAR_HEIGHT);
    wall->createPhysics(pillardDims, 0, world);

    // Loop
    while (!quit) {
//        if (cvWaitKey(10) == 'r')
//            initialisationDetection(capture, p11, p12, p13, p14);
//
//        // Refresh webcam image
//        capture >> src;
//
//        // Convert to grayscale
//        cv::cvtColor(src, gray, CV_BGR2GRAY);
//
//        // Use Canny instead of threshold to catch squares with gradient shading
//        blur(gray, gray, Size(3, 3));
//        cv::Canny(gray, src_canny, 80, 240, 3);
//
//        int high = highestPoint(p11, p12, p13, p14);
//        int low = lowestPoint(p11, p12, p13, p14);
//        int right = rightestPoint(p11, p12, p13, p14);
//        int left = leftestPoint(p11, p12, p13, p14);
//
//        eps2=0.20*dist(p11, p12);
//        applyMask(src_canny, high, low, right, left, eps2).copyTo(src_canny);
//        // Display canny image
//        cv::imshow("src_canny", src_canny);
//
//        // Find contours
//        cv::findContours(src_canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//
//        src.copyTo(dst);
//
//        vectRect.clear();
//
//        for (auto &contour : contours) {
//
//            // Approximate contour with accuracy proportional
//            // to the contour perimeter
//            cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
//
//            // Skip small or non-convex objects
//            if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) {
//                continue;
//            }
//
//            if (approx.size() >= 4 && approx.size() <= 6) {
//                // Number of vertices of polygonal curve
//                int vtc = static_cast<int>(approx.size());
//
//                // Use the degrees obtained above and the number of vertices
//                // to determine the shape of the contour
//                if (vtc == 4) {
//                    Image::setLabel(dst, "RECKT", contour);
//                    r = boundingRect(contour);
//
//                    pRect.x = r.x + r.width / 2;
//                    pRect.y = r.y + r.height / 2;
//                    vectRect.push_back(pRect);
//                }
//            }
//        }
//
//        cv::line(dst, p11, p12, Scalar(0, 255, 0), 3);
//        cv::line(dst, p12, p13, Scalar(0, 255, 0), 3);
//        cv::line(dst, p13, p14, Scalar(0, 255, 0), 3);
//        cv::line(dst, p14, p11, Scalar(0, 255, 0), 3);
//
//        // Draw game
//        cv::imshow("dst", dst);
//
//        if (vectRect.size() >= 4) {
//
//            p1 = nearest(vectRect, p11);
//            p2 = nearest(vectRect, p12);
//            p3 = nearest(vectRect, p13);
//            p4 = nearest(vectRect, p14);
//            //cout << "p1 : " << p1 << " " << p2 << " " << p3 << " " << p4 << endl;
//
//            circle(dst, p1, 2, Scalar(0, 0, 255), 10);
//            circle(dst, p2, 2, Scalar(0, 0, 255), 10);
//            circle(dst, p3, 2, Scalar(0, 0, 255), 10);
//            circle(dst, p4, 2, Scalar(0, 0, 255), 10);
//
//            // Draw game
//            cv::imshow("dst", dst);
//            objCapture.clear();
//            if (dist(p1, p11) < eps3 && dist(p2, p12) < eps3 && dist(p3, p13) < eps3 && dist(p4, p14) < eps3) {
//                objCapture.push_back(p1);
//                objCapture.push_back(p2);
//                objCapture.push_back(p3);
//                objCapture.push_back(p4);
//                p11 = p1;
//                p12 = p2;
//                p13 = p3;
//                p14 = p4;
//
//                eps3 = 0.25*dist(p1, p2);
//            }
//
//        }
//
//        if (objCapture.size() == 4)
//        {
//            //cout << "YES" << endl;
//            h = findHomography(objCapture, objRef, RANSAC);
//            //cout << h << endl << endl;
//
//            solutions = decomposeHomographyMat(h, cameraMatrix, Rs_decomp, ts_decomp, normals_decomp);
//            //cout << "Decompose homography matrix estimated by findHomography():" << endl << endl;
//            Mat rvec_decomp1;
//            Mat rvec_decomp2;
//            Mat rvec_decomp;
//
//            for (int i = 0; i < solutions; i++)
//            {
//                Rodrigues(Rs_decomp[i], rvec_decomp);
//                //cout << "Solution " << i << ":" << endl;
//                //cout << "rvec from homography decomposition: " << rvec_decomp.t() << endl;
//            }
//
//            Rodrigues(Rs_decomp[0], rvec_decomp1);
//            Rodrigues(Rs_decomp[2], rvec_decomp2);
//            rvec_decomp = rvec_decomp1;
//
//            for (int i = 0;i < 3;i++) {
//                if (abs(rvec_decomp2.at<double>(i, 0)) > abs(rvec_decomp1.at<double>(i, 0))) {
//                    rvec_decomp.at<double>(i, 0) = rvec_decomp2.at<double>(i, 0);
//                }
//            }
//            //cout << rvec_decomp << endl;
//
//
//            try {
//                warpPerspective(wallPoints, final, h, final.size());
//            }
//            catch (std::exception const& e)
//            {
//                cerr << "ERREUR : " << e.what() << endl;
//            }
//
//            cv::imshow("final", final);
//
//
//            draw_GL(win, rvec_decomp, wallPoints, start, finish, world, wall);
//        }
//
//        //vérifie si l'utilisateur veut quitter
//        if (cvWaitKey(1) == 'q')
//            quit = true;
    }
//
//    // Stop the game
//    stop();
}

void Game::start(int argc, char **argv) {
    getCalibration();

    glWindows->init();

    initialise();

    run();
}

/*

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

int Game::random(int min, int max) {
	default_random_engine gen;
	uniform_int_distribution<int> interval(min, max);
	return interval(gen);
}

Mat Game::getWallsMat(Mat src, Rect zone, Rect rt, Rect rc) {
	Mat zone2Jeu_canny, src_canny;

	src.copyTo(src_canny);

	//on retire de la zone de jeu le triangle et le cercle mat�rialisants le d�part et l'arriv�e
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
*/
