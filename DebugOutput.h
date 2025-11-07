/*
 * DebugOutput.h
 *
 * Hilfsfunktionen für Debug-Ausgaben im Testmodus
 */

#ifndef DEBUGOUTPUT_H_
#define DEBUGOUTPUT_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <map>

class DebugOutput {
public:
    // Speichert ein Bild mit Funktionsnamen und Parametern im Dateinamen
    // Beispiel: "ImageProcessor_resize_width=100_height=200.png"
    static void saveDebugImage(const cv::Mat& image, 
                              const std::string& functionName,
                              const std::map<std::string, std::string>& parameters,
                              const std::string& outputDir = "debug_output");

    // Hilfsfunktion zum Erstellen des Output-Verzeichnisses
    static void createOutputDirectory(const std::string& dir);

private:
    // Erstellt einen Dateinamen aus Funktionsnamen und Parametern
    static std::string createFilename(const std::string& functionName,
                                    const std::map<std::string, std::string>& parameters);
};

#endif /* DEBUGOUTPUT_H_ */