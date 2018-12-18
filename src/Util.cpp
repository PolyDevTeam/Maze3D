#include "Util.hpp"

using namespace std;
using namespace cv;

bool Util::squareIsBlack(Mat square) {
    int cptNoir = 0;

    for (int i = 0; i < square.rows; i++) {
        for (int j = 0; j < square.cols; j++) {
            if (square.at<Vec3b>(i, j)[0] < 150
                && square.at<Vec3b>(i, j)[1] < 150
                && square.at<Vec3b>(i, j)[3] < 150) {
                cptNoir++;
            }
        }
    }

    return cptNoir >= 0.60 * square.rows * square.cols;
}

double Util::dist(Point2i p1, Point2i p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Retourne le point dans vectRect le plus proche de target
Point2i Util::nearest(vector<Point2i> vecRect, Point2i target) {
    Point2i ref = vecRect[0];
    double distmin = dist(vecRect[0], target);

    for (size_t i = 1; i < vecRect.size(); i++) {
        if (dist(target, vecRect[i]) < distmin) {
            distmin = dist(target, vecRect[i]);
            ref = vecRect[i];
        }
    }

    return ref;
}