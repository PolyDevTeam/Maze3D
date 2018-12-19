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

int Util::highestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
    int higher = p11.y;

    if (p12.y < higher) higher = p12.y;
    if (p13.y < higher) higher = p13.y;
    if (p14.y < higher) higher = p14.y;
    return higher;
}

int Util::lowestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
    int low = p11.y;

    if (p12.y > low) low = p12.y;
    if (p13.y > low) low = p13.y;
    if (p14.y > low) low = p14.y;
    return low;
}

int Util::rightestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
    int right = p11.x;

    if (p12.x > right) right = p12.x;
    if (p13.x > right) right = p13.x;
    if (p14.x > right) right = p14.x;
    return right;
}

int Util::leftestPoint(Point2i p11, Point2i p12, Point2i p13, Point2i p14) {
    int left = p11.y;

    if (p12.x < left) left = p12.x;
    if (p13.x < left) left = p13.x;
    if (p14.x < left) left = p14.x;
    return left;
}

Mat Util::applyMask(Mat canny, int high, int low, int right, int left, int eps) {
    Mat dest;
    Mat mask = Mat::zeros(canny.rows, canny.cols, canny.type());
    rectangle(mask, Rect(left - eps, high - eps, right - left + 2 * eps, low - high + 2 * eps), Scalar(255, 255, 255),
              FILLED);
    bitwise_and(canny, mask, dest);
    return dest;
}