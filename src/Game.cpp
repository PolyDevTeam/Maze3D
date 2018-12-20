#include <iostream>
#include <vector>

#include "Game.hpp"
#include "CameraCalibration.hpp"
#include "Util.hpp"

using namespace std;
using namespace cv;

typedef std::vector<std::vector<cv::Point>> Contours;

cv::Mat cameraMatrix;

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
        Image dst;

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
                }
                else {
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
                pTopRight = Util::nearest(vectRect, Point2i(640, 0));
                pTopLeft = Util::nearest(vectRect, Point2i(0, 0));
                pBottomLeft = Util::nearest(vectRect, Point2i(0, 480));
                pBottomRight = Util::nearest(vectRect, Point2i(640, 480));

                objRef.clear();
                objRef.push_back(pTopRight);
                objRef.push_back(pTopLeft);
                objRef.push_back(pBottomLeft);
                objRef.push_back(pBottomRight);

                // sauvegarde du point de depart et d'arrivee
                map->start(boundingCircle.tl());
                map->finish(boundingTriangle.tl());
                map->start().x -= pTopLeft.x;
                map->start().y -= pTopLeft.y;
                map->finish().x -= pTopLeft.x;
                map->finish().y -= pTopLeft.y;

                if (zone_jeu.width != 0) {
                    map->wall()->update(canny, zone_jeu, boundingTriangle, boundingCircle);

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

        // vérifie si l'utilisateur veut quitter
        if (cvWaitKey(10) == 'q') {
            quit = true;
        }
    }
}

void Game::run() {
    VideoCapture capture(0);

    int eps3 = 100;

    // INIT PHYSIQUE
    map->createPhysics();

    // Loop
    while (!quit) {
        if (cvWaitKey(10) == 'r') {
            resetDetection(capture);
        }

        Image cam;
        // Refresh webcam image
        capture >> cam;

        Image grayscale, canny;
        // Convert to grayscale
        cv::cvtColor(cam, grayscale, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        blur(grayscale, grayscale, Size(3, 3));
        cv::Canny(grayscale, canny, 80, 240, 3);

        int high = Util::highestPoint(pTopRight, pTopLeft, pBottomLeft, pBottomRight);
        int low = Util::lowestPoint(pTopRight, pTopLeft, pBottomLeft, pBottomRight);
        int right = Util::rightestPoint(pTopRight, pTopLeft, pBottomLeft, pBottomRight);
        int left = Util::leftestPoint(pTopRight, pTopLeft, pBottomLeft, pBottomRight);

        int eps2 = static_cast<int>((int) 0.20 * Util::dist(pTopRight, pTopLeft));
        Util::applyMask(canny, high, low, right, left, eps2).copyTo(canny);

        // Display canny image
        cv::imshow("src_canny", canny);

        // Find contours
        Contours contours;
        cv::findContours(canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        Image dst;
        cam.copyTo(dst);

	    vector<Point2i> vectRect;

        for (auto &contour : contours) {
            // Approximate contour with accuracy proportional
            // to the contour perimeter
            vector<cv::Point> approx;
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
                    Image::setLabel(dst, "RECT", contour);
                    Point2i pRect;
                    cv::Rect rect = boundingRect(contour);

                    pRect.x = rect.x + rect.width / 2;
                    pRect.y = rect.y + rect.height / 2;
                    vectRect.push_back(pRect);
                }
            }
        }

        cv::line(dst, pTopRight, pTopLeft, Scalar(0, 255, 0), 3);
        cv::line(dst, pTopLeft, pBottomLeft, Scalar(0, 255, 0), 3);
        cv::line(dst, pBottomLeft, pBottomRight, Scalar(0, 255, 0), 3);
        cv::line(dst, pBottomRight, pTopRight, Scalar(0, 255, 0), 3);

        // Draw game
        cv::imshow("dst", dst);

        vector<Point2i> objCapture;
        if (vectRect.size() >= 4) {
            cv::Point2i top_right = Util::nearest(vectRect, pTopRight);
            cv::Point2i top_left = Util::nearest(vectRect, pTopLeft);
            cv::Point2i bottom_left = Util::nearest(vectRect, pBottomLeft);
            cv::Point2i bottom_right = Util::nearest(vectRect, pBottomRight);

            //cout << "p1 : " << p1 << " " << p2 << " " << p3 << " " << p4 << endl;

            circle(dst, top_right, 2, Scalar(0, 0, 255), 10);
            circle(dst, top_left, 2, Scalar(0, 0, 255), 10);
            circle(dst, bottom_left, 2, Scalar(0, 0, 255), 10);
            circle(dst, bottom_right, 2, Scalar(0, 0, 255), 10);

            // Draw game
            cv::imshow("dst", dst);
            objCapture.clear();

            if (Util::dist(top_right, pTopRight) < eps3
            && Util::dist(top_left, pTopLeft) < eps3
            && Util::dist(bottom_left, pBottomLeft) < eps3
            && Util::dist(bottom_right, pBottomRight) < eps3) {
                objCapture.push_back(top_right);
                objCapture.push_back(top_left);
                objCapture.push_back(bottom_left);
                objCapture.push_back(bottom_right);

                pTopRight = top_right;
                pTopLeft = top_left;
                pBottomLeft = bottom_left;
                pBottomRight = bottom_right;

                eps3 = static_cast<int>(0.25 * Util::dist(top_right, top_left));
            }
        }

        if (objCapture.size() == 4) {
            //cout << "YES" << endl;
            Mat homographyMat = findHomography(objCapture, objRef, RANSAC);
            //cout << h << endl << endl;

            vector<Mat> Rs_decomp, ts_decomp, normals_decomp;
            int solutions = decomposeHomographyMat(homographyMat, cameraMatrix, Rs_decomp, ts_decomp, normals_decomp);
            //cout << "Decompose homography matrix estimated by findHomography():" << endl << endl;


            Mat rvec_decomp;
            for (int i = 0; i < solutions; i++) {
                Rodrigues(Rs_decomp[i], rvec_decomp);
                //cout << "Solution " << i << ":" << endl;
                //cout << "rvec from homography decomposition: " << rvec_decomp.t() << endl;
            }

            Mat rvec_decomp1;
            Mat rvec_decomp2;
            Rodrigues(Rs_decomp[0], rvec_decomp1);
            Rodrigues(Rs_decomp[2], rvec_decomp2);
            rvec_decomp = rvec_decomp1;

            for (int i = 0;i < 3;i++) {
                if (abs(rvec_decomp2.at<double>(i, 0)) > abs(rvec_decomp1.at<double>(i, 0))) {
                    rvec_decomp.at<double>(i, 0) = rvec_decomp2.at<double>(i, 0);
                }
            }

            Mat final;
            try {
                warpPerspective(map->wall()->getPoints(), final, homographyMat, final.size());
            }
            catch (std::exception const& e) {
                cerr << "ERREUR : " << e.what() << endl;
            }

            cv::imshow("final", final);

            map->draw(glWindows, rvec_decomp);
        }

        //vérifie si l'utilisateur veut quitter
        if (cvWaitKey(1) == 'q') {
            quit = true;
        }
    }
}

void Game::start(int argc, char **argv) {
    getCalibration();

    glWindows->init();

//    initialise();
//
//    run();
}

void Game::resetDetection(VideoCapture capture) {
    cout << "Reinitialisation" << endl;

    bool initialisation = false;

    while (cvWaitKey(10) != 'q' && initialisation == false) {
        // initialisation
        Image cam;
        capture >> cam;

        // Convert to grayscale
        Image grayscale;
        cv::cvtColor(cam, grayscale, CV_BGR2GRAY);

        // Use Canny instead of threshold to catch squares with gradient shading
        Image bw;
        blur(grayscale, bw, Size(3, 3));
        cv::Canny(grayscale, bw, 80, 240, 3);

        // Find contours
        Contours contours;
        cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        Image dst;
        cam.copyTo(dst);

        vector<Point2i> vectRect;

        for (auto &contour : contours) {
            // Approximate contour with accuracy proportional
            // to the contour perimeter
            vector<cv::Point> approx;
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
                    Image::setLabel(dst, "RECT", contour);
                    Rect rect = boundingRect(contour);

                    Point2i pRect;
                    pRect.x = rect.x + rect.width / 2;
                    pRect.y = rect.y + rect.height / 2;
                    vectRect.push_back(pRect);
                }
            }
        }

        if (vectRect.size() >= 4) {
            pTopRight = Util::nearest(vectRect, Point2i(640, 0));
            pTopLeft = Util::nearest(vectRect, Point2i(0, 0));
            pBottomLeft = Util::nearest(vectRect, Point2i(0, 480));
            pBottomRight = Util::nearest(vectRect, Point2i(640, 480));

            initialisation = true;
            cout << "FIN REINITIALISATION" << endl;
        }
    }
}