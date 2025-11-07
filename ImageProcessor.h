/*
 * ImageProcessor.h
 *
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include "ImageInput.h"
#include "Config.h"

class ImageProcessor {
public:
    ImageProcessor(const Config & config);

    void setOrientation(int rotationDegrees);
    void setInput(cv::Mat & img);
    void process();
    const std::vector<cv::Mat> & getOutput();

    void debugWindow(bool bval = true);
    void debugSkew(bool bval = true);
    void debugEdges(bool bval = true);
    void debugDigits(bool bval = true);
    void showImage();
    void saveConfig();
    void loadConfig();

private:
    void rotate(double rotationDegrees);
    void findCounterDigits();
    void findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
            std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result);
    float detectSkew();
    void drawLines(std::vector<cv::Vec2f>& lines);
    void drawLines(std::vector<cv::Vec4i>& lines, int xoff=0, int yoff=0);
    cv::Mat cannyEdges();
    void filterContours(std::vector<std::vector<cv::Point> >& contours, std::vector<cv::Rect>& boundingBoxes,
            std::vector<std::vector<cv::Point> >& filteredContours);
    cv::Rect cropRectangle(const cv::Rect& original, double cropPercent, const cv::Size& imageSize);
    cv::Rect cropRectangleCustom(const cv::Rect& original, double cropPercentHorizontal, double cropPercentVertical, const cv::Size& imageSize);
    cv::Mat filterFragments(const cv::Mat& digitImage);
    void correctPerspective();
    std::vector<cv::Point2f> detectMeterCorners();

    cv::Mat _img;
    cv::Mat _imgGray;
    std::vector<cv::Mat> _digits;
    Config _config;
    bool _debugWindow;
    bool _debugSkew;
    bool _debugEdges;
    bool _debugDigits;
    
    // Perspective correction scaling factors
    double _perspectiveScaleX = 1.0;
    double _perspectiveScaleY = 1.0;
    bool _perspectiveCorrectionApplied = false;
};

#endif /* IMAGEPROCESSOR_H_ */
