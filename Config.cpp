/*
 * Config.cpp
 *
 */

#include <opencv2/highgui/highgui.hpp>

#include "Config.h"

Config::Config() :
    _rotationDegrees(0), _ocrMaxDist(5e5), _digitMinHeight(20), _digitMaxHeight(
        90), _digitYAlignment(10), _cannyThreshold1(100), _cannyThreshold2(
        200), _trainingDataFilename("trainctr.yml"), _testMode(false), _areaOfInterest(false), _cropDigits(false), _perspectiveCorrection(false) {
}

void Config::saveConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::WRITE);
    fs << "rotationDegrees" << _rotationDegrees;
    fs << "cannyThreshold1" << _cannyThreshold1;
    fs << "cannyThreshold2" << _cannyThreshold2;
    fs << "digitMinHeight" << _digitMinHeight;
    fs << "digitMaxHeight" << _digitMaxHeight;
    fs << "digitYAlignment" << _digitYAlignment;
    fs << "ocrMaxDist" << _ocrMaxDist;
    fs << "trainingDataFilename" << _trainingDataFilename;
    fs << "testMode" << _testMode;
    fs << "areaOfInterest" << _areaOfInterest;
    fs << "cropDigits" << _cropDigits;
    fs << "perspectiveCorrection" << _perspectiveCorrection;
    fs << "cropPercentHorizontal" << _cropPercentHorizontal;
    fs << "cropPercentVertical" << _cropPercentVertical;
    fs << "cropPercentHorizontalAOI" << _cropPercentHorizontalAOI;
    fs << "cropPercentVerticalAOI" << _cropPercentVerticalAOI;
    fs << "morphKernelSizeDivisor" << _morphKernelSizeDivisor;
    fs << "morphIterations" << _morphIterations;
    fs << "morphSizeRatioThreshold" << _morphSizeRatioThreshold;
    fs << "smartSpacingTolerance" << _smartSpacingTolerance;
    fs << "smartSizeTolerance" << _smartSizeTolerance;
    fs << "digitMinWidth" << _digitMinWidth;
    fs << "perspectiveHeightTolerance" << _perspectiveHeightTolerance;
    fs << "aoiWidthMultiplier" << _aoiWidthMultiplier;
    fs << "aoiMinEdgeDensity" << _aoiMinEdgeDensity;
    fs << "aoiMaxEdgeDensity" << _aoiMaxEdgeDensity;
    fs << "enclosedAreaDetection" << _enclosedAreaDetection;
    fs << "minHoleAreaRatio" << _minHoleAreaRatio;
    fs.release();
}

void Config::loadConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["rotationDegrees"] >> _rotationDegrees;
        fs["cannyThreshold1"] >> _cannyThreshold1;
        fs["cannyThreshold2"] >> _cannyThreshold2;
        fs["digitMinHeight"] >> _digitMinHeight;
        fs["digitMaxHeight"] >> _digitMaxHeight;
        fs["digitYAlignment"] >> _digitYAlignment;
        fs["ocrMaxDist"] >> _ocrMaxDist;
        fs["trainingDataFilename"] >> _trainingDataFilename;
        if (!fs["testMode"].empty()) fs["testMode"] >> _testMode;
        if (!fs["areaOfInterest"].empty()) fs["areaOfInterest"] >> _areaOfInterest;
        if (!fs["cropDigits"].empty()) fs["cropDigits"] >> _cropDigits;
        if (!fs["perspectiveCorrection"].empty()) fs["perspectiveCorrection"] >> _perspectiveCorrection;
        if (!fs["cropPercentHorizontal"].empty()) fs["cropPercentHorizontal"] >> _cropPercentHorizontal;
        if (!fs["cropPercentVertical"].empty()) fs["cropPercentVertical"] >> _cropPercentVertical;
        if (!fs["cropPercentHorizontalAOI"].empty()) fs["cropPercentHorizontalAOI"] >> _cropPercentHorizontalAOI;
        if (!fs["cropPercentVerticalAOI"].empty()) fs["cropPercentVerticalAOI"] >> _cropPercentVerticalAOI;
        if (!fs["morphKernelSizeDivisor"].empty()) fs["morphKernelSizeDivisor"] >> _morphKernelSizeDivisor;
        if (!fs["morphIterations"].empty()) fs["morphIterations"] >> _morphIterations;
        if (!fs["morphSizeRatioThreshold"].empty()) fs["morphSizeRatioThreshold"] >> _morphSizeRatioThreshold;
        if (!fs["smartSpacingTolerance"].empty()) fs["smartSpacingTolerance"] >> _smartSpacingTolerance;
        if (!fs["smartSizeTolerance"].empty()) fs["smartSizeTolerance"] >> _smartSizeTolerance;
        if (!fs["digitMinWidth"].empty()) fs["digitMinWidth"] >> _digitMinWidth;
        if (!fs["perspectiveHeightTolerance"].empty()) fs["perspectiveHeightTolerance"] >> _perspectiveHeightTolerance;
        if (!fs["aoiWidthMultiplier"].empty()) fs["aoiWidthMultiplier"] >> _aoiWidthMultiplier;
        if (!fs["aoiMinEdgeDensity"].empty()) fs["aoiMinEdgeDensity"] >> _aoiMinEdgeDensity;
        if (!fs["aoiMaxEdgeDensity"].empty()) fs["aoiMaxEdgeDensity"] >> _aoiMaxEdgeDensity;
        if (!fs["enclosedAreaDetection"].empty()) fs["enclosedAreaDetection"] >> _enclosedAreaDetection;
        if (!fs["minHoleAreaRatio"].empty()) fs["minHoleAreaRatio"] >> _minHoleAreaRatio;
        fs.release();
    } else {
        // no config file - create an initial one with default values
        saveConfig();
    }
}

