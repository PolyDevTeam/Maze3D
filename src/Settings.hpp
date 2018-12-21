#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include <opencv2/opencv.hpp>

class Settings {
public:
    enum InputType {
        INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST
    };
    enum Pattern {
        NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID
    };

    Settings();

    bool hasGoodInput() const;

    InputType getInputType() const;

    cv::Mat nextImage();

    int getNbFrames() const;

    bool isVerticalyFlipped() const;

    Pattern getCalibrationPattern() const;

    cv::Size getBoardSize() const;

    cv::VideoCapture getVideoCapture() const;

    int getDelay() const;

    bool isShowDistorsed() const;

    void inverseShowDistorsed();

    std::vector<std::string> getImageList() const;

    int getFlag() const;

    float getSquareSize() const;

    std::string getOutputFileName() const;

    float getAspectRatio() const;

    void read(const cv::FileNode &node);

private:
    void interprate();

    static bool isListOfImages(const std::string &filename);

    static bool readStringList(const std::string &filename, std::vector<std::string> &l);

    float aspectRatio;         // The aspect ratio
    InputType inputType;
    bool goodInput;
    int nbFrames;              // The number of frames to use from the input for calibration
    bool flipVertical;         // Flip the captured images around the horizontal axis
    Pattern calibrationPattern;// One of the Chessboard, circles, or asymmetric circle pattern
    cv::Size boardSize;        // The size of the board -> Number of items by width and height
    cv::VideoCapture inputCapture;
    int delay;                 // In case of a video input
    bool showUndistorsed;      // Show undistorted images after calibration
    std::vector<std::string> imageList;
    int flag;
    float squareSize;          // The size of a square in your defined unit (point, millimeter,etc).
    std::string outputFileName;// The name of the file where to write
    std::string patternToUse;
    bool bwritePoints;         //  Write detected feature points
    bool bwriteExtrinsics;     // Write extrinsic parameters
    bool calibZeroTangentDist; // Assume zero tangential distortion
    bool calibFixPrincipalPoint;// Fix the principal point at the center
    std::string input;          // The input ->
    int cameraID;
    int atImageList;
};

//    void write(FileStorage& fs) const                        //Write serialization for this class
//    {
//        fs << "{" << "BoardSize_Width"  << boardSize.width
//                  << "BoardSize_Height" << boardSize.height
//                  << "Square_Size"         << squareSize
//                  << "Calibrate_Pattern" << patternToUse
//                  << "Calibrate_NrOfFrameToUse" << nrFrames
//                  << "Calibrate_FixAspectRatio" << aspectRatio
//                  << "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
//                  << "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint
//
//                  << "Write_DetectedFeaturePoints" << bwritePoints
//                  << "Write_extrinsicParameters"   << bwriteExtrinsics
//                  << "Write_outputFileName"  << outputFileName
//
//                  << "Show_UndistortedImage" << showUndistorsed
//
//                  << "Input_FlipAroundHorizontalAxis" << flipVertical
//                  << "Input_Delay" << delay
//                  << "Input" << input
//           << "}";
//    }
#endif /* __SETTINGS_HPP__ */
