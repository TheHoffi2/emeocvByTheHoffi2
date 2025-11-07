# Detaillierter Testausgabe-Modus (Debug/Test-Mode)

## Beschreibung und Nutzen

Im neuen Testmodus werden während der Bildverarbeitung in jedem Teilschritt Zwischenergebnisse als Bilddateien gespeichert. Dies erleichtert die Fehlersuche und Optimierung des Algorithmus erheblich, da jeder Verarbeitungsschritt nachvollzogen werden kann.

**Nutzen:**
- Nachvollziehbare Bildverarbeitung durch gespeicherte Zwischenergebnisse
- Einfaches Debugging und Vergleich von Algorithmen
- Automatisierte Testszenarien möglich

## Codeänderungen

- Neuer Parameter `testMode` in der Klasse `Config` (wird in `config.yml` gespeichert).
- Die Klasse `DebugOutput` speichert Bilder mit Funktionsnamen und Parametern im Dateinamen.
- In `ImageProcessor` werden im Testmodus nach jedem wichtigen Schritt (z.B. Graustufen, Rotation, Canny-Edges, Ziffernextraktion) Bilder gespeichert.

**Wichtige Codeausschnitte:**

```cpp
// Beispiel: Graustufenbild speichern
if (_config.getTestMode()) {
    std::map<std::string, std::string> params;
    DebugOutput::saveDebugImage(_imgGray, "ImageProcessor_grayscale", params);
}
// ... weitere Schritte analog
```

Siehe Dateien: `Config.cpp`, `ImageProcessor.cpp`, `DebugOutput.cpp`
