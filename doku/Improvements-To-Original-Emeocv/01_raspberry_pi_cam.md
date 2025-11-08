# Umstellung auf Raspberry Pi Cam 3 (Adu Cam IMX519)

## Beschreibung und Vorteile

Die ursprüngliche Version von emeocv verwendete eine einfache USB 2.0 Mini Web Cam (Seeed Studio) mit niedriger Auflösung und ohne Autofokus. Die neue Version nutzt nun die Raspberry Pi Cam 3 (Adu Cam IMX519), die über eine deutlich höhere native Auflösung, Autofokus und bessere Bildqualität verfügt. Die Ansteuerung erfolgt über das Tool `rpicam-still` (libcamera), da OpenCV mit modernen Pi-Kameras nicht zuverlässig funktioniert.

**Vorteile:**
- Deutlich höhere Bildqualität und Auflösung
- Autofokus für scharfe Bilder
- Bessere Low-Light-Performance
- Zuverlässigere Bildaufnahme durch native Unterstützung

## Codeänderungen

- Die Klasse `CameraInput` wurde angepasst, um die Pi Cam 3 über `rpicam-still` anzusteuern.
- Fallback auf OpenCV V4L2 nur noch, falls rpicam nicht funktioniert.
- Test, ob rpicam funktioniert, beim Start.
- Simulationsmodus mit statischem Testbild, falls keine Kamera verfügbar ist.

**Wichtige Codeausschnitte:**

```cpp
// CameraInput::CameraInput(int device, bool useHdri)
_useRpicam = true;
std::string testCmd = "rpicam-still -o /tmp/pi_camera_test.jpg ...";
int result = system(testCmd.c_str());
if (result == 0) { ... }
else { ... }
// Fallback auf OpenCV V4L2
```

Siehe Datei: `ImageInput.cpp`, Klasse `CameraInput`
