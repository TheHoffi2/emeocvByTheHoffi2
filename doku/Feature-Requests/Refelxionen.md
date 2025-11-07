Lichtreflexionen von Glas und Blechkanten - das ist ein klassisches Problem bei der optischen Zählererkennung! 
Das sind Glanzlichter und Spiegelungen die als falsche Konturen erkannt werden.

#### Problem-Analyse: Lichtreflexionen
Typische Reflexions-Artefakte bei Zähler mit Glasscheibe:
 
### Lösungsansätze für Lichtreflexions-Filterung:
## 1. Intensitäts-basierte Filterung (Empfohlen)
Idee: Lichtreflexionen sind oft überbelichtet (sehr helle Bereiche) im Originalbild.
# Neue Parameter für config.yml
reflectionFiltering: true
maxReflectionIntensity: 240 # Pixel über diesem Wert = wahrscheinlich Reflexion
reflectionAreaThreshold: 0.8 # 80% der Kontur muss überbelichtet sein
intensityVarianceThreshold: 30 # Reflexionen haben niedrige Varianz

## 2. Histogram-Analyse
Idee: Echte Ziffern haben charakteristische Histogramm-Verteilungen, Reflexionen sind peak-lastig.
# Neue Parameter für config.yml
histogramFiltering: true
reflectionPeakThreshold: 0.7 # 70% der Pixel in einem Intensitäts-Peak
histogramEntropyThreshold: 3.0 # Mindest-Entropie für echte Ziffern
 
## 3. Geometrie-basierte Filterung
Idee: Lichtreflexionen haben oft uncharakteristische Formen für Ziffern.
# Neue Parameter für config.yml
geometryFiltering: true
minAspectRatio: 0.3 # Ziffern haben typisches Breite/Höhe-Verhältnis
maxAspectRatio: 0.8
minSolidity: 0.6 # Ziffern sind relativ "solid" (gefüllt)
maxEccentricity: 0.9 # Reflexionen oft sehr elliptisch
 
## 4. Multi-Scale-Analyse
Idee: Echte Ziffern sind skalierungs-invariant, Reflexionen oft noise-artig bei verschiedenen Scales.
# Neue Parameter für config.yml
multiScaleFiltering: true
scaleFactors: [0.8, 1.0, 1.2] # Teste verschiedene Skalierungen
scaleConsistencyThreshold: 0.7 # Konturen müssen bei 70% der Scales erkennbar sein

## 5. Erweiterte Canny-Parameter
Idee: Adaptive Canny-Schwellwerte um Reflexionen zu unterdrücken.
 
adaptiveCannyFiltering: true
reflectionSuppressionMode: true
cannyHighThresholdMultiplier: 1.3 # Höhere Threshold für überbelichtete Bereiche
cannyLowThresholdMultiplier: 0.7 # Niedrigere für normale Bereiche

Empfohlene Implementierung:
Phase 1: Intensitäts-Filter (Sofort umsetzbar)
Erweitere deine config.yml:

# Reflexions-Filterung hinzufügen
reflectionFiltering: true
maxReflectionIntensity: 240
reflectionAreaThreshold: 0.8
intensityVarianceThreshold: 30

## Zusätzliche Hardware-Ansätze:
Beleuchtungsoptimierung:
Diffuse Beleuchtung statt Punktlichtquellen
Polarisationsfilter vor der Kamera
Indirekte Beleuchtung (45° Winkel)
Kamera-Einstellungen:
Belichtung reduzieren um Überbelichtung zu vermeiden
Kontrast erhöhen für bessere Ziffer-Hintergrund-Trennung