````markdown
# Die Option -P: Perspektivkorrektur für schräge Kamerawinkel

## Beschreibung und Vorteile

Die Option `-P` aktiviert die automatische Perspektivkorrektur für schräg aufgenommene Zählerbilder. Wenn die Kamera nicht perfekt senkrecht vor dem Zähler positioniert ist, entstehen perspektivische Verzerrungen, die die Ziffernerkennung erheblich beeinträchtigen können. Die Perspektivkorrektur erkennt automatisch die Ecken der Zähleranzeige und transformiert das Bild in eine frontale Ansicht.

**Vorteile:**
- Automatische Korrektur von perspektivischen Verzerrungen
- Verbesserte Ziffernerkennung bei ungünstigen Kamerawinkeln  
- Flexibilität bei der Kamerapositionierung
- Erhöhung der Robustheit gegenüber unterschiedlichen Montagegegebenheiten
- Bessere Ergebnisse bei schwer zugänglichen Zählerpositionen
- Reduzierung von senkrecht aufeineanderstehenden Spiegelungen

## Funktionsweise

### 1. Aktivierung über Kommandozeile
Die Option `-P` kann beim Programmstart angegeben werden:

```bash
./emeocv -c 0 -P -A -C -d -t
./emeocv -i /pfad/zu/bildern -P -A -C -t
```

### 2. Algorithmischer Ablauf

**Schritt 1: Eckenerkennung**
- Verwendung der Canny-Edge-Detection zur Kantenerkennung
- Analyse aller Konturen im Bild
- Identifikation großer rechteckiger Strukturen (Zählerrahmen)
- Bewertung nach Flächengröße und Bildanteil

**Schritt 2: Homographie-Berechnung**
- Bestimmung der vier Eckpunkte des Zählerbereichs
- Berechnung der Homographie-Matrix zur Perspektivtransformation
- Ermittlung der Skalierungsfaktoren für nachgelagerte Verarbeitung

**Schritt 3: Bildtransformation**
- Anwendung der Perspektivkorrektur mittels `cv::warpPerspective()`
- Transformation sowohl des Graustufen- als auch des Farbbildes
- Anpassung der Parameter für die nachfolgende Ziffernerkennung

### 3. Intelligente Fallback-Strategien

**Primärstrategie: Konturerkennung**
- Suche nach großen Konturen (>10% der Bildfläche)
- Mindestbreite von 50% und Mindesthöhe von 30% des Bildes
- Verwendung der Bounding-Box als Perspektivreferenz

**Fallback-Strategie: Größte Kontur**
- Bei unzureichender Haupterkennung: Verwendung der größten verfügbaren Kontur
- Lockerung der Kriterien auf 8% Mindestfläche und 45% Mindestbreite

**Ultimate Fallback: Bildränder**
- Falls keine geeigneten Konturen gefunden werden
- Verwendung der Bildecken mit 5% Inset als Referenzpunkte
- Gewährleistet Funktionsfähigkeit auch bei sehr schwierigen Bildern

## Codeänderungen

### **main.cpp**
- Option `-P` wird in der Kommandozeile erkannt (`getopt`) und als `perspectiveCorrection` an die Konfiguration übergeben
- Hilfetext wird entsprechend erweitert (noch zu implementieren in usage())

**Wichtige Codeausschnitte:**
```cpp
while ((opt = getopt(argc, argv, "i:c:ltaws:o:v:hdHACP")) != -1) {
    switch (opt) {
        // ...
        case 'P':
            perspectiveCorrection = true;
            break;
        // ...
    }
}
config.setPerspectiveCorrection(perspectiveCorrection);
```

### **Config.h/Config.cpp**
- Neues Flag `_perspectiveCorrection` mit Getter/Setter
- Persistierung in `config.yml`
- Standardwert: `false` (rückwärtskompatibel)

**Code-Erweiterungen:**
```cpp
// Config.h
bool getPerspectiveCorrection() const { return _perspectiveCorrection; }
void setPerspectiveCorrection(bool mode) { _perspectiveCorrection = mode; }

// Config.cpp - Speichern/Laden
fs << "perspectiveCorrection" << _perspectiveCorrection;
if (!fs["perspectiveCorrection"].empty()) fs["perspectiveCorrection"] >> _perspectiveCorrection;
```

### **ImageProcessor.h/ImageProcessor.cpp**
- Neue Methoden `correctPerspective()` und `detectMeterCorners()`
- Skalierungsfaktoren `_perspectiveScaleX/Y` für nachgelagerte Parameteranpassung
- Flag `_perspectiveCorrectionApplied` zur Steuerung angepasster Toleranzen

**Hauptfunktionalität:**
```cpp
void ImageProcessor::correctPerspective() {
    if (!_config.getPerspectiveCorrection()) {
        return; // Überspringe wenn deaktiviert
    }
    
    // Eckenerkennung
    std::vector<cv::Point2f> srcCorners = detectMeterCorners();
    
    if (srcCorners.size() == 4) {
        // Zielrechteck definieren
        std::vector<cv::Point2f> dstCorners = { /* ... */ };
        
        // Homographie berechnen und anwenden
        cv::Mat homography = cv::getPerspectiveTransform(srcCorners, dstCorners);
        cv::warpPerspective(_imgGray, corrected, homography, _imgGray.size());
        _imgGray = corrected;
        
        // Skalierungsfaktoren für nachgelagerte Verarbeitung
        _perspectiveScaleX = (double)targetWidth / originalBounds.width;
        _perspectiveScaleY = (double)targetHeight / originalBounds.height;
        _perspectiveCorrectionApplied = true;
    }
}
```

**Adaptive Parameteranpassung:**
```cpp
// In filterContours() und findAlignedBoxes()
if (_perspectiveCorrectionApplied) {
    scaledMinHeight = (int)(_config.getDigitMinHeight() * _perspectiveScaleY);
    scaledMaxHeight = (int)(_config.getDigitMaxHeight() * _perspectiveScaleY * 
                           _config.getPerspectiveHeightTolerance()); // 130% Toleranz
    scaledYAlignment = (int)(_config.getDigitYAlignment() * _perspectiveScaleY * 1.5);
}
```

## Kompatibilität und Integration

- **Vollständig kompatibel** mit allen anderen Optionen (`-A`, `-C`, `-H`, `-d`)
- **Reihenfolge**: Perspektivkorrektur erfolgt **vor** Rotation und Skew-Korrektur
- **Skalierungsanpassung**: Alle nachgelagerten Parameter werden automatisch an die neue Bildgeometrie angepasst
- **Debug-Modus**: Im Testmodus (`-d`) werden korrigierte Bilder gespeichert
- **Rückwärtskompatibel**: Ohne `-P` bleibt das Verhalten unverändert

## Debug-Ausgaben und Logging

**Log-Informationen:**
- Anzahl gefundener Konturen und deren Eigenschaften
- Verwendete Eckpunkte für die Perspektivkorrektur
- Berechnete Skalierungsfaktoren
- Angepasste Parameter für die Ziffernerkennung

**Debug-Bilder:**
- `ImageProcessor_perspective_corrected_method=homography_corners=4.png`
- Visualisierung der erkannten Eckpunkte
- Vorher-Nachher-Vergleich der Transformation

## Fazit

Die Option `-P` erweitert emeocv um eine wichtige Funktionalität zur automatischen Perspektivkorrektur. Sie ermöglicht eine flexiblere Kamerapositionierung und verbessert die Robustheit der Ziffernerkennung bei ungünstigen Aufnahmewinkeln. Die Implementierung ist vollständig in das bestehende System integriert und berücksichtigt die Auswirkungen der Perspektivkorrektur auf alle nachgelagerten Verarbeitungsschritte.

---

*Letzte Änderung: 18.10.2025*
````