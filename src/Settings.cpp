#include "Settings.hpp"

using namespace cv;
using namespace std;

Settings::Settings() : goodInput(false) {

}

bool Settings::hasGoodInput() const {
    return this->goodInput;
}

Settings::InputType Settings::getInputType() const {
    return this->inputType;
}

Mat Settings::nextImage() {
    Mat result;
    if (inputCapture.isOpened()) {
        Mat view0;
        inputCapture >> view0;
        view0.copyTo(result);
    } else if (atImageList < (int) imageList.size()) {
        result = imread(imageList[atImageList++], CV_LOAD_IMAGE_COLOR);
    }

    return result;
}

int Settings::getNbFrames() const {
    return this->nbFrames;
}

bool Settings::isVerticalyFlipped() const {
    return this->flipVertical;
}

Settings::Pattern Settings::getCalibrationPattern() const {
    return this->calibrationPattern;
}

cv::Size Settings::getBoardSize() const {
    return this->boardSize;
}

cv::VideoCapture Settings::getVideoCapture() const {
    return this->inputCapture;
}

int Settings::getDelay() const {
    return this->delay;
}

bool Settings::isShowDistorsed() const {
    return this->showUndistorsed;
}

void Settings::inverseShowDistorsed() {
    this->showUndistorsed = !this->showUndistorsed;
}

std::vector<std::string> Settings::getImageList() const {
    return this->imageList;
}

int Settings::getFlag() const {
    return this->flag;
}

float Settings::getSquareSize() const {
    return this->squareSize;
}

std::string Settings::getOutputFileName() const {
    return this->outputFileName;
}

float Settings::getAspectRatio() const {
    return this->aspectRatio;
}

void Settings::read(const FileNode &node) { //Read serialization for this class
    node["BoardSize_Width"] >> boardSize.width;
    node["BoardSize_Height"] >> boardSize.height;
    node["Calibrate_Pattern"] >> patternToUse;
    node["Square_Size"] >> squareSize;
    node["Calibrate_NrOfFrameToUse"] >> nbFrames;
    node["Calibrate_FixAspectRatio"] >> aspectRatio;
    node["Write_DetectedFeaturePoints"] >> bwritePoints;
    node["Write_extrinsicParameters"] >> bwriteExtrinsics;
    node["Write_outputFileName"] >> outputFileName;
    node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
    node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
    node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
    node["Show_UndistortedImage"] >> showUndistorsed;
    node["Input"] >> input;
    node["Input_Delay"] >> delay;
    interprate();
}

void Settings::interprate() {
    goodInput = true;
    if (boardSize.width <= 0 || boardSize.height <= 0) {
        cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
        goodInput = false;
    }

    if (squareSize <= 10e-6) {
        cerr << "Invalid square size " << squareSize << endl;
        goodInput = false;
    }

    if (nbFrames <= 0) {
        cerr << "Invalid number of frames " << nbFrames << endl;
        goodInput = false;
    }

    if (input.empty()) { // Check for valid input
        inputType = INVALID;
    } else {
        if (input[0] >= '0' && input[0] <= '9') {
            stringstream ss(input);
            ss >> cameraID;
            inputType = CAMERA;
        } else {
            if (isListOfImages(input) && readStringList(input, imageList)) {
                inputType = IMAGE_LIST;
                nbFrames = (nbFrames < (int) imageList.size()) ? nbFrames : (int) imageList.size();
            } else {
                inputType = VIDEO_FILE;
            }
        }

        if (inputType == CAMERA) {
            inputCapture.open(cameraID);
        }

        if (inputType == VIDEO_FILE) {
            inputCapture.open(input);
            inputCapture.open(input);
        }
        if (inputType != IMAGE_LIST && !inputCapture.isOpened()) {
            inputType = INVALID;
        }
    }
    if (inputType == INVALID) {
        cerr << " Inexistent input: " << input;
        goodInput = false;
    }

    flag = 0;
    if (calibFixPrincipalPoint) {
        flag |= CV_CALIB_FIX_PRINCIPAL_POINT;
    }

    if (calibZeroTangentDist) {
        flag |= CV_CALIB_ZERO_TANGENT_DIST;
    }

    if (aspectRatio) {
        flag |= CV_CALIB_FIX_ASPECT_RATIO;
    }


    calibrationPattern = NOT_EXISTING;
    if (!patternToUse.compare("CHESSBOARD")) {
        calibrationPattern = CHESSBOARD;
    }

    if (!patternToUse.compare("CIRCLES_GRID")) {
        calibrationPattern = CIRCLES_GRID;
    }

    if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) {
        calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
    }

    if (calibrationPattern == NOT_EXISTING) {
        cerr << " Inexistent camera calibration mode: " << patternToUse << endl;
        goodInput = false;
    }
    atImageList = 0;
}

bool Settings::readStringList(const string &filename, vector<string> &l) {
    l.clear();
    FileStorage fs(filename, FileStorage::READ);
    if (!fs.isOpened()) {
        return false;
    }

    FileNode n = fs.getFirstTopLevelNode();

    if (n.type() != FileNode::SEQ) {
        return false;
    }

    FileNodeIterator it = n.begin(), it_end = n.end();

    for (; it != it_end; ++it) {
        l.push_back((string) *it);
    }

    return true;
}

bool Settings::isListOfImages(const string &filename) {
    string s(filename);
    // Look for file extension
    if (s.find(".xml") == string::npos && s.find(".yaml") == string::npos && s.find(".yml") == string::npos) {
        return false;
    } else {
        return true;
    }
}