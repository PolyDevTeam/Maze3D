#ifndef __CAMERACALIBRATION_HPP__
#define __CAMERACALIBRATION_HPP__

#include "Settings.hpp"

typedef std::vector<std::vector<cv::Point2f>> ImagesPoints;
typedef std::vector<std::vector<cv::Point3f>> ObjectPoints;

using namespace cv;

class CameraCalibration {
public:
    static int launchCalibration();

private:
    static bool runCalibrationAndSave(Settings &s, cv::Size imageSize, cv::Mat &cameraMatrix, cv::Mat &distCoeffs,
                                      ImagesPoints imagePoints);

    static void calcBoardCornerPositions(Size boardSize, float squareSize, std::vector<Point3f> &corners,
                                         Settings::Pattern patternType /*= Settings::CHESSBOARD*/);

    static bool runCalibration(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                               ImagesPoints imagePoints, std::vector<Mat> &rvecs, std::vector<Mat> &tvecs,
                               std::vector<float> &reprojErrs, double &totalAvgErr);

    static void saveCameraParams(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                                 const std::vector<Mat> &rvecs, const std::vector<Mat> &tvecs,
                                 const std::vector<float> &reprojErrs, const ImagesPoints &imagePoints,
                                 double totalAvgErr);

    static double computeReprojectionErrors(const ObjectPoints &objectPoints,
                                            const ImagesPoints &imagePoints,
                                            const std::vector<Mat> &rvecs, const std::vector<Mat> &tvecs,
                                            const Mat &cameraMatrix, const Mat &distCoeffs,
                                            std::vector<float> &perViewErrors);
};

#endif /* __CAMERACALIBRATION_HPP__ */
