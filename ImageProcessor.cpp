/*
 * ImageProcessor.cpp
 *
 */

#include <vector>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "ImageProcessor.h"
#include "Config.h"
#include "DebugOutput.h"

/**
 * Functor to help sorting rectangles by their x-position.
 */
class sortRectByX {
public:
    bool operator()(cv::Rect const & a, cv::Rect const & b) const {
        return a.x < b.x;
    }
};

ImageProcessor::ImageProcessor(const Config & config) :
        _config(config), _debugWindow(false), _debugSkew(false), _debugDigits(false), _debugEdges(false),
        _perspectiveScaleX(1.0), _perspectiveScaleY(1.0), _perspectiveCorrectionApplied(false) {
}

/**
 * Set the input image.
 */
void ImageProcessor::setInput(cv::Mat & img) {
    _img = img;
}

/**
 * Get the vector of output images.
 * Each image contains the edges of one isolated digit.
 */
const std::vector<cv::Mat> & ImageProcessor::getOutput() {
    return _digits;
}

void ImageProcessor::debugWindow(bool bval) {
    _debugWindow = bval;
    if (_debugWindow) {
        cv::namedWindow("ImageProcessor");
    }
}

void ImageProcessor::debugSkew(bool bval) {
    _debugSkew = bval;
}

void ImageProcessor::debugEdges(bool bval) {
    _debugEdges = bval;
}

void ImageProcessor::debugDigits(bool bval) {
    _debugDigits = bval;
}

void ImageProcessor::showImage() {
    cv::imshow("ImageProcessor", _img);
    cv::waitKey(1);
}

/**
 * Main processing function.
 * Read input image and create vector of images for each digit.
 */
void ImageProcessor::process() {
    _digits.clear();

    // convert to gray
#if CV_MAJOR_VERSION == 2
    cvtColor(_img, _imgGray, CV_BGR2GRAY); 
#elif CV_MAJOR_VERSION == 3 | 4
    cvtColor(_img, _imgGray, cv::COLOR_BGR2GRAY);
#endif

    // Debug: Original image after grayscale conversion
    if (_config.getTestMode()) {
        std::map<std::string, std::string> params;
        DebugOutput::saveDebugImage(_imgGray, "ImageProcessor_grayscale", params);
    }

    // Perspective correction using homography (before rotation/skew correction)
    correctPerspective();

    // initial rotation to get the digits up
    rotate(_config.getRotationDegrees());

    // detect and correct remaining skew (+- 30 deg)
    float skew_deg = detectSkew();
    rotate(skew_deg);

    // find and isolate counter digits
    findCounterDigits();

    if (_debugWindow) {
        showImage();
    }
}

/**
 * Rotate image.
 */
void ImageProcessor::rotate(double rotationDegrees) {
    cv::Mat M = cv::getRotationMatrix2D(cv::Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
    cv::Mat img_rotated;
    cv::warpAffine(_imgGray, img_rotated, M, _imgGray.size());
    _imgGray = img_rotated;
    if (_debugWindow) {
        cv::warpAffine(_img, img_rotated, M, _img.size());
        _img = img_rotated;
    }
    
    // Debug: Save rotated image
    if (_config.getTestMode()) {
        std::map<std::string, std::string> params;
        params["degrees"] = std::to_string(rotationDegrees);
        DebugOutput::saveDebugImage(_imgGray, "ImageProcessor_rotate", params);
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec2f>& lines) {
    // draw lines
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        cv::line(_img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec4i>& lines, int xoff, int yoff) {
    for (size_t i = 0; i < lines.size(); i++) {
        cv::line(_img, cv::Point(lines[i][0] + xoff, lines[i][1] + yoff),
                cv::Point(lines[i][2] + xoff, lines[i][3] + yoff), cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Detect the skew of the image by finding almost (+- 30 deg) horizontal lines.
 */
float ImageProcessor::detectSkew() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    cv::Mat edges = cannyEdges();

    // find lines
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

    // filter lines by theta and compute average
    std::vector<cv::Vec2f> filteredLines;
    float theta_min = 60.f * CV_PI / 180.f;
    float theta_max = 120.f * CV_PI / 180.0f;
    float theta_avr = 0.f;
    float theta_deg = 0.f;
    for (size_t i = 0; i < lines.size(); i++) {
        float theta = lines[i][1];
        if (theta >= theta_min && theta <= theta_max) {
            filteredLines.push_back(lines[i]);
            theta_avr += theta;
        }
    }
    if (filteredLines.size() > 0) {
        theta_avr /= filteredLines.size();
        theta_deg = (theta_avr / CV_PI * 180.f) - 90;
        rlog.info("detectSkew: %.1f deg", theta_deg);
    } else {
        rlog.warn("failed to detect skew");
    }

    if (_debugSkew) {
        drawLines(filteredLines);
    }

    return theta_deg;
}

/**
 * Detect edges using Canny algorithm.
 */
cv::Mat ImageProcessor::cannyEdges() {
    cv::Mat edges;
    // detect edges
    cv::Canny(_imgGray, edges, _config.getCannyThreshold1(), _config.getCannyThreshold2());
    
    // Debug: Save canny edges
    if (_config.getTestMode()) {
        std::map<std::string, std::string> params;
        params["threshold1"] = std::to_string(_config.getCannyThreshold1());
        params["threshold2"] = std::to_string(_config.getCannyThreshold2());
        DebugOutput::saveDebugImage(edges, "ImageProcessor_cannyEdges", params);
    }
    
    return edges;
}

/**
 * Find bounding boxes that are aligned at y position.
 */
void ImageProcessor::findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
        std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result) {
    std::vector<cv::Rect>::const_iterator it = begin;
    cv::Rect start = *it;
    ++it;
    result.push_back(start);

    // Calculate scaled Y alignment based on perspective correction
    int scaledYAlignment = _config.getDigitYAlignment();
    int scaledHeightTolerance = 5;
    
    if (_perspectiveCorrectionApplied) {
        // More lenient Y-alignment after perspective correction due to possible distortion
        scaledYAlignment = (int)(_config.getDigitYAlignment() * _perspectiveScaleY * 1.5); // 50% more tolerance
        scaledHeightTolerance = (int)(10 * _perspectiveScaleY); // Double the height tolerance
        
        log4cpp::Category& rlog = log4cpp::Category::getRoot();
        rlog << log4cpp::Priority::INFO << "Using scaled Y-alignment: " << scaledYAlignment 
             << " (base: " << _config.getDigitYAlignment() << ", scale: " << _perspectiveScaleY 
             << "), height tolerance: " << scaledHeightTolerance;
    }

    for (; it != end; ++it) {
        if (abs(start.y - it->y) < scaledYAlignment && abs(start.height - it->height) < scaledHeightTolerance) {
            result.push_back(*it);
        }
    }
}

/**
 * Filter contours by size of bounding rectangle.
 */
void ImageProcessor::filterContours(std::vector<std::vector<cv::Point> >& contours,
        std::vector<cv::Rect>& boundingBoxes, std::vector<std::vector<cv::Point> >& filteredContours) {
    // Calculate scaled parameters based on perspective correction
    int scaledMinHeight = _config.getDigitMinHeight();
    int scaledMaxHeight = _config.getDigitMaxHeight();
    int scaledMinWidth = _config.getDigitMinWidth();
    
    if (_perspectiveCorrectionApplied) {
        scaledMinHeight = (int)(_config.getDigitMinHeight() * _perspectiveScaleY);
        // More generous maximum height to accommodate perspective distortion using configurable tolerance
        scaledMaxHeight = (int)(_config.getDigitMaxHeight() * _perspectiveScaleY * _config.getPerspectiveHeightTolerance());
        scaledMinWidth = (int)(_config.getDigitMinWidth() * _perspectiveScaleX);
        
        log4cpp::Category& rlog = log4cpp::Category::getRoot();
        rlog << log4cpp::Priority::INFO << "Using scaled parameters for digit detection: minHeight=" 
             << scaledMinHeight << " (was " << _config.getDigitMinHeight() << "), maxHeight=" 
             << scaledMaxHeight << " (was " << _config.getDigitMaxHeight() << "), minWidth=" 
             << scaledMinWidth << " (was " << _config.getDigitMinWidth() << "), Y-scale=" << _perspectiveScaleY;
    }
    
    // filter contours by bounding rect size
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bounds = cv::boundingRect(contours[i]);
        if (bounds.height > scaledMinHeight && bounds.height < scaledMaxHeight
                && bounds.width > scaledMinWidth && bounds.width < bounds.height) {
            boundingBoxes.push_back(bounds);
            filteredContours.push_back(contours[i]);
        }
    }
}

/**
 * Crop a rectangle by removing a percentage from each side to focus on center content.
 * This helps remove frame edges and borders from digit recognition.
 */
cv::Rect ImageProcessor::cropRectangle(const cv::Rect& original, double cropPercent, const cv::Size& imageSize) {
    // Use configurable crop percentages from settings
    double cropPercentHorizontal = _config.getCropPercentHorizontal();
    double cropPercentVertical = _config.getCropPercentVertical();
    
    // Calculate crop amounts for each side
    int cropX = (int)(original.width * cropPercentHorizontal);
    int cropY = (int)(original.height * cropPercentVertical);
    
    // Calculate new rectangle dimensions
    int newX = original.x + cropX;
    int newY = original.y + cropY;
    int newWidth = original.width - (2 * cropX);
    int newHeight = original.height - (2 * cropY);
    
    // Ensure the cropped rectangle stays within image bounds
    newX = std::max(0, std::min(newX, imageSize.width - 1));
    newY = std::max(0, std::min(newY, imageSize.height - 1));
    newWidth = std::max(1, std::min(newWidth, imageSize.width - newX));
    newHeight = std::max(1, std::min(newHeight, imageSize.height - newY));
    
    return cv::Rect(newX, newY, newWidth, newHeight);
}

/**
 * Crop rectangle with custom horizontal and vertical percentages.
 * Used for special cases like AOI digits that need different cropping.
 */
cv::Rect ImageProcessor::cropRectangleCustom(const cv::Rect& original, double cropPercentHorizontal, double cropPercentVertical, const cv::Size& imageSize) {
    // Calculate crop amounts for each side
    int cropX = (int)(original.width * cropPercentHorizontal);
    int cropY = (int)(original.height * cropPercentVertical);
    
    // Calculate new rectangle dimensions
    int newX = original.x + cropX;
    int newY = original.y + cropY;
    int newWidth = original.width - (2 * cropX);
    int newHeight = original.height - (2 * cropY);
    
    // Ensure the cropped rectangle stays within image bounds
    newX = std::max(0, std::min(newX, imageSize.width - 1));
    newY = std::max(0, std::min(newY, imageSize.height - 1));
    newWidth = std::max(1, std::min(newWidth, imageSize.width - newX));
    newHeight = std::max(1, std::min(newHeight, imageSize.height - newY));
    
    return cv::Rect(newX, newY, newWidth, newHeight);
}

/**
 * Filter fragments from digit images using morphological operations.
 * Steps: Dilate → Find contours → Keep largest → Delete smaller → Erode
 * This removes small fragments while preserving the main digit structure.
 */
cv::Mat ImageProcessor::filterFragments(const cv::Mat& digitImage) {
    // Safety check: ensure we have a valid input image
    if (digitImage.empty()) {
        return digitImage.clone();
    }
    
    // Work with a copy of the input image
    cv::Mat result = digitImage.clone();
    
    // Step 1: Light dilation to merge nearby edges using configurable parameters
    int kernelSize = std::max(2, std::min(digitImage.rows, digitImage.cols) / _config.getMorphKernelSizeDivisor());
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(kernelSize, kernelSize));
    cv::Mat dilated;
    // Use configurable iterations for dilation
    cv::dilate(result, dilated, kernel, cv::Point(-1,-1), _config.getMorphIterations());
    
    // Step 2: Find contours in dilated image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dilated, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    if (contours.empty()) {
        return result; // No contours found, return original
    }
    
    if (contours.size() == 1) {
        // Only one contour found - apply erosion to restore original thickness
        cv::Mat singleContourResult;
        cv::erode(dilated, singleContourResult, kernel, cv::Point(-1,-1), _config.getMorphIterations());
        return singleContourResult;
    }
    
    // Step 3: Enhanced contour analysis with enclosed area detection (configurable)
    std::vector<double> areas;
    std::vector<bool> hasEnclosedAreas;
    
    for (size_t i = 0; i < contours.size(); i++) {
        areas.push_back(cv::contourArea(contours[i]));
        
        if (_config.getEnclosedAreaDetection()) {
            // Check if this contour has enclosed areas (holes) after dilation
            cv::Mat contourMask = cv::Mat::zeros(dilated.size(), CV_8UC1);
            cv::drawContours(contourMask, contours, i, cv::Scalar(255), cv::FILLED);
            
            // Find inner contours (holes) within this filled contour
            std::vector<std::vector<cv::Point>> innerContours;
            std::vector<cv::Vec4i> innerHierarchy;
            cv::findContours(contourMask, innerContours, innerHierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
            
            // Count significant holes - using configurable minimum hole area ratio
            int significantHoles = 0;
            double minHoleArea = areas[i] * _config.getMinHoleAreaRatio();
            
            for (size_t j = 0; j < innerContours.size(); j++) {
                // Check if this is a hole (has parent in hierarchy)
                if (innerHierarchy[j][3] != -1) { // Has parent = is a hole
                    double holeArea = cv::contourArea(innerContours[j]);
                    if (holeArea > minHoleArea) {
                        significantHoles++;
                    }
                }
            }
            
            hasEnclosedAreas.push_back(significantHoles > 0);
        } else {
            // Enclosed area detection disabled - assume no enclosed areas
            hasEnclosedAreas.push_back(false);
        }
    }
    
    // Find three largest areas for improved 8-digit detection
    std::vector<std::pair<double, int>> areaIndexPairs;
    for (size_t i = 0; i < areas.size(); i++) {
        areaIndexPairs.push_back({areas[i], static_cast<int>(i)});
    }
    
    // Sort by area (descending)
    std::sort(areaIndexPairs.begin(), areaIndexPairs.end(), 
              [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                  return a.first > b.first;
              });
    
    // Extract the three largest
    double maxArea = areaIndexPairs.empty() ? 0 : areaIndexPairs[0].first;
    double secondMaxArea = (areaIndexPairs.size() > 1) ? areaIndexPairs[1].first : 0;
    double thirdMaxArea = (areaIndexPairs.size() > 2) ? areaIndexPairs[2].first : 0;
    int maxAreaIdx = areaIndexPairs.empty() ? 0 : areaIndexPairs[0].second;
    
    // Step 4: Advanced 3-hole fragment filtering with enclosed area analysis
    double secondRatio = (maxArea > 0) ? (secondMaxArea / maxArea) : 0;
    double thirdRatio = (maxArea > 0) ? (thirdMaxArea / maxArea) : 0;
    
    // Filter contours based on enclosed area detection (if enabled) or size-based fallback
    std::vector<int> validContourIndices;
    
    if (_config.getEnclosedAreaDetection()) {
        // Advanced filtering using enclosed area analysis
        for (size_t i = 0; i < contours.size(); i++) {
            double areaRatio = areas[i] / maxArea;
            
            // Balanced approach: Prefer enclosed areas but allow fallbacks for edge cases
            // - Primary rule: Contours with enclosed areas are likely valid digits
            // - Fallback rule: Large contours might be damaged digits (e.g., broken "0")
            // - Reject rule: Small contours without structure are definitely fragments
            
            if (hasEnclosedAreas[i]) {
                // Contour has enclosed areas - definitely a valid digit
                validContourIndices.push_back(i);
            } else if (areaRatio > _config.getMorphSizeRatioThreshold()) {
                // Large contour without holes - might be damaged digit (broken "0", "6", etc.)
                // Keep as fallback to avoid losing valid digits due to poor image quality
                validContourIndices.push_back(i);
            }
            // Small contours without enclosed areas are fragments - reject them
        }
    } else {
        // Fallback: Simple size-based filtering (original Smart Fragment Filtering)
        // Keep only largest contour when enclosed area detection is disabled
        validContourIndices.push_back(maxAreaIdx);
    }
    
    // If no valid contours found, fall back to largest contour
    if (validContourIndices.empty()) {
        validContourIndices.push_back(maxAreaIdx);
    }
    
    // Enhanced 3-hole analysis for better digit 8 detection
    if (secondRatio >= _config.getMorphSizeRatioThreshold() && 
        thirdRatio >= _config.getMorphSizeRatioThreshold()) {
        
        // Three-hole structure detected (likely digit 8 or complex structure)
        // Keep all three largest contours, just apply erosion to restore thickness
        cv::Mat preservedResult;
        cv::erode(dilated, preservedResult, kernel, cv::Point(-1,-1), _config.getMorphIterations());
        
        if (_debugDigits) {
            std::cout << "Three-hole structure: Areas(" << maxArea << ", " << secondMaxArea << ", " << thirdMaxArea 
                      << ") Ratios(" << secondRatio << ", " << thirdRatio << ")" << std::endl;
        }
        return preservedResult;
        
    } else if (validContourIndices.size() > 1 && secondRatio > _config.getMorphSizeRatioThreshold()) {
        
        // Two-hole structure detected (likely digit 0, 6, 9 or damaged structure)
        // Keep valid contours, just apply erosion to restore thickness
        cv::Mat preservedResult;
        cv::erode(dilated, preservedResult, kernel, cv::Point(-1,-1), _config.getMorphIterations());
        
        if (_debugDigits) {
            std::cout << "Two-hole structure: Areas(" << maxArea << ", " << secondMaxArea 
                      << ") Ratio(" << secondRatio << ")" << std::endl;
        }
        return preservedResult;
    }
    
    // Step 5: Create mask with valid contours only
    cv::Mat mask = cv::Mat::zeros(dilated.size(), CV_8UC1);
    for (int idx : validContourIndices) {
        cv::drawContours(mask, contours, idx, cv::Scalar(255), cv::FILLED);
    }
    
    // Step 6: Apply mask to dilated image (preserves valid contours)
    cv::Mat filteredDilated = cv::Mat::zeros(dilated.size(), CV_8UC1);
    dilated.copyTo(filteredDilated, mask);
    
    // Step 7: Erode the filtered result back to original thickness
    // Use same number of iterations as dilation to maintain balance
    cv::Mat finalResult;
    cv::erode(filteredDilated, finalResult, kernel, cv::Point(-1,-1), _config.getMorphIterations());
    
    return finalResult;
}

/**
 * Find and isolate the digits of the counter,
 */
void ImageProcessor::findCounterDigits() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    // edge image
    cv::Mat edges = cannyEdges();
    if (_debugEdges) {
        cv::imshow("edges", edges);
    }

    cv::Mat img_ret = edges.clone();

    // find contours in whole image
    std::vector<std::vector<cv::Point> > contours, filteredContours;
    std::vector<cv::Rect> boundingBoxes;

#if CV_MAJOR_VERSION == 2
    cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
#elif CV_MAJOR_VERSION == 3 | 4
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
#endif

    // filter contours by bounding rect size
    filterContours(contours, boundingBoxes, filteredContours);

    rlog << log4cpp::Priority::INFO << "number of filtered contours: " << filteredContours.size();

    // find bounding boxes that are aligned at y position
    std::vector<cv::Rect> alignedBoundingBoxes, tmpRes;
    for (std::vector<cv::Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
        tmpRes.clear();
        findAlignedBoxes(ib, boundingBoxes.end(), tmpRes);
        if (tmpRes.size() > alignedBoundingBoxes.size()) {
            alignedBoundingBoxes = tmpRes;
        }
    }
    rlog << log4cpp::Priority::INFO << "max number of alignedBoxes: " << alignedBoundingBoxes.size();

    // sort bounding boxes from left to right
    std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());

    // Area of Interest approach: If enabled and we have exactly 6 boxes with regular spacing,
    // predict the 7th digit (decimal place) position
    if (_config.getAreaOfInterest() && alignedBoundingBoxes.size() == 6) {
        rlog << log4cpp::Priority::INFO << "=== AREA OF INTEREST FOR 7TH DIGIT ===";
        
        // Check if the 6 boxes have regular spacing and similar sizes
        bool regularSpacing = true;
        bool similarSizes = true;
        
        // Calculate average spacing between digits
        std::vector<int> spacings;
        std::vector<int> widths;
        std::vector<int> heights;
        
        for (size_t i = 0; i < alignedBoundingBoxes.size(); i++) {
            widths.push_back(alignedBoundingBoxes[i].width);
            heights.push_back(alignedBoundingBoxes[i].height);
            
            if (i > 0) {
                int spacing = alignedBoundingBoxes[i].x - (alignedBoundingBoxes[i-1].x + alignedBoundingBoxes[i-1].width);
                spacings.push_back(spacing);
            }
        }
        
        // Calculate averages
        int avgSpacing = 0, avgWidth = 0, avgHeight = 0;
        for (int s : spacings) avgSpacing += s;
        for (int w : widths) avgWidth += w;
        for (int h : heights) avgHeight += h;
        
        avgSpacing /= spacings.size();
        avgWidth /= widths.size();
        avgHeight /= heights.size();
        
        rlog << log4cpp::Priority::INFO << "Average spacing: " << avgSpacing << ", width: " << avgWidth << ", height: " << avgHeight;
        
        // Check regularity using configurable spacing tolerance
        for (int s : spacings) {
            if (abs(s - avgSpacing) > avgSpacing * _config.getSmartSpacingTolerance()) {
                regularSpacing = false;
                break;
            }
        }
        
        // Check size similarity using configurable size tolerance
        for (size_t i = 0; i < widths.size(); i++) {
            if (abs(widths[i] - avgWidth) > avgWidth * _config.getSmartSizeTolerance() || 
                abs(heights[i] - avgHeight) > avgHeight * _config.getSmartSizeTolerance()) {
                similarSizes = false;
                break;
            }
        }
        
        rlog << log4cpp::Priority::INFO << "Regular spacing: " << (regularSpacing ? "YES" : "NO") 
             << ", Similar sizes: " << (similarSizes ? "YES" : "NO");
        
        if (regularSpacing && similarSizes && avgSpacing > 0) {
            // Predict 7th digit position (decimal place)
            cv::Rect lastBox = alignedBoundingBoxes[5]; // rightmost box
            
            // Position for decimal digit: after last box with same spacing
            int decimalsX = lastBox.x + lastBox.width + avgSpacing;
            int decimalsY = lastBox.y; // same Y as other digits
            
            // Use configurable width multiplier for decimal box to ensure complete digit capture
            int decimalsWidth = (int)(avgWidth * _config.getAoiWidthMultiplier());
            int decimalsHeight = avgHeight;
            
            cv::Rect predictedDecimalBox(decimalsX, decimalsY, decimalsWidth, decimalsHeight);
            
            rlog << log4cpp::Priority::INFO << "Predicted 7th digit area: x=" << decimalsX 
                 << " y=" << decimalsY << " w=" << decimalsWidth << " h=" << decimalsHeight;
            
            // Check if this area is within image bounds
            if (decimalsX >= 0 && decimalsY >= 0 && 
                decimalsX + decimalsWidth < img_ret.cols && 
                decimalsY + decimalsHeight < img_ret.rows) {
                
                // Extract the predicted area
                cv::Mat decimalsArea = img_ret(predictedDecimalBox);
                
                // Check if this area contains significant edges (potential digit)
                cv::Scalar meanValue = cv::mean(decimalsArea);
                int edgePixels = cv::countNonZero(decimalsArea);
                double edgeDensity = (double)edgePixels / (decimalsWidth * decimalsHeight);
                
                rlog << log4cpp::Priority::INFO << "Decimal area edge density: " << edgeDensity 
                     << " (pixels: " << edgePixels << "/" << (decimalsWidth * decimalsHeight) << ")";
                
                // If edge density is reasonable using configurable thresholds, add as 7th digit
                if (edgeDensity >= _config.getAoiMinEdgeDensity() && edgeDensity <= _config.getAoiMaxEdgeDensity()) {
                    alignedBoundingBoxes.push_back(predictedDecimalBox);
                    rlog << log4cpp::Priority::INFO << "Added predicted 7th digit! Total digits: " << alignedBoundingBoxes.size();
                    
                    // Re-sort with the new 7th digit
                    std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());
                } else {
                    rlog << log4cpp::Priority::INFO << "Predicted area rejected - edge density outside range (" 
                         << _config.getAoiMinEdgeDensity() << "-" << _config.getAoiMaxEdgeDensity() << ")";
                }
            } else {
                rlog << log4cpp::Priority::INFO << "Predicted 7th digit area is outside image bounds";
            }
        } else {
            rlog << log4cpp::Priority::INFO << "Cannot predict 7th digit - irregular spacing or sizes";
        }
        
        rlog << log4cpp::Priority::INFO << "=== END AREA OF INTEREST ===";
    }
    
    rlog << log4cpp::Priority::INFO << "Final number of aligned boxes: " << alignedBoundingBoxes.size();

    if (_debugEdges) {
        // draw contours
        cv::Mat cont = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
        cv::drawContours(cont, filteredContours, -1, cv::Scalar(255));
        cv::imshow("contours", cont);
    }

    // cut out found rectangles from edged image
    for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
        cv::Rect roi = alignedBoundingBoxes[i];
        
        cv::Rect finalRoi = roi;
        
        // Crop digits if enabled in config
        if (_config.getCropDigits()) {
            // Special cropping for AOI (7th digit): 15% horizontal, 2% vertical
            // AOI digit is the rightmost (last) digit when we have 7 digits total
            bool isAOIDigit = (_config.getAreaOfInterest() && 
                             alignedBoundingBoxes.size() == 7 && 
                             i == alignedBoundingBoxes.size() - 1);
            
            if (isAOIDigit) {
                // AOI-specific cropping using configurable parameters
                finalRoi = cropRectangleCustom(roi, _config.getCropPercentHorizontalAOI(), 
                                             _config.getCropPercentVerticalAOI(), img_ret.size());
            } else {
                // Standard cropping using configurable parameters
                finalRoi = cropRectangle(roi, 0.0, img_ret.size()); // Parameter wird in Funktion ignoriert
            }
        }
        
        // Extract digit image
        cv::Mat digitImage = img_ret(finalRoi);
        
        // Apply fragment filtering if cropping is enabled
        if (_config.getCropDigits()) {
            digitImage = filterFragments(digitImage);
        }
        
        _digits.push_back(digitImage);
        if (_debugDigits) {
            cv::rectangle(_img, roi, cv::Scalar(0, 255, 0), 2);
            if (_config.getCropDigits()) {
                cv::rectangle(_img, finalRoi, cv::Scalar(255, 0, 0), 1); // Blue inner rectangle when cropping
            }
        }
        
        // Debug: Save individual digit with crop info
        if (_config.getTestMode()) {
            std::map<std::string, std::string> params;
            params["digit_index"] = std::to_string(i);
            params["x"] = std::to_string(finalRoi.x);
            params["y"] = std::to_string(finalRoi.y);
            params["width"] = std::to_string(finalRoi.width);
            params["height"] = std::to_string(finalRoi.height);
            if (_config.getCropDigits()) {
                // Check if this is AOI digit for debug output
                bool isAOIDigit = (_config.getAreaOfInterest() && 
                                 alignedBoundingBoxes.size() == 7 && 
                                 i == alignedBoundingBoxes.size() - 1);
                
                if (isAOIDigit) {
                    params["crop"] = "15pct_width_2pct_height_AOI";
                } else {
                    params["crop"] = "10pct_width_2pct_height";
                }
                params["filter"] = "fragments_removed_smart";
            } else {
                params["crop"] = "none";
                params["filter"] = "none";
            }
            DebugOutput::saveDebugImage(digitImage, "ImageProcessor_digit", params);
        }
    }
}

/**
 * Correct perspective distortion using homography transformation.
 * This function detects the meter display corners and applies perspective correction
 * to make the image appear as if viewed from directly in front.
 */
void ImageProcessor::correctPerspective() {
    if (!_config.getPerspectiveCorrection()) {
        return; // Skip if perspective correction is disabled
    }
    
    log4cpp::Category& rlog = log4cpp::Category::getRoot();
    rlog << log4cpp::Priority::INFO << "=== PERSPECTIVE CORRECTION ===";
    
    // Detect the four corners of the meter display
    std::vector<cv::Point2f> srcCorners = detectMeterCorners();
    
    if (srcCorners.size() == 4) {
        // Define target rectangle (straight view) - maintain aspect ratio
        int targetWidth = _imgGray.cols;
        int targetHeight = _imgGray.rows;
        
        std::vector<cv::Point2f> dstCorners = {
            cv::Point2f(0, 0),                           // top-left
            cv::Point2f(targetWidth - 1, 0),            // top-right
            cv::Point2f(targetWidth - 1, targetHeight - 1), // bottom-right
            cv::Point2f(0, targetHeight - 1)            // bottom-left
        };
        
        // Calculate homography matrix
        cv::Mat homography = cv::getPerspectiveTransform(srcCorners, dstCorners);
        
        // Calculate scaling factors before transformation
        cv::Rect originalBounds = cv::boundingRect(srcCorners);
        _perspectiveScaleX = (double)targetWidth / originalBounds.width;
        _perspectiveScaleY = (double)targetHeight / originalBounds.height;
        _perspectiveCorrectionApplied = true;
        
        rlog << log4cpp::Priority::INFO << "Perspective scaling factors: X=" << _perspectiveScaleX 
             << ", Y=" << _perspectiveScaleY << " (original area: " << originalBounds.width 
             << "x" << originalBounds.height << " -> " << targetWidth << "x" << targetHeight << ")";
        
        // Apply perspective correction
        cv::Mat corrected;
        cv::warpPerspective(_imgGray, corrected, homography, _imgGray.size());
        _imgGray = corrected;
        
        if (_debugWindow) {
            cv::warpPerspective(_img, corrected, homography, _img.size());
            _img = corrected;
        }
        
        rlog << log4cpp::Priority::INFO << "Perspective correction applied successfully";
        
        // Debug: Save corrected image
        if (_config.getTestMode()) {
            std::map<std::string, std::string> params;
            params["method"] = "homography";
            params["corners"] = std::to_string(srcCorners.size());
            DebugOutput::saveDebugImage(_imgGray, "ImageProcessor_perspective_corrected", params);
        }
    } else {
        rlog << log4cpp::Priority::WARN << "Could not detect 4 corners for perspective correction (found: " 
             << srcCorners.size() << ")";
    }
    
    rlog << log4cpp::Priority::INFO << "=== END PERSPECTIVE CORRECTION ===";
}

/**
 * Detect the four corners of the meter display for perspective correction.
 * Uses contour detection to find rectangular shapes that likely represent the meter frame.
 */
std::vector<cv::Point2f> ImageProcessor::detectMeterCorners() {
    std::vector<cv::Point2f> corners;
    
    log4cpp::Category& rlog = log4cpp::Category::getRoot();
    
    // Use Canny edge detection for better contour detection
    cv::Mat edges = cannyEdges();
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
#if CV_MAJOR_VERSION == 2
    cv::findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
#elif CV_MAJOR_VERSION == 3 | 4
    cv::findContours(edges, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
#endif
    
        // Enhanced approach: Look for larger quadrilateral contours representing the meter display
        double maxArea = 0;
        std::vector<cv::Point> bestRectangle;
        
        rlog << log4cpp::Priority::INFO << "Analyzing " << contours.size() << " contours for perspective correction";
        
        // NEW APPROACH: Find largest contours and create bounding rectangles from them
        // This works better for perspective-distorted images where perfect rectangles don't exist
        
        std::vector<std::pair<double, size_t>> largeContours;
        double imageArea = _imgGray.rows * _imgGray.cols;
        
        // Collect all contours with reasonable size
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            if (area > imageArea * 0.005) { // At least 0.5% of image
                largeContours.push_back(std::make_pair(area, i));
            }
        }
        
        // Sort by area (largest first)
        std::sort(largeContours.begin(), largeContours.end(), std::greater<std::pair<double, size_t>>());
        
        rlog << log4cpp::Priority::INFO << "Found " << largeContours.size() << " large contours for perspective correction";
        
        // Try the largest contours - use their bounding rectangles as approximation
        for (size_t idx = 0; idx < std::min(largeContours.size(), size_t(5)); idx++) {
            size_t i = largeContours[idx].second;
            double area = largeContours[idx].first;
            double areaPercent = (area / imageArea) * 100;
            
            rlog << log4cpp::Priority::INFO << "Considering contour " << i << ": area=" << area 
                 << " (" << areaPercent << "% of image) - rank #" << (idx + 1);
            
            // Get bounding rectangle of this large contour
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            double widthRatio = (double)boundingRect.width / _imgGray.cols;
            double heightRatio = (double)boundingRect.height / _imgGray.rows;
            
            // Accept if it spans a reasonable part of the image - strict criteria to prevent instability
            if (area > imageArea * 0.10 && widthRatio > 0.50 && heightRatio > 0.30) {
                // Create rectangle from bounding box - this gives us guaranteed 4 corners
                bestRectangle = {
                    cv::Point(boundingRect.x, boundingRect.y),                                     // top-left
                    cv::Point(boundingRect.x + boundingRect.width, boundingRect.y),              // top-right  
                    cv::Point(boundingRect.x + boundingRect.width, boundingRect.y + boundingRect.height), // bottom-right
                    cv::Point(boundingRect.x, boundingRect.y + boundingRect.height)              // bottom-left
                };
                maxArea = area;
                
                rlog << log4cpp::Priority::INFO << "Using bounding rectangle of large contour: " 
                     << "size=" << boundingRect.width << "x" << boundingRect.height 
                     << " (" << (widthRatio*100) << "% x " << (heightRatio*100) << "%), area=" << areaPercent << "%";
                break;
            }
        }
        
        // If still nothing found, try with even more permissive criteria
        if (bestRectangle.empty() && !largeContours.empty()) {
            size_t i = largeContours[0].second; // Take the very largest
            double area = largeContours[0].first;
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            
            if (area > imageArea * 0.08 && cv::boundingRect(contours[i]).width > _imgGray.cols * 0.45) { // Strict fallback to prevent instability
                bestRectangle = {
                    cv::Point(boundingRect.x, boundingRect.y),
                    cv::Point(boundingRect.x + boundingRect.width, boundingRect.y),
                    cv::Point(boundingRect.x + boundingRect.width, boundingRect.y + boundingRect.height),
                    cv::Point(boundingRect.x, boundingRect.y + boundingRect.height)
                };
                maxArea = area;
                
                rlog << log4cpp::Priority::INFO << "Using largest available contour as fallback: area=" 
                     << (area/imageArea*100) << "%, size=" << boundingRect.width << "x" << boundingRect.height;
            }
        }
        
        // Fallback: If no suitable contour found, use image corners with slight inset
        if (bestRectangle.empty()) {
            rlog << log4cpp::Priority::INFO << "No suitable contour found, using image corners as fallback";
            int inset = std::min(_imgGray.rows, _imgGray.cols) / 20; // 5% inset
            bestRectangle = {
                cv::Point(inset, inset),                                    // top-left
                cv::Point(_imgGray.cols - inset, inset),                   // top-right  
                cv::Point(_imgGray.cols - inset, _imgGray.rows - inset),   // bottom-right
                cv::Point(inset, _imgGray.rows - inset)                    // bottom-left
            };
            rlog << log4cpp::Priority::INFO << "Using fallback corners with " << inset << "px inset";
        }
        
    if (!bestRectangle.empty()) {
        // Convert to Point2f and sort corners in consistent order
        for (const auto& pt : bestRectangle) {
            corners.push_back(cv::Point2f(pt.x, pt.y));
        }
        
        // Sort corners: top-left, top-right, bottom-right, bottom-left
        std::sort(corners.begin(), corners.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
            return (a.y < b.y) || (a.y == b.y && a.x < b.x);
        });
        
        // Ensure proper order: top two points, then bottom two points
        if (corners.size() == 4) {
            // Sort top two points by x-coordinate
            if (corners[0].x > corners[1].x) {
                std::swap(corners[0], corners[1]);
            }
            // Sort bottom two points by x-coordinate  
            if (corners[2].x < corners[3].x) {
                std::swap(corners[2], corners[3]);
            }
        }
        
        rlog << log4cpp::Priority::INFO << "Detected rectangular meter frame with area: " << maxArea;
        for (size_t i = 0; i < corners.size(); i++) {
            rlog << log4cpp::Priority::INFO << "Corner " << i << ": (" << corners[i].x << ", " << corners[i].y << ")";
        }
    } else {
        rlog << log4cpp::Priority::WARN << "No suitable rectangular contour found for perspective correction";
    }
    
    return corners;
}
