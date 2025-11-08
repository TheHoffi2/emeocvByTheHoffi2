# Die Option -C: Digit Cropping und Fragment-Filterung

## Motivation und Vorteile

Zähler mit silbernem Frontblech erzeugen harte Rahmen um die Ziffernfelder. Diese Rahmen führen zu Störfragmenten, die die Ziffernerkennung erschweren. Die Option `-C` wurde eingeführt, um diese Störfragmente zu entfernen und die Ziffernfelder gezielt zu bereinigen.

**Vorteile:**
- Entfernt störende Rahmen und Fragmente um die Ziffern
- Verbessert die OCR-Erkennung durch saubere Ziffernbilder
- Kompatibel mit Area-of-Interest (`-A`) und Debug-Modus (`-d`)
- Rückwärtskompatibel: Standardmäßig deaktiviert

## Funktionsweise

### 1. Aktivierung über Kommandozeile
Die Option `-C` kann beim Programmstart angegeben werden:

```bash
./emeocv -c 0 -A -C -d -t
./emeocv -i /pfad/zu/bildern -A -C -t
```

### 2. Erweiterung der Konfiguration
- Neues Flag `cropDigits` in `Config.h`/`Config.cpp`
- Persistenz in `config.yml`
- Zusätzliche Parameter für Cropping und Morphologie (z.B. Kernelgröße, Iterationen)

### 3. Änderungen in der Hauptanwendung
- Option `-C` wird in `main.cpp` geparst und an die Konfiguration übergeben
- Hilfetext und Usage-Ausgabe um `-C` erweitert

### 4. Bildverarbeitung: Digit Cropping & Fragment-Filter
- In `ImageProcessor.cpp` werden die Ziffernrechtecke nach dem Erkennen beschnitten (asymmetrisch: 10% Breite, 2-3% Höhe)
- Nach dem Cropping werden Fragmente mit einer neuen Funktion `filterFragments()` entfernt:
  - **Dilate:** Kanten werden verdickt, sodass Ziffer und Fragmente zu Konturen verschmelzen
  - **Find Contours:** Alle Konturen werden gefunden
  - **Select Largest:** Nur die größte Kontur (Ziffer) bleibt erhalten
  - **Apply Mask:** Kleinere Fragmente werden entfernt
  - **Erode:** Ursprüngliche Kantenstärke wird wiederhergestellt
- Parameter für Morphologie sind dynamisch und abhängig von der Bildgröße
- Debug-Ausgabe zeigt an, ob Cropping und Fragmentfilter aktiv waren (`crop=10pct_width_3pct_height`, `filter=fragments_removed`)

### 5. Kompatibilität
- Funktioniert mit und ohne Area-of-Interest (`-A`)
- Debug-Modus (`-d`) zeigt Zwischenschritte und speichert Debug-Bilder
- Rückwärtskompatibel: Ohne `-C` bleibt das Verhalten wie bisher

## Beispiel für die neue Hilfeausgabe

```
Options:
  -C : Enable digit cropping and fragment filtering (configurable crop percentages + morphological noise removal).
```

## Technische Details der Codeänderungen

- **Config.h/Config.cpp:**
  - Neues Flag `cropDigits` und zugehörige Getter/Setter
  - Zusätzliche Parameter für Cropping und Morphologie
- **main.cpp:**
  - Option `-C` wird erkannt und an die Konfiguration übergeben
  - Usage/Hilfe um `-C` erweitert
- **ImageProcessor.h/Cpp:**
  - Neue Methode `filterFragments(const cv::Mat&)`
  - Cropping und Fragmentfilter werden im Digit-Extraktionsprozess angewendet
  - Debug-Ausgabe mit neuen Parametern

## Fazit
Die Option `-C` bietet eine robuste Möglichkeit, störende Rahmen und Fragmente aus den Ziffernbildern zu entfernen. Sie verbessert die Erkennungsrate und ist flexibel konfigurierbar. Die Implementierung ist rückwärtskompatibel und lässt sich mit anderen Optionen kombinieren.

---

*Letzte Änderung: 17.10.2025*
