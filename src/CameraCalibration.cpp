#include <iostream>

#include "CameraCalibration.hpp"
#include "Settings.hpp"

using namespace std;
using namespace cv;

enum {
    DETECTION = 0, CAPTURING = 1, CALIBRATED = 2
};

const char ESC_KEY = 27;
const Scalar RED(0, 0, 255), GREEN(0, 255, 0);

static void help() {
    cout << "This is a camera calibration sample." << endl
         << "Usage: calibration configurationFile" << endl
         << "Near the sample file you'll find the configuration file, which has detailed help of "
            "how to edit it.  It may be any OpenCV supported file format XML/YAML." << endl;
}

static void read(const FileNode &node, Settings &x, const Settings &default_value = Settings()) {
    if (node.empty())
        x = default_value;
    else
        x.read(node);
}

int CameraCalibration::launchCalibration() {
    help();

    Settings s;

    const string inputSettingsFile = "settingCalibration.xml";
    FileStorage fs(inputSettingsFile, FileStorage::READ); // Read the settings

    if (!fs.isOpened()) {
        cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
        return -1;
    }

    fs["Settings"] >> s;

    fs.release(); // close Settings file

    if (!s.hasGoodInput()) {
        cout << "Invalid input detected. Application stopping. " << endl;
        return -1;
    }

    Mat cameraMatrix, distCoeffs;
    Size imageSize;

    ImagesPoints imagePoints;
    int mode = s.getInputType() == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
    clock_t prevTimestamp = 0;

    for (int i = 0;; ++i) {
        Mat view = s.nextImage();

        //-----  If no more image, or got enough, then stop calibration and show result -------------
        if (mode == CAPTURING && imagePoints.size() >= (unsigned) s.getNbFrames()) {
            if (runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints)) {
                mode = CALIBRATED;
            } else {
                mode = DETECTION;
            }
        }

        if (view.empty()) {         // If no more images then run calibration, save and stop loop.
            if (!imagePoints.empty()) {
                runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints);
            }
            break;
        }

        imageSize = view.size();  // Format input image.

        if (s.isVerticalyFlipped()) {
            flip(view, view, 0);
        }

        vector<Point2f> pointBuf;
        bool found;
        switch (s.getCalibrationPattern()) { // Find feature points on the input format
            case Settings::CHESSBOARD:
                found = findChessboardCorners(view, s.getBoardSize(), pointBuf,
                                              CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK |
                                              CV_CALIB_CB_NORMALIZE_IMAGE);
                break;
            case Settings::CIRCLES_GRID:
                found = findCirclesGrid(view, s.getBoardSize(), pointBuf);
                break;
            case Settings::ASYMMETRIC_CIRCLES_GRID:
                found = findCirclesGrid(view, s.getBoardSize(), pointBuf, CALIB_CB_ASYMMETRIC_GRID);
                break;
            default:
                found = false;
                break;
        }

        bool blinkOutput = false;

        if (found) { // If done with success,
            // improve the found corners' coordinate accuracy for chessboard
            if (s.getCalibrationPattern() == Settings::CHESSBOARD) {
                Mat viewGray;
                cvtColor(view, viewGray, COLOR_BGR2GRAY);
                cornerSubPix(viewGray, pointBuf, Size(11, 11),
                             Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            }

            if (mode == CAPTURING &&  // For camera only take new samples after delay time
                (!s.getVideoCapture().isOpened() || clock() - prevTimestamp > s.getDelay() * 1e-3 * CLOCKS_PER_SEC)) {
                imagePoints.push_back(pointBuf);
                prevTimestamp = clock();
                blinkOutput = s.getVideoCapture().isOpened();
            }

            // Draw the corners.
            drawChessboardCorners(view, s.getBoardSize(), Mat(pointBuf), found);
        }

        //----------------------------- Output Text ------------------------------------------------
        string msg = (mode == CAPTURING) ? "100/100" :
                     mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
        int baseLine = 0;
        Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
        Point textOrigin(view.cols - 2 * textSize.width - 10, view.rows - 2 * baseLine - 10);

        if (mode == CAPTURING) {
            if (s.isShowDistorsed()) {
                msg = format("%d/%d Undist", (int) imagePoints.size(), s.getNbFrames());
            } else {
                msg = format("%d/%d", (int) imagePoints.size(), s.getNbFrames());
            }

            putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);

            if (blinkOutput) {
                bitwise_not(view, view);
            }

            //------------------------- Video capture  output  undistorted ------------------------------
            if (mode == CALIBRATED && s.isShowDistorsed()) {
                Mat temp = view.clone();
                undistort(temp, view, cameraMatrix, distCoeffs);
            }

            //------------------------------ Show image and check for input commands -------------------
            imshow("Image View", view);
            char key = (char) waitKey(s.getVideoCapture().isOpened() ? 50 : s.getDelay());

            if (key == ESC_KEY) {
                break;
            }

            if (key == 'u' && mode == CALIBRATED) {
                s.inverseShowDistorsed();
            }

            if (s.getVideoCapture().isOpened() && key == 'g') {
                mode = CAPTURING;
                imagePoints.clear();
            }
        }
    }
    // -----------------------Show the undistorted image for the image list ------------------------
    if (s.getInputType() == Settings::IMAGE_LIST && s.isShowDistorsed()) {
        Mat view, rview, map1, map2;
        initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
                                getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
                                imageSize, CV_16SC2, map1, map2);

        for (int i = 0; i < (int) s.getImageList().size(); i++) {
            view = imread(s.getImageList()[i], 1);
            if (view.empty())
                continue;
            remap(view, rview, map1, map2, INTER_LINEAR);
            imshow("Image View", rview);
            char c = (char) waitKey();
            if (c == ESC_KEY || c == 'q' || c == 'Q') {
                break;
            }
        }
    }

    return 0;
}

bool CameraCalibration::runCalibration(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                                       vector<vector<Point2f> > imagePoints, vector<Mat> &rvecs, vector<Mat> &tvecs,
                                       vector<float> &reprojErrs, double &totalAvgErr) {
    cameraMatrix = Mat::eye(3, 3, CV_64F);

    if (s.getFlag() & CV_CALIB_FIX_ASPECT_RATIO) {
        cameraMatrix.at<double>(0, 0) = 1.0;
    }

    distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);
    calcBoardCornerPositions(s.getBoardSize(), s.getSquareSize(), objectPoints[0], s.getCalibrationPattern());

    objectPoints.resize(imagePoints.size(), objectPoints[0]);

    //Find intrinsic and extrinsic camera parameters
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                 distCoeffs, rvecs, tvecs, s.getFlag() | CV_CALIB_FIX_K4 | CV_CALIB_FIX_K5);

    cout << "Re-projection error reported by calibrateCamera: " << rms << endl;

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                            rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}

// Print camera parameters to the output file
void CameraCalibration::saveCameraParams(Settings &s, Size &imageSize, Mat &cameraMatrix, Mat &distCoeffs,
                                         const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                         const vector<float> &reprojErrs, const vector<vector<Point2f> > &imagePoints,
                                         double totalAvgErr) {
    FileStorage fs(s.getOutputFileName(), FileStorage::WRITE);

    time_t tm;
    time(&tm);
    struct tm *t2 = localtime(&tm);
    char buf[1024];
    strftime(buf, sizeof(buf) - 1, "%c", t2);

    fs << "calibration_Time" << buf;

    if (!rvecs.empty() || !reprojErrs.empty()) {
        fs << "nrOfFrames" << (int) std::max(rvecs.size(), reprojErrs.size());
    }

    fs << "image_Width" << imageSize.width;
    fs << "image_Height" << imageSize.height;
    fs << "board_Width" << s.getBoardSize().width;
    fs << "board_Height" << s.getBoardSize().height;
    fs << "square_Size" << s.getSquareSize();

    if (s.getFlag() & CV_CALIB_FIX_ASPECT_RATIO) {
        fs << "FixAspectRatio" << s.getAspectRatio();
    }

    if (s.getFlag()) {
        sprintf(buf, "flags: %s%s%s%s",
                s.getFlag() & CV_CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "",
                s.getFlag() & CV_CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "",
                s.getFlag() & CV_CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "",
                s.getFlag() & CV_CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "");
        cvWriteComment(*fs, buf, 0);
    }

    fs << "flagValue" << s.getFlag();

    fs << "Camera_Matrix" << cameraMatrix;
    fs << "Distortion_Coefficients" << distCoeffs;

    fs << "Avg_Reprojection_Error" << totalAvgErr;
    if (!reprojErrs.empty())
        fs << "Per_View_Reprojection_Errors" << Mat(reprojErrs);

    if (!rvecs.empty() && !tvecs.empty()) {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int) rvecs.size(), 6, rvecs[0].type());
        for (int i = 0; i < (int) rvecs.size(); i++) {
            Mat r = bigmat(Range(i, i + 1), Range(0, 3));
            Mat t = bigmat(Range(i, i + 1), Range(3, 6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment(*fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0);
        fs << "Extrinsic_Parameters" << bigmat;
    }

    if (!imagePoints.empty()) {
        Mat imagePtMat((int) imagePoints.size(), (int) imagePoints[0].size(), CV_32FC2);
        for (int i = 0; i < (int) imagePoints.size(); i++) {
            Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "Image_points" << imagePtMat;
    }
}

bool CameraCalibration::runCalibrationAndSave(Settings &s, cv::Size imageSize, cv::Mat &cameraMatrix,
                                              cv::Mat &distCoeffs, ImagesPoints imagePoints) {
    vector<Mat> rvecs, tvecs;
    vector<float> reprojErrs;
    double totalAvgErr = 0;

    bool ok = runCalibration(s, imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs,
                             reprojErrs, totalAvgErr);

    cout << (ok ? "Calibration succeeded" : "Calibration failed")
         << ". avg re projection error = " << totalAvgErr;

    if (ok) {
        saveCameraParams(s, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, reprojErrs,
                         imagePoints, totalAvgErr);
    }

    return ok;
}

void CameraCalibration::calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f> &corners,
                                                 Settings::Pattern patternType /*= Settings::CHESSBOARD*/) {
    corners.clear();

    switch (patternType) {
        case Settings::CHESSBOARD:
        case Settings::CIRCLES_GRID:
            for (int i = 0; i < boardSize.height; ++i) {
                for (int j = 0; j < boardSize.width; ++j) {
                    corners.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
                }
            }
            break;
        case Settings::ASYMMETRIC_CIRCLES_GRID:
            for (int i = 0; i < boardSize.height; i++) {
                for (int j = 0; j < boardSize.width; j++) {
                    corners.push_back(Point3f(float((2 * j + i % 2) * squareSize), float(i * squareSize), 0));
                }
            }
            break;
        default:
            break;
    }
}

double CameraCalibration::computeReprojectionErrors(const vector<vector<Point3f> > &objectPoints,
                                                    const vector<vector<Point2f> > &imagePoints,
                                                    const vector<Mat> &rvecs, const vector<Mat> &tvecs,
                                                    const Mat &cameraMatrix, const Mat &distCoeffs,
                                                    vector<float> &perViewErrors) {
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for (i = 0; i < (int) objectPoints.size(); ++i) {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                      distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);

        int n = (int) objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err * err / n);
        totalErr += err * err;
        totalPoints += n;
    }

    return std::sqrt(totalErr / totalPoints);
}