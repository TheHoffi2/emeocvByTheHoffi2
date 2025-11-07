/*
 * Config.h
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

class Config {
public:
    Config();
    void saveConfig();
    void loadConfig();

    bool getTestMode() const { return _testMode; }
    void setTestMode(bool mode) { _testMode = mode; }

    bool getAreaOfInterest() const { return _areaOfInterest; }
    void setAreaOfInterest(bool mode) { _areaOfInterest = mode; }

    bool getCropDigits() const { return _cropDigits; }
    void setCropDigits(bool mode) { _cropDigits = mode; }

    bool getPerspectiveCorrection() const { return _perspectiveCorrection; }
    void setPerspectiveCorrection(bool mode) { _perspectiveCorrection = mode; }

    int getDigitMaxHeight() const {
        return _digitMaxHeight;
    }

    int getDigitMinHeight() const {
        return _digitMinHeight;
    }

    int getDigitYAlignment() const {
        return _digitYAlignment;
    }

    std::string getTrainingDataFilename() const {
        return _trainingDataFilename;
    }

    float getOcrMaxDist() const {
        return _ocrMaxDist;
    }

    int getRotationDegrees() const {
        return _rotationDegrees;
    }

    int getCannyThreshold1() const {
        return _cannyThreshold1;
    }

    int getCannyThreshold2() const {
        return _cannyThreshold2;
    }

    // Crop parameters for digit processing
    double getCropPercentHorizontal() const {
        return _cropPercentHorizontal;
    }
    void setCropPercentHorizontal(double percent) {
        _cropPercentHorizontal = percent;
    }

    double getCropPercentVertical() const {
        return _cropPercentVertical;
    }
    void setCropPercentVertical(double percent) {
        _cropPercentVertical = percent;
    }

    double getCropPercentHorizontalAOI() const {
        return _cropPercentHorizontalAOI;
    }
    void setCropPercentHorizontalAOI(double percent) {
        _cropPercentHorizontalAOI = percent;
    }

    double getCropPercentVerticalAOI() const {
        return _cropPercentVerticalAOI;
    }
    void setCropPercentVerticalAOI(double percent) {
        _cropPercentVerticalAOI = percent;
    }

    // Morphological operation parameters for fragment filtering
    int getMorphKernelSizeDivisor() const {
        return _morphKernelSizeDivisor;
    }
    void setMorphKernelSizeDivisor(int divisor) {
        _morphKernelSizeDivisor = divisor;
    }

    int getMorphIterations() const {
        return _morphIterations;
    }
    void setMorphIterations(int iterations) {
        _morphIterations = iterations;
    }

    double getMorphSizeRatioThreshold() const {
        return _morphSizeRatioThreshold;
    }
    void setMorphSizeRatioThreshold(double threshold) {
        _morphSizeRatioThreshold = threshold;
    }

    // Smart fragment filtering parameters for -C option
    double getSmartSpacingTolerance() const {
        return _smartSpacingTolerance;
    }
    void setSmartSpacingTolerance(double tolerance) {
        _smartSpacingTolerance = tolerance;
    }

    double getSmartSizeTolerance() const {
        return _smartSizeTolerance;
    }
    void setSmartSizeTolerance(double tolerance) {
        _smartSizeTolerance = tolerance;
    }

    // Contour filtering parameters for -C option
    int getDigitMinWidth() const {
        return _digitMinWidth;
    }
    void setDigitMinWidth(int width) {
        _digitMinWidth = width;
    }

    double getPerspectiveHeightTolerance() const {
        return _perspectiveHeightTolerance;
    }
    void setPerspectiveHeightTolerance(double tolerance) {
        _perspectiveHeightTolerance = tolerance;
    }

    // AOI (Area of Interest) parameters for -A option
    double getAoiWidthMultiplier() const {
        return _aoiWidthMultiplier;
    }
    void setAoiWidthMultiplier(double multiplier) {
        _aoiWidthMultiplier = multiplier;
    }

    double getAoiMinEdgeDensity() const {
        return _aoiMinEdgeDensity;
    }
    void setAoiMinEdgeDensity(double density) {
        _aoiMinEdgeDensity = density;
    }

    double getAoiMaxEdgeDensity() const {
        return _aoiMaxEdgeDensity;
    }
    void setAoiMaxEdgeDensity(double density) {
        _aoiMaxEdgeDensity = density;
    }

    // Enclosed Area Detection parameters
    bool getEnclosedAreaDetection() const {
        return _enclosedAreaDetection;
    }
    void setEnclosedAreaDetection(bool enabled) {
        _enclosedAreaDetection = enabled;
    }

    double getMinHoleAreaRatio() const {
        return _minHoleAreaRatio;
    }
    void setMinHoleAreaRatio(double ratio) {
        _minHoleAreaRatio = ratio;
    }

private:
    int _rotationDegrees;
    float _ocrMaxDist;
    int _digitMinHeight;
    int _digitMaxHeight;
    int _digitYAlignment;
    int _cannyThreshold1;
    int _cannyThreshold2;
    std::string _trainingDataFilename;
    bool _testMode = false;
    bool _areaOfInterest = false;
    bool _cropDigits = false;
    bool _perspectiveCorrection = false;
    
    // Crop parameters for digit processing
    double _cropPercentHorizontal = 0.1;      // 10% standard horizontal crop
    double _cropPercentVertical = 0.02;       // 2% standard vertical crop
    double _cropPercentHorizontalAOI = 0.15;  // 15% AOI digit horizontal crop
    double _cropPercentVerticalAOI = 0.02;    // 2% AOI digit vertical crop
    
    // Morphological operation parameters for fragment filtering
    int _morphKernelSizeDivisor = 15;          // Kernel size = min(width,height) / divisor (default 15)
    int _morphIterations = 1;                  // Number of dilation/erosion iterations (default 1)
    double _morphSizeRatioThreshold = 0.4;     // Threshold for similar-sized contours (default 0.4)
    
    // Smart fragment filtering parameters for -C option
    double _smartSpacingTolerance = 0.5;       // Spacing variance tolerance (50% of average spacing)
    double _smartSizeTolerance = 0.3;          // Size variance tolerance (30% of average width/height)
    
    // Contour filtering parameters for -C option
    int _digitMinWidth = 5;                    // Minimum width for valid digit contours (default 5px)
    double _perspectiveHeightTolerance = 1.3;  // Additional height tolerance for perspective correction (130%)
    
    // AOI (Area of Interest) parameters for -A option  
    double _aoiWidthMultiplier = 1.20;         // Width multiplier for 7th digit prediction (120%)
    double _aoiMinEdgeDensity = 0.05;          // Minimum edge density for valid digit area (5%)
    double _aoiMaxEdgeDensity = 0.5;           // Maximum edge density for valid digit area (50%)
    
    // Enclosed Area Detection parameters
    bool _enclosedAreaDetection = true;        // Enable/disable enclosed area detection (default: enabled)
    double _minHoleAreaRatio = 0.02;           // Minimum hole area as ratio of parent contour (2%)
};

#endif /* CONFIG_H_ */
