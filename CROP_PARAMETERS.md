# Konfigurierbare Crop-Parameter

Die Option `-C` (Digit Cropping) verwendet jetzt kon## Smart Fragment Filtering Parameter (-C Option)

Die Smart Fragment Filtering Funktion verwendet intelligente Algorithmen zur Erkennung regulärer Ziffern-Muster:

### Verfügbare Parameter

#### Spacing-Analyse
- **smartSpacingTolerance**: Toleranz für Abstands-Varianz (Standard: 0.5 = 50%)
  - Bestimmt wie ähnlich die Abstände zwischen Ziffern sein müssen
  - Niedrigere Werte = strengere Anforderungen an regelmäßige Abstände

#### Größen-Analyse  
- **smartSizeTolerance**: Toleranz für Größen-Varianz (Standard: 0.3 = 30%)
  - Bestimmt wie ähnlich Breite und Höhe der Ziffern sein müssen
  - Niedrigere Werte = strengere Anforderungen an einheitliche Größen

### Funktionsweise der Smart Fragment Filtering

1. **Spacing-Prüfung**: Analysiert Abstände zwischen erkannten Ziffern
2. **Größen-Prüfung**: Vergleicht Breite und Höhe aller Ziffern  
3. **Regularität**: Nur bei regelmäßigen Mustern wird AOI-Vorhersage aktiviert
4. **Robustheit**: Filtert falsche Positive bei unregelmäßigen Zählwerken

## Area of Interest (AOI) Parameter (-A Option)

Die AOI-Funktion sagt die Position der 7. Ziffer (Dezimalstelle) vorher:

### Verfügbare Parameter

#### Geometrie-Parameter
- **aoiWidthMultiplier**: Breiten-Multiplikator für 7. Ziffer (Standard: 1.2 = 120%)
  - Bestimmt die Breite des vorhergesagten Bereichs relativ zur durchschnittlichen Ziffernbreite
  
#### Edge-Density-Parameter  
- **aoiMinEdgeDensity**: Minimale Kanten-Dichte für gültige Ziffer (Standard: 0.05 = 5%)
- **aoiMaxEdgeDensity**: Maximale Kanten-Dichte für gültige Ziffer (Standard: 0.5 = 50%)
  - Bestimmt den Bereich gültiger Kantenanteile im vorhergesagten Bereich

### Funktionsweise der AOI-Vorhersage

1. **Voraussetzung**: Smart Fragment Filtering muss Regularität bestätigen
2. **Positionsberechnung**: 7. Ziffer nach durchschnittlichem Abstand platziert
3. **Größenberechnung**: Breite mit `aoiWidthMultiplier` skaliert
4. **Validierung**: Edge-Density muss im konfigurierten Bereich liegen
5. **Integration**: Gültige AOI-Ziffer wird zu den erkannten Ziffern hinzugefügt

## Konfigurationsbeispiele

### Konservative Einstellungen (hohe Präzision)
```yaml
smartSpacingTolerance: 0.3        # Strengere Abstands-Anforderungen
smartSizeTolerance: 0.2           # Strengere Größen-Anforderungen
aoiWidthMultiplier: 1.1           # Schmalere AOI-Box
aoiMinEdgeDensity: 0.08           # Höhere Mindest-Kantendichte
aoiMaxEdgeDensity: 0.4            # Niedrigere Max-Kantendichte
```

### Permissive Einstellungen (höhere Erkennungsrate)
```yaml
smartSpacingTolerance: 0.7        # Tolerantere Abstands-Anforderungen  
smartSizeTolerance: 0.4           # Tolerantere Größen-Anforderungen
aoiWidthMultiplier: 1.3           # Breitere AOI-Box
aoiMinEdgeDensity: 0.03           # Niedrigere Mindest-Kantendichte
aoiMaxEdgeDensity: 0.6            # Höhere Max-Kantendichte
```

### Empfohlene Bereiche

- **smartSpacingTolerance**: 0.2 - 0.8 (Standard: 0.5)
- **smartSizeTolerance**: 0.1 - 0.5 (Standard: 0.3)
- **aoiWidthMultiplier**: 1.0 - 1.5 (Standard: 1.2)
- **aoiMinEdgeDensity**: 0.01 - 0.1 (Standard: 0.05)
- **aoiMaxEdgeDensity**: 0.3 - 0.7 (Standard: 0.5)

## Debug Ausgabe

Mit `-d` Option werden die angewandten Parameter in den Debug-Dateien dokumentiert:
- `ImageProcessor_digit_X_crop=Xpct_width_Xpct_height`
- `ImageProcessor_digit_X_crop=Xpct_width_Xpct_height_AOI` (für AOI-Ziffer)

### Log-Ausgaben für Smart Fragment & AOI
- Spacing/Size regularity analysis results  
- AOI prediction calculations with edge density
- Parameter values used for validationierbare Parameter aus der `config.yml`:

## Verfügbare Parameter

### Standard Digit Cropping
- **cropPercentHorizontal**: Horizontaler Crop-Prozentsatz für normale Ziffern (Standard: 0.1 = 10%)
- **cropPercentVertical**: Vertikaler Crop-Prozentsatz für normale Ziffern (Standard: 0.02 = 2%)

### AOI (Area-of-Interest) Digit Cropping  
- **cropPercentHorizontalAOI**: Horizontaler Crop-Prozentsatz für AOI-Ziffer (Standard: 0.15 = 15%)
- **cropPercentVerticalAOI**: Vertikaler Crop-Prozentsatz für AOI-Ziffer (Standard: 0.02 = 2%)

## Funktionsweise

### Standard Ziffern (Ziffern 1-6)
Werden mit `cropPercentHorizontal` und `cropPercentVertical` beschnitten.

### AOI Ziffer (7. Ziffer bei -A Option)  
Wird mit `cropPercentHorizontalAOI` und `cropPercentVerticalAOI` beschnitten.
Der höhere horizontale Wert (15% statt 10%) hilft bei der besseren Erkennung der Dezimalstelle.

## Konfiguration ändern

### Über config.yml
```yaml
cropPercentHorizontal: 0.08      # 8% horizontal crop für normale Ziffern
cropPercentVertical: 0.03        # 3% vertical crop für normale Ziffern  
cropPercentHorizontalAOI: 0.12   # 12% horizontal crop für AOI-Ziffer
cropPercentVerticalAOI: 0.025    # 2.5% vertical crop für AOI-Ziffer
```

### Programmatisch (falls gewünscht)
```cpp
Config config;
config.setCropPercentHorizontal(0.08);
config.setCropPercentVertical(0.03);
config.setCropPercentHorizontalAOI(0.12);
config.setCropPercentVerticalAOI(0.025);
config.saveConfig();
```

## Empfohlene Werte

- **Standard Horizontal**: 0.05 - 0.15 (5% - 15%)
- **Standard Vertikal**: 0.01 - 0.05 (1% - 5%)  
- **AOI Horizontal**: 0.10 - 0.20 (10% - 20%)
- **AOI Vertikal**: 0.01 - 0.05 (1% - 5%)

## Erweiterte 3-Löcher-Analyse (NEU)

Die neue 3-Löcher-Analyse verbessert die Erkennung komplexer Ziffern, insbesondere der Ziffer 8:

### Funktionsweise
1. **Sortierung**: Alle Konturen werden nach Größe sortiert (absteigend)
2. **3-Stufen-Klassifikation**:
   - **3-Löcher-Struktur**: 2. und 3. Kontur ≥ 20% der größten → Ziffer 8 erkannt
   - **2-Löcher-Struktur**: Nur 2. Kontur ≥ 20% → Ziffern 0, 6, 9 
   - **1-Löcher-Struktur**: Fallback auf Enclosed Area Detection

### Konfiguration
```yaml
morphSizeRatioThreshold: 0.2    # 20% Schwellwert für 2. und 3. Kontur
```

### Vorteile
- **Bessere Ziffer-8-Erkennung**: Drei separate Bereiche werden korrekt identifiziert
- **Robuste Filterung**: Fragmente werden zuverlässiger entfernt
- **Segment-Display-Kompatibilität**: Funktioniert mit digitalen Stromzählern

## Morphologische Filter-Parameter

Die Option `-C` verwendet auch konfigurierbare morphologische Operationen zur Fragment-Filterung:

### Verfügbare Parameter

- **morphKernelSizeDivisor**: Teiler für Kernel-Größe (Standard: 15)
  - Kernel-Größe = min(Bildbreite, Bildhöhe) / morphKernelSizeDivisor
  - Kleinere Werte = größere Kernel = stärkere Verbindung von Kanten
  
- **morphIterations**: Anzahl Dilatation/Erosion Iterationen (Standard: 1) 
  - Höhere Werte = stärkere morphologische Effekte
  
- **morphSizeRatioThreshold**: Schwellwert für 3-Löcher-Analyse (Standard: 0.2)
  - Verhältnis zweit-/drittgrößte Kontur zur größten für komplexe Ziffern (0, 6, 8, 9)
  - **NEU**: Analysiert nun drei Konturen statt zwei für bessere Ziffer-8-Erkennung

### Funktionsweise (Erweiterte 3-Löcher-Analyse)

1. **Dilatation**: Verdickt Kanten um nahe Fragmente zu verbinden
2. **Konturenanalyse**: Findet alle verbundenen Komponenten nach Größe sortiert
3. **3-Stufen-Klassifikation**: 
   - **3 große Konturen** (≥20%): Ziffer 8 oder komplexe Struktur → Alle behalten
   - **2 große Konturen** (≥20%): Ziffer 0, 6, 9 → Zwei größte behalten  
   - **1 große Kontur**: Einfache Ziffer oder Fragmente → Intelligente Filterung
4. **Erosion**: Stellt ursprüngliche Kantenstärke wieder her

### Konfiguration in config.yml
```yaml
morphKernelSizeDivisor: 12        # Größere Kernel (weniger konservativ)
morphIterations: 2                # Stärkere morphologische Effekte  
morphSizeRatioThreshold: 0.2      # Optimiert für 3-Löcher-Analyse bei Ziffer 8
```

### Empfohlene Werte

- **morphKernelSizeDivisor**: 10-20 (Standard: 15)
- **morphIterations**: 1-3 (Standard: 1)
- **morphSizeRatioThreshold**: 0.1-0.4 (Standard: 0.2) - **NEU**: Optimiert für 3-Löcher-Analyse

## Kontur-Filter Parameter (-C Option)

Die Kontur-Filterung verwendet zusätzliche Parameter zur Qualitätssicherung:

### Verfügbare Parameter

#### digitMinWidth
- **Standard**: `5` (Pixel)
- **Beschreibung**: Minimale Breite für gültige Ziffern-Konturen
- **Zweck**: Filtert sehr schmale Konturen aus (Rauschen, Artefakte)
- **Empfohlener Bereich**: 3-10 Pixel
- **Skalierung**: Wird automatisch mit Perspektivenkorrektur skaliert

```yaml
digitMinWidth: 5              # Schmalere Konturen werden verworfen
```

#### perspectiveHeightTolerance
- **Standard**: `1.3` (Multiplikator)
- **Beschreibung**: Höhen-Toleranz für Ziffern-Konturen relativ zur Breite
- **Zweck**: Verhindert zu hohe/schmale Konturen (z.B. Striche, Linien)
- **Empfohlener Bereich**: 1.2-1.5
- **Berechnung**: `maxHöhe = Breite × perspectiveHeightTolerance`

```yaml
perspectiveHeightTolerance: 1.3    # Ziffern dürfen max. 1.3x breiter als hoch sein
```

### Funktionsweise

Die Kontur-Filterung erfolgt in `filterContours()` und prüft:

1. **Mindestbreite**: `contour.width > digitMinWidth * perspectiveScale`
2. **Höhen-Verhältnis**: `contour.height < contour.width * perspectiveHeightTolerance`
3. **Standard-Größen**: Zusätzlich digitMinHeight/MaxHeight Prüfungen

### Anwendungsbeispiele

**Konservative Filterung** (mehr Konturen akzeptieren):
```yaml
digitMinWidth: 3
perspectiveHeightTolerance: 1.5
```

**Strenge Filterung** (nur eindeutige Ziffern):
```yaml
digitMinWidth: 7
perspectiveHeightTolerance: 1.2
```

**Standard-Empfehlung** (ausgewogene Filterung):
```yaml
digitMinWidth: 5
perspectiveHeightTolerance: 1.3
```

### Integration mit Perspektivenkorrektur

- **Automatische Skalierung**: `digitMinWidth` wird mit `perspectiveScaleX` multipliziert
- **Adaptive Toleranz**: Berücksichtigt Bildverzerrung durch Kamerawinkel
- **Robuste Erkennung**: Funktioniert auch bei schrägen Aufnahmen

### Troubleshooting

| Problem | Lösung |
|---------|--------|
| Zu viele falsche Erkennungen | `digitMinWidth` erhöhen, `perspectiveHeightTolerance` senken |
| Ziffern werden nicht erkannt | `digitMinWidth` senken, `perspectiveHeightTolerance` erhöhen |
| Vertikale Linien stören | `perspectiveHeightTolerance` auf 1.2 senken |
| Schmale Ziffern (1, I) fehlen | `digitMinWidth` auf 3 senken |

## Enclosed Area Detection Parameter (-C Option)

Die Enclosed Area Detection erweitert die Smart Fragment Filtering um intelligente Loch-Erkennung:

### Verfügbare Parameter

#### enclosedAreaDetection
- **Standard**: `true` (aktiviert)
- **Beschreibung**: Aktiviert/deaktiviert die erweiterte Enclosed Area Detection
- **Zweck**: Erkennt Ziffern mit charakteristischen Löchern (0, 4, 6, 8, 9)
- **Performance**: Minimal zusätzlicher Rechenaufwand bei deutlich verbesserter Genauigkeit

```yaml
enclosedAreaDetection: true       # Aktiviert erweiterte Loch-Erkennung
```

#### minHoleAreaRatio
- **Standard**: `0.02` (2%)
- **Beschreibung**: Minimale Lochgröße als Anteil der Gesamt-Konturfläche
- **Zweck**: Filtert zu kleine Löcher aus (Rauschen vs. echte Ziffern-Löcher)
- **Empfohlener Bereich**: 0.01-0.05 (1%-5%)

```yaml
minHoleAreaRatio: 0.02           # Löcher müssen mindestens 2% der Konturfläche haben
```

### Funktionsweise

Die Enclosed Area Detection analysiert jede Ziffer-Kontur auf eingeschlossene Bereiche:

1. **Loch-Erkennung**: Findet alle Löcher innerhalb einer Ziffer
2. **Größen-Filterung**: Verwirft zu kleine Löcher (Rauschen) 
3. **Intelligente Klassifikation**: Bevorzugt Konturen mit signifikanten Löchern
4. **Fallback-Strategie**: Behält große Konturen ohne Löcher als Backup

### Anwendungsbeispiele

**Maximale Genauigkeit** (empfohlen):
```yaml
enclosedAreaDetection: true
minHoleAreaRatio: 0.02
```

**Konservative Einstellung** (weniger sensitiv):
```yaml
enclosedAreaDetection: true
minHoleAreaRatio: 0.04           # Nur größere, eindeutige Löcher
```

**Deaktiviert** (Fallback auf einfache Größen-Filterung):
```yaml
enclosedAreaDetection: false     # Verwendet nur klassische Smart Fragment Filtering
```

### Ziffern-Typen und Loch-Erkennung

| Ziffern-Typ | Löcher | Erkennung |
|-------------|--------|-----------|
| **0, 6, 8, 9** | 1+ Löcher | ✅ Excellent |
| **4** | Dreieckiges Loch | ✅ Sehr gut |
| **A, B, D, O, P, Q, R** | 1+ Löcher | ✅ Excellent |
| **1, 2, 3, 5, 7** | Keine Löcher | ⚠️ Fallback-Logik |
| **Fragmente** | Keine Struktur | ❌ Gefiltert |

### Integration mit Smart Fragment Filtering

- **Aktiviert**: Enclosed Area Detection + Smart Fragment Filtering
- **Deaktiviert**: Nur Smart Fragment Filtering (Rückwärtskompatibilität)
- **Kombinierte Intelligenz**: Beste Ergebnisse durch beide Algorithmen

### Troubleshooting

| Problem | Lösung |
|---------|--------|
| Ziffern mit Löchern werden nicht erkannt | `minHoleAreaRatio` auf 0.01 senken |
| Zu viele Fragmente bleiben erhalten | `minHoleAreaRatio` auf 0.03 erhöhen |
| Performance-Probleme | `enclosedAreaDetection` deaktivieren |
| Inkonsistente Ergebnisse | `minHoleAreaRatio` auf 0.02 (Standard) |

## Debug Ausgabe

Mit `-d` Option werden die angewandten Parameter in den Debug-Dateien dokumentiert:
- `ImageProcessor_digit_X_crop=Xpct_width_Xpct_height`
- `ImageProcessor_digit_X_crop=Xpct_width_Xpct_height_AOI` (für AOI-Ziffer)
- `ImageProcessor_digit_X_filter=fragments_removed_smart` (nach morphologischer Filterung)