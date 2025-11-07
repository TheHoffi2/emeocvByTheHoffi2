Die Option -A aktiviert die AOI (Area of Interest) Vorhersage für die 7. Ziffer (Dezimalstelle).

Was macht die -A Option?

## Grundprinzip:
Die -A Option erweitert die normale 6-Ziffern-Erkennung um eine intelligente Vorhersage der Position der 7. Ziffer (Dezimalstelle) und fügt diese automatisch hinzu.

## Algorithmus-Ablauf:

# Schritt 1: Analyse der erkannten 6 Ziffern (Analysiere Abstände zwischen den ersten 6 Ziffern)

# Schritt 2: Qualitätskontrolle mit konfigurierbaren Parametern
smartSpacingTolerance: 0.5 # 50% Toleranz für Abstände
smartSizeTolerance: 0.3 # 30% Toleranz für Größen

Prüfung der Regelmäßigkeit:
Spacing-Check: Sind die Abstände zwischen den Ziffern regelmäßig? (±50%)
Size-Check: Sind die Zifferngrößen ähnlich? (±30%)

# Schritt 3: AOI-Berechnung (nur bei regulären Mustern)
aoiWidthMultiplier: 1.2 # 120% der durchschnittlichen Ziffernbreite

# Schritt 4: Edge-Density-Validierung
aoiMinEdgeDensity: 0.05 # Mindestens 5% Kanten erforderlich
aoiMaxEdgeDensity: 0.5 # Maximal 50% Kanten erlaubt

## Praktisches Beispiel:
Eingabe: 6 erkannte Ziffern
[1] [2] [3] [4] [5] [6] ?
│ │ │ │ │ │ │
20 20 21 19 20 20 ?? ← Abstände in Pixeln

# Analyse:
Durchschnittsabstand: 20 Pixel
Spacing-Toleranz: ±10 Pixel (50% von 20)
Alle Abstände im Bereich: 19-21 Pixel ✅
Größen ähnlich: Alle ~30x45 Pixel ✅

# AOI-Vorhersage:
Position X: 6te_Ziffer.x + 6te_Ziffer.width + 20 = 247
Position Y: Durchschnitts-Y = 15
Breite: 30 \* 1.2 = 36 Pixel (20% Sicherheitsmargin)
Höhe: 45 Pixel

# Edge-Density-Check:
AOI-Box: 36x45 = 1620 Pixel
Gefundene Kanten: 364 Pixel
Edge-Density: 364/1620 = 0.22 (22%)
✅ Zwischen 5% und 50% → Gültige Ziffer!

Wann funktioniert -A NICHT?
❌ Unregelmäßige Abstände:
[1] [2][3] [4] [5] [6]
│ │ │ │ │ │
20 2 25 15 20 19 ← Zu unregelmäßig!

❌ Unterschiedliche Größen:
[1] [2] [?] [4] [5] [6]
28x45 30x44 15x20 31x44 28x45 30x44
↑ Fragment! Größe zu unterschiedlich

❌ Leerer Bereich:
Vorhergesagte AOI-Box:
┌─────────────────┐
│ │ Edge-Density: 0.01 (1%)
│ │ ❌ Unter 5% → Abgelehnt
│ │
└─────────────────┘
🔧 Konfigurierbare Aspekte deiner config.yml:
# Smart Fragment Filtering (bestimmt OB AOI aktiviert wird):
smartSpacingTolerance: 0.5 # Wie regelmäßig müssen Abstände sein?
smartSizeTolerance: 0.3 # Wie ähnlich müssen Größen sein?

# AOI-Geometrie (bestimmt WIE die Box berechnet wird):
aoiWidthMultiplier: 1.2 # Wie breit soll die AOI-Box sein?

# AOI-Validierung (bestimmt ob Inhalt gültig ist):
aoiMinEdgeDensity: 0.05 # Mindest-Kantendichte
aoiMaxEdgeDensity: 0.5 # Maximal-Kantendichte

# AOI-Beschneidung (nach erfolgreicher Erkennung):
cropPercentHorizontalAOI: 0.15 # 15% horizontale Beschneidung
cropPercentVerticalAOI: 0.02 # 2% vertikale Beschneidung

## Vorteile der -A Option:
# Automatische 7-Digit-Erkennung
Kein manuelles Definieren der Dezimalstellen-Position nötig
Adaptiert sich an verschiedene Zählertypen und -größen

# Intelligente Qualitätskontrolle
Nur bei regelmäßigen, erkannten 6 Ziffern wird AOI aktiviert
Edge-Density verhindert falsche Positive bei leeren Bereichen

# Robuste Parameter
Alle Schwellwerte sind konfigurierbar
Balance zwischen Sensitivität und Falsch-Positiv-Rate

# Integration mit anderen Features
Funktioniert mit -C (Smart Fragment Filtering)
Nutzt Perspektivkorrektur-Skalierung bei -P
Kompatibel mit Enclosed Area Detection

## Fazit:
Die -A Option ist ein intelligenter "7. Ziffer-Autopilot", der:
Analysiert ob die ersten 6 Ziffern regelmäßig erkannt wurden
Berechnet die wahrscheinliche Position der 7. Ziffer
Validiert ob dort tatsächlich eine Ziffer vorhanden ist
Fügt hinzu die 7. Ziffer zur Erkennung, falls alle Tests bestehen
Das macht die Zählererkennung vollständiger und robuster, ohne manuelle Konfiguration der Dezimalstellen-Position!



# Änderungsdokumentation zur Option -A (Area-of-Interest für die 7. Ziffer)

## Vorteile der Option -A
- **Automatische 7-Digit-Erkennung**: Kein manuelles Definieren der Dezimalstellen-Position nötig.
- **Intelligente Qualitätskontrolle**: Nur bei regelmäßig erkannten 6 Ziffern wird AOI aktiviert.
- **Robuste Parameter**: Alle Schwellwerte sind konfigurierbar.
- **Integration mit anderen Features**: Funktioniert mit -C (Fragment-Filter), -P (Perspektivkorrektur) und Enclosed Area Detection.
- **Vollständige und robuste Zählererkennung**: Die Erkennung wird vollständiger und zuverlässiger.

## Funktionsweise
### Grundprinzip
Die -A Option erweitert die normale 6-Ziffern-Erkennung um eine intelligente Vorhersage der Position der 7. Ziffer (Dezimalstelle) und fügt diese automatisch hinzu.

### Algorithmus-Ablauf
1. **Analyse der erkannten 6 Ziffern**: Analysiere die Abstände und Größen der ersten 6 Ziffern.
2. **Qualitätskontrolle**: Prüfe Regelmäßigkeit der Abstände und Größen mit konfigurierbaren Toleranzen (`smartSpacingTolerance`, `smartSizeTolerance`).
3. **AOI-Berechnung**: Nur bei regulären Mustern wird die AOI-Box für die 7. Ziffer berechnet.
4. **Edge-Density-Validierung**: Prüfe, ob im AOI-Bereich tatsächlich eine Ziffer vorhanden ist (Kantendichte).
5. **Beschneidung**: Die AOI-Ziffer wird mit eigenen Crop-Parametern beschnitten (`cropPercentHorizontalAOI`, `cropPercentVerticalAOI`).

### Beispielhafte Parameter
```yaml
smartSpacingTolerance: 0.5   # Wie regelmäßig müssen Abstände sein?
smartSizeTolerance: 0.3      # Wie ähnlich müssen Größen sein?
aoiWidthMultiplier: 1.2      # Wie breit soll die AOI-Box sein?
aoiMinEdgeDensity: 0.05      # Mindest-Kantendichte
aoiMaxEdgeDensity: 0.5       # Maximal-Kantendichte
cropPercentHorizontalAOI: 0.15 # 15% horizontale Beschneidung
cropPercentVerticalAOI: 0.02   # 2% vertikale Beschneidung
```

## Beispielhafte Nutzung
```bash
./emeocv -c 0 -A -C -d -t
./emeocv -i /path/to/images -A -C -t
```

## Kompatibilität
- Funktioniert mit -C (Smart Fragment Filtering)
- Kompatibel mit Debug-Modus (-d) für Visualisierung
- Integriert mit Konfigurationssystem
- Rückwärtskompatibel (off by default)

## Code-Änderungen (Dokumentation)
### Hauptprogramm (`main.cpp`)
- Erweiterung der Kommandozeilenoptionen um `-A`:
	```cpp
	std::cout << "  -A : Enable Area-of-Interest for 7th digit prediction (decimal place).\n";
	```
- Logik zur AOI-Erkennung und Validierung integriert.
- Crop-Parameter für AOI werden aus Konfiguration geladen und angewendet.

### Konfigurationsdateien (`config.yml`, `CROP_PARAMETERS.md`)
- Neue Parameter für AOI-Erkennung und Beschneidung hinzugefügt.
- Dokumentation der Parameter und Beispielwerte.

### Bildverarbeitung (`ImageProcessor.cpp`)
- Implementierung der AOI-Vorhersage und Kantendichte-Prüfung.
- Integration mit Fragment-Filterung (-C Option).

### Debug-Ausgabe (`DebugOutput.cpp`)
- Visualisierung der AOI-Box und der erkannten 7. Ziffer.

## Fazit
Die Option `-A` bietet eine intelligente und automatische Erkennung der Dezimalstelle auf Stromzählerbildern. Sie erhöht die Zuverlässigkeit und Flexibilität der Zählerstandserkennung und ist mit anderen Optionen kombinierbar.

---

*Letzte Änderung: 18.10.2025*
