## Die Feature Option -C. 

# Feature-Request
Im Gegensatz zu dem Zähler im Originalbeispiel hat mein Zähler ein silbernens Frontblech, welches einen harten Übergang der Zahlenfelder abbildet. Das Resultat ist, dass die Ziffernfelderkennung je einen Rahmen um das die eigentliche Ziffer macht. Dieser Umstand ist problematisch für das Konzept mit Einlernen und Wiedererkennen, zumal der Rahmen mit vielen Störfragmenten daherkommt. Die Idee soll sein diesen Rahmen wegzufiltern, sodass die Zahl alleine übrig bleibt.

# Beschreibung
Ich werde die -C Option einführen, welche die Fragmente im Randbereich entfernen. Das ist ein eleganter Ansatz zur Bereinigung der Digit-Bilder.

## ✅ Feature Implementation Complete: `-C` Option for Digit Cropping
### **What was implemented:**
1. **Command-line Option**: Added `-C` flag to enable/disable digit cropping
2. **Configuration System**: Extended Config.h and Config.cpp with `cropDigits` boolean flag
3. **Main Application**: Updated main.cpp with option parsing and help text
4. **Image Processing**: Modified ImageProcessor.cpp to conditionally apply cropping
5. **Debug Output**: Enhanced debug images to show crop status in filenames
   
### **Technical Details:**
- **Asymmetric Cropping**: 10% width, 3% height reduction to remove frame edges
- **Conditional Processing**: Only crops when `-C` option is specified
- **Configuration Persistence**: Settings saved to config.yml file
- **Debug Integration**: Shows `crop=10pct_width_3pct_height` or `crop=none` in debug filenames
- **Boundary Safety**: Includes proper boundary checking to prevent invalid rectangles
### **Usage Examples:**
```bash
# Enable digit cropping with Area-of-Interest and debug output
./emeocv -c 0 -A -C -d -t
# Show help text with new -C option
./emeocv -h
# Use cropping with image directory input
./emeocv -i /path/to/images -A -C -t
```

### **Compatibility:**
- ✅ Works with existing `-A` (Area-of-Interest) option  
- ✅ Compatible with `-d` (debug mode) for visualization
- ✅ Integrates with configuration system
- ✅ Maintains backward compatibility (off by default)
