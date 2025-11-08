# Die Option -H: HDRI-Modus für bessere Zählerbilder

## Beschreibung und Vorteile

Die Option `-H` aktiviert den HDRI-Modus (High Dynamic Range Imaging) für die Bildaufnahme mit der Raspberry Pi Kamera. Im HDR-Modus werden mehrere Belichtungen kombiniert, um auch bei schwierigen Lichtverhältnissen (z.B. Gegenlicht, Schatten) optimale Ergebnisse zu erzielen. Die Integration in emeocv ermöglicht eine robustere Ziffernerkennung bei wechselnden Lichtbedingungen und verbessert die Bildqualität deutlich.

**Vorteile:**
- Bessere Lesbarkeit von Ziffern bei schwierigen Lichtverhältnissen
- Höherer Dynamikumfang, weniger Über- oder Unterbelichtung
- Supersampling durch natives Downscaling auf 640x480
- Automatische Aktivierung spezieller Kamera-Parameter

## Aktivierung und Nutzung

Die Option `-H` kann beim Programmstart angegeben werden:

```bash
./emeocv -c 0 -H -A -C -d -t
./emeocv -i /pfad/zu/bildern -H -A -C -t
```

## Codeänderungen

- **main.cpp**
  - Option `-H` wird in der Kommandozeile erkannt und als `useHdri` an die Konfiguration übergeben.
  - Hilfetext und Usage-Ausgabe um `-H` erweitert:
    ```cpp
    std::cout << "  -H : Enable HDR mode for better contrast and higher resolution (Pi Camera only).\n";
    ```
- **CameraInput**
  - Neuer Parameter `useHdri` im Konstruktor und in der Konfiguration.
  - Im HDR-Modus werden spezielle Parameter an `rpicam-still` übergeben:
    - `--hdr auto`, `--zsl`, längeres Timeout, Autofokus direkt vor Aufnahme
  - Nach HDR-Aufnahme wird das hochauflösende Bild auf 640x480 herunterskaliert (Supersampling):
    ```cpp
    if (_useHdri) {
        cmd += " --timeout 3000";
        cmd += " --hdr auto";
        cmd += " --zsl";
        cmd += " --autofocus-on-capture 1";
        // ...
        // Downscaling auf 640x480
        cv::resize(originalImg, _img, cv::Size(640, 480));
    }
    ```
  - Siehe Datei: `ImageInput.cpp`, Methode `CameraInput::nextImage()`

## Kompatibilität

- Funktioniert nur mit Pi Cam 3 (Adu Cam IMX519) und aktiviertem `rpicam-still`.
- Kann mit anderen Optionen wie `-A` (Area-of-Interest), `-C` (Digit Cropping) und `-d` (Debug-Modus) kombiniert werden.
- Rückwärtskompatibel: Ohne `-H` bleibt das Verhalten wie bisher.

## Fazit

Die Option `-H` bietet eine robuste Möglichkeit, die Bildqualität und die Erkennungsrate bei schwierigen Lichtverhältnissen zu verbessern. Sie ist flexibel konfigurierbar und lässt sich mit anderen Optionen kombinieren.

---

*Letzte Änderung: 17.10.2025*