### Idee
Um den Konturen der noch zu verschwindenen oder der bereits zu kommenden Zahlen wegzufiltern soll folgener Alogrithmus eingeführt werden. Es soll eine vertikale Pixelreihe iterativ on oben nach unten geprüft werden. Die Pixelreihe rechts und links soll ein margin haben um Fragmente am Rand nicht zu erfassen. die Iteration soll bis ein drittel von der Höhe gemacht werden. Sobald drei Reihen kein weißes Pixel erfassen, ist das mit einer Trennlinie zwichen Konturen zu werten und alles was oberhalb ist soll weggeschnitten werden. Gleiches Prozedere soll von unten gemacht werden. So soll sicher gestellt werden, dass Konturen von kommenden und gehenden Zahlen weggefiltert werden
 
## Ziel
Dieser Algorithmus löst ein sehr spezifisches Problem von Analogzählern lösen - die partiellen Ziffern am oberen und unteren Rand, die durch die mechanische Rotation entstehen.

## Vorteile dieser Idee
# 1. Präzise Fragment-Erkennung:
Vertikale Analyse erkennt horizontale Trennungen perfekt
Margin-System verhindert falsche Positive an den Seitenrändern
Triple-Check (3 aufeinanderfolgende leere Reihen) für Robustheit

# 2. Intelligente Begrenzung:
1/3 Höhen-Begrenzung verhindert Überfilterung der Hauptziffer
Bidirektional (oben + unten) für vollständige Abdeckung
Adaptive Erkennung funktioniert bei verschiedenen Zifferngrößen

# Implementierungsvorschlag:
Neue Parameter für config.yml:
# Vertical Fragment Filtering
verticalFragmentFiltering: true # Enable/Disable Feature
verticalScanMargin: 0.15 # 15% margin from left/right edges
verticalScanDepth: 0.33 # Scan up to 1/3 of digit height
verticalEmptyRowsThreshold: 3 # 3 consecutive empty rows = separator

# Standard-Konfiguration:
verticalFragmentFiltering: true
verticalScanMargin: 0.15 # 15% Seitenrand
verticalScanDepth: 0.33 # 1/3 der Höhe scannen
verticalEmptyRowsThreshold: 3 # 3 leere Reihen = Trennlinie
 
# Aggressive Filterung (für stark fragmentierte Zähler):
verticalScanMargin: 0.10 # Weniger Seitenrand
verticalScanDepth: 0.40 # Tiefer scannen
verticalEmptyRowsThreshold: 2 # Sensitivere Erkennung
 
# Konservative Filterung (für saubere Zähler):
verticalScanMargin: 0.20 # Mehr Seitenrand
verticalScanDepth: 0.25 # Weniger tief scannen
verticalEmptyRowsThreshold: 4 # Robustere Erkennung

## Erwartete Verbesserungen:
# OCR-Qualität:
Eliminiert Fragmentrauschen von rotierenden Ziffern
Klarere Ziffernkonturen für bessere Erkennung
Weniger falsche Positive durch saubere Trennung
# Robustheit:
Funktioniert bei verschiedenen Zählertypen (mechanisch rotierend)
Adaptive Parameter für verschiedene Bildqualitäten
Optional aktivierbar für Kompatibilität
# Performance:
Effizienter Algorithmus (nur vertikale Scans)
Begrenzte Suchbereiche (1/3 Höhe)
Parallele Verarbeitung pro Ziffer möglich
# Implementierungs-Roadmap:
Parameter zu Config.h/cpp hinzufügen 
filterVerticalFragments() Funktion implementieren
Integration in process() Pipeline
Debug-Ausgabe für Visualisierung
Dokumentation in CROP_PARAMETERS.md ergänzen
Tests mit verschiedenen Zählertypen
Soll ich die Implementierung beginnen