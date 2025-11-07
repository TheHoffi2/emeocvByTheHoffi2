/*
 * DebugOutput.cpp
 *
 * Hilfsfunktionen für Debug-Ausgaben im Testmodus
 */

#include "DebugOutput.h"
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

void DebugOutput::saveDebugImage(const cv::Mat& image, 
                                const std::string& functionName,
                                const std::map<std::string, std::string>& parameters,
                                const std::string& outputDir) {
    if (image.empty()) {
        std::cerr << "Warning: Empty image passed to saveDebugImage for " << functionName << std::endl;
        return;
    }

    // Output-Verzeichnis erstellen falls nicht vorhanden
    createOutputDirectory(outputDir);

    // Dateinamen erstellen
    std::string filename = createFilename(functionName, parameters);
    std::string fullPath = outputDir + "/" + filename + ".png";

    // Bild speichern
    bool success = cv::imwrite(fullPath, image);
    if (success) {
        std::cout << "Debug image saved: " << fullPath << std::endl;
    } else {
        std::cerr << "Error: Could not save debug image: " << fullPath << std::endl;
    }
}

void DebugOutput::createOutputDirectory(const std::string& dir) {
    struct stat st = {0};
    if (stat(dir.c_str(), &st) == -1) {
        mkdir(dir.c_str(), 0755);
    }
}

std::string DebugOutput::createFilename(const std::string& functionName,
                                       const std::map<std::string, std::string>& parameters) {
    std::stringstream ss;
    ss << functionName;
    
    // Parameter hinzufügen
    for (const auto& param : parameters) {
        ss << "_" << param.first << "=" << param.second;
    }
    
    return ss.str();
}