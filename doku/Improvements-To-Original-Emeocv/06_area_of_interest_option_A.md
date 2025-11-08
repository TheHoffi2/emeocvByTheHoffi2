# Die Option -A: Area-of-Interest für die 7. Ziffer (Dezimalstelle)

## Beschreibung und Vorteile

Die Option `-A` aktiviert die intelligente AOI (Area of Interest) Vorhersage für die automatische Erkennung der 7. Ziffer (Dezimalstelle) auf Stromzählerbildern. Diese Funktion erweitert die Standard-Erkennung von 6 Ziffern um eine präzise Vorhersage und Validierung einer zusätzlichen Dezimalstelle, ohne dass manuelle Konfiguration erforderlich ist.

**Vorteile:**
- **Automatische 7-Digit-Erkennung**: Kein manuelles Definieren der Dezimalstellen-Position nötig
- **Intelligente Qualitätskontrolle**: Nur bei regelmäßig erkannten 6 Ziffern wird AOI aktiviert
- **Robuste Parameter**: Alle Schwellwerte sind konfigurierbar für verschiedene Zählertypen
- **Integration mit anderen Features**: Funktioniert nahtlos mit -C (Fragment-Filter), -P (Perspektivkorrektur) und Enclosed Area Detection
- **Erhöhte Präzision**: Vollständigere und zuverlässigere Zählerstandserkennung
- **Adaptive Erkennung**: Passt sich automatisch an verschiedene Zählertypen und -größen an

## Funktionsweise

### 1. Aktivierung über Kommandozeile
Die Option `-A` kann beim Programmstart angegeben werden:

```bash
./emeocv -c 0 -A -C -d -t
./emeocv -i /pfad/zu/bildern -A -C -t
```

### 2. Algorithmischer Ablauf

**Schritt 1: Analyse der erkannten 6 Ziffern**
- Berechnung der Abstände zwischen den ersten 6 erkannten Ziffern
- Ermittlung der durchschnittlichen Zifferngrößen (Breite und Höhe)
- Bestimmung der Y-Position-Konsistenz

**Schritt 2: Qualitätskontrolle mit konfigurierbaren Parametern**
- **Spacing-Check**: Prüfung der Regelmäßigkeit der Abstände (±50% Toleranz, konfigurierbar via `smartSpacingTolerance`)
- **Size-Check**: Validierung der Größenähnlichkeit (±30% Toleranz, konfigurierbar via `smartSizeTolerance`)
- Nur bei bestandener Qualitätskontrolle wird mit der AOI-Vorhersage fortgefahren

**Schritt 3: AOI-Berechnung (nur bei regulären Mustern)**
- Position X: `6te_Ziffer.x + 6te_Ziffer.width + durchschnittlicher_Abstand`
- Position Y: Durchschnitts-Y-Position aller erkannten Ziffern
- Breite: `durchschnittliche_Ziffernbreite * aoiWidthMultiplier` (Standard: 120%)
- Höhe: Durchschnittliche Ziffernhöhe

**Schritt 4: Edge-Density-Validierung**
- Extraktion des vorhergesagten AOI-Bereichs
- Berechnung der Kantendichte: `kantenPixel / gesamtPixel`
- Validierung gegen konfigurierbare Schwellwerte (`aoiMinEdgeDensity`: 5%, `aoiMaxEdgeDensity`: 50%)
- Nur bei gültiger Kantendichte wird die 7. Ziffer zur Erkennung hinzugefügt

**Schritt 5: Spezielle AOI-Beschneidung**
- AOI-Ziffer wird mit eigenen Crop-Parametern beschnitten
- `cropPercentHorizontalAOI`: 15% (vs. 10% für normale Ziffern)
- `cropPercentVerticalAOI`: 2% (gleich wie normale Ziffern)

### 3. Praktisches Beispiel

**Eingabe:** 6 erkannte Ziffern
```
[1] [2] [3] [4] [5] [6] ?
│   │   │   │   │   │   │
20  20  21  19  20  20  ?? ← Abstände in Pixeln
```

**Analyse:**
- Durchschnittsabstand: 20 Pixel
- Spacing-Toleranz: ±10 Pixel (50% von 20)
- Alle Abstände im Bereich: 19-21 Pixel ✅
- Größen ähnlich: Alle ~30x45 Pixel ✅

**AOI-Vorhersage:**
- Position X: 247 (nach 6. Ziffer + Abstand)
- Position Y: 15 (Durchschnitts-Y)
- Breite: 36 Pixel (30 * 1.2)
- Höhe: 45 Pixel

**Edge-Density-Check:**
- AOI-Box: 36x45 = 1620 Pixel
- Gefundene Kanten: 364 Pixel
- Edge-Density: 22% ✅ (zwischen 5% und 50%)

## Codeänderungen

### **main.cpp**
- Option `-A` wird in der Kommandozeile erkannt und als `areaOfInterest` an die Konfiguration übergeben
- Erweiterte Hilfetext-Ausgabe

**Wichtige Codeausschnitte:**
```cpp
while ((opt = getopt(argc, argv, "i:c:ltaws:o:v:hdHACP")) != -1) {
    switch (opt) {
        // ...
        case 'A':
            areaOfInterest = true;
            break;
        // ...
    }
}
config.setAreaOfInterest(areaOfInterest);

// In usage():
std::cout << "  -A : Enable Area-of-Interest for 7th digit prediction (decimal place).\n";
```

### **Config.h/Config.cpp**
- Neues Flag `_areaOfInterest` mit Getter/Setter-Methoden
- Umfangreiche neue Parameter für AOI-Funktionalität
- Persistierung aller Parameter in `config.yml`

**Neue Parameter:**
```cpp
// Smart Fragment Filtering (bestimmt OB AOI aktiviert wird)
double _smartSpacingTolerance = 0.5;       // 50% Toleranz für Abstände
double _smartSizeTolerance = 0.3;          // 30% Toleranz für Größen

// AOI-Geometrie (bestimmt WIE die Box berechnet wird)
double _aoiWidthMultiplier = 1.20;         // 120% der durchschnittlichen Ziffernbreite

// AOI-Validierung (bestimmt ob Inhalt gültig ist)
double _aoiMinEdgeDensity = 0.05;          // Mindestens 5% Kanten erforderlich
double _aoiMaxEdgeDensity = 0.5;           // Maximal 50% Kanten erlaubt

// AOI-Beschneidung (nach erfolgreicher Erkennung)
double _cropPercentHorizontalAOI = 0.15;   // 15% horizontale Beschneidung
double _cropPercentVerticalAOI = 0.02;     // 2% vertikale Beschneidung
```

### **ImageProcessor.cpp**
- Umfangreiche Erweiterung der `findCounterDigits()` Funktion
- Implementierung der kompletten AOI-Logik
- Integration mit bestehender Crop- und Fragment-Filterung

**Kern-Implementierung:**
```cpp
// Area of Interest approach: If enabled and we have exactly 6 boxes with regular spacing,
// predict the 7th digit (decimal place) position
if (_config.getAreaOfInterest() && alignedBoundingBoxes.size() == 6) {
    rlog << log4cpp::Priority::INFO << "=== AREA OF INTEREST FOR 7TH DIGIT ===";
    
    // Check if the 6 boxes have regular spacing and similar sizes
    bool regularSpacing = true;
    bool similarSizes = true;
    
    // Calculate average spacing between digits
    std::vector<int> spacings, widths, heights;
    for (size_t i = 0; i < alignedBoundingBoxes.size(); i++) {
        widths.push_back(alignedBoundingBoxes[i].width);
        heights.push_back(alignedBoundingBoxes[i].height);
        
        if (i > 0) {
            int spacing = alignedBoundingBoxes[i].x - 
                         (alignedBoundingBoxes[i-1].x + alignedBoundingBoxes[i-1].width);
            spacings.push_back(spacing);
        }
    }
    
    // Validate regularity using configurable tolerances
    // ... (Spacing und Size Validation)
    
    if (regularSpacing && similarSizes && avgSpacing > 0) {
        // Predict 7th digit position
        cv::Rect predictedDecimalBox(decimalsX, decimalsY, decimalsWidth, decimalsHeight);
        
        // Edge-density validation
        cv::Mat decimalsArea = img_ret(predictedDecimalBox);
        double edgeDensity = (double)cv::countNonZero(decimalsArea) / 
                           (decimalsWidth * decimalsHeight);
        
        if (edgeDensity >= _config.getAoiMinEdgeDensity() && 
            edgeDensity <= _config.getAoiMaxEdgeDensity()) {
            alignedBoundingBoxes.push_back(predictedDecimalBox);
            // Re-sort with the new 7th digit
            std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());
        }
    }
}
```

**Spezielle AOI-Beschneidung:**
```cpp
// Special cropping for AOI (7th digit): 15% horizontal, 2% vertical
bool isAOIDigit = (_config.getAreaOfInterest() && 
                   alignedBoundingBoxes.size() == 7 && 
                   i == alignedBoundingBoxes.size() - 1);

if (isAOIDigit) {
    finalRoi = cropRectangleCustom(roi, _config.getCropPercentHorizontalAOI(), 
                                 _config.getCropPercentVerticalAOI(), img_ret.size());
} else {
    finalRoi = cropRectangle(roi, 0.0, img_ret.size());
}
```

## Konfigurationsparameter

### Smart Fragment Filtering Parameter
```yaml
smartSpacingTolerance: 0.5   # Wie regelmäßig müssen Abstände sein? (Standard: 50%)
smartSizeTolerance: 0.3      # Wie ähnlich müssen Größen sein? (Standard: 30%)
```

### AOI-Geometrie Parameter
```yaml
aoiWidthMultiplier: 1.2      # Breite der AOI-Box relativ zur Ziffernbreite (Standard: 120%)
```

### AOI-Validierung Parameter
```yaml
aoiMinEdgeDensity: 0.05      # Mindest-Kantendichte für gültige Ziffer (Standard: 5%)
aoiMaxEdgeDensity: 0.5       # Maximal-Kantendichte für gültige Ziffer (Standard: 50%)
```

### AOI-Beschneidung Parameter
```yaml
cropPercentHorizontalAOI: 0.15 # Horizontale Beschneidung für AOI-Ziffer (Standard: 15%)
cropPercentVerticalAOI: 0.02   # Vertikale Beschneidung für AOI-Ziffer (Standard: 2%)
```

## Kompatibilität und Integration

- **Vollständig kompatibel** mit allen anderen Optionen (`-C`, `-P`, `-H`, `-d`)
- **Smart Fragment Filtering Integration**: Teilt Qualitätskontroll-Parameter mit der -C Option
- **Perspektivkorrektur-Support**: Automatische Anpassung an Skalierungsfaktoren bei -P
- **Debug-Modus**: Umfangreiche Visualisierung und Logging im Testmodus (`-d`)
- **Rückwärtskompatibel**: Ohne `-A` bleibt das Verhalten vollständig unverändert

## Debug-Ausgaben und Logging

**Log-Informationen:**
- Detaillierte Spacing- und Size-Analyse der ersten 6 Ziffern
- AOI-Berechnungen mit Positionsangaben
- Edge-Density-Messungen und Validierungsergebnisse
- Verwendete Konfigurationsparameter

**Debug-Bilder:**
- `ImageProcessor_digit_6_crop=15pct_width_2pct_height_AOI.png` (AOI-Ziffer)
- `ImageProcessor_digit_0-5_crop=10pct_width_2pct_height.png` (normale Ziffern)
- Unterscheidung zwischen AOI und Standard-Ziffern in Dateinamen

## Anwendungsszenarien

### Wann funktioniert -A optimal:
✅ **Regelmäßige Zähler**: Gleichmäßige Abstände und ähnliche Zifferngrößen  
✅ **Klare Dezimalstelle**: Gut sichtbare 7. Ziffer im erwarteten Bereich  
✅ **Stabile Bildqualität**: Ausreichend Kontrast für Edge-Density-Validierung

### Wann funktioniert -A NICHT:
❌ **Unregelmäßige Abstände**: Zu große Varianz zwischen Ziffernabständen  
❌ **Unterschiedliche Größen**: Fragmente oder beschädigte Ziffern stören die Analyse  
❌ **Leerer Dezimalbereich**: Keine oder zu schwache Kanten im vorhergesagten Bereich  
❌ **Nur 5 oder weniger Ziffern**: Basis-Algorithmus benötigt exakt 6 erkannte Ziffern

## Fazit

Die Option `-A` erweitert emeocv um eine hochintelligente Funktionalität zur automatischen Erkennung der Dezimalstelle. Sie kombiniert statistische Analyse, geometrische Vorhersage und Kantendichte-Validierung zu einem robusten System, das sich an verschiedene Zählertypen anpasst. Die vollständige Integration in das bestehende System und die umfangreichen Konfigurationsmöglichkeiten machen sie zu einem wertvollen Werkzeug für präzise Stromzählerablesung.

---

*Letzte Änderung: 18.10.2025*