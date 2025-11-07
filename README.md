emeocv
======


Read and recognize the counter of an electricity meter with OpenCV.

**Note on Modifications:**
This version contains extensions and improvements by Daniel Hofmann (2023–2025), specifically for a different meter type with a reflective (silver) front plate. The modifications improve recognition under difficult lighting conditions and for meters with highly reflective frames. They are fully backwards compatible and optionally activatable.

**Motivation:**
Unlike the original meter (see https://github.com/skaringa/emeocv.git and https://en.kompf.de/cplus/emeocv.html), the new meter type with reflective front plate creates hard frames and additional fragments around the digit fields. These significantly impair automatic digit recognition. The new features specifically filter out these interfering fragments and improve recognition robustness.

![OpenCV for the electricity meter](http://www.kompf.de/cplus/images/emeocv_m.png)


Prerequisites
=============

* g++, make
* OpenCV 2, 3, 4 (Debian/Ubuntu/Raspbian: `apt-get install libopencv-dev`) - developed with OpenCV 2.3.1 and later ported to OpenCV 3.2.0 and 4.2.0
* RRDtool (`apt-get install rrdtool librrd-dev`)
* log4cpp (`apt-get install liblog4cpp5-dev`)
* imagemagick (optional, only needed to display graphs)

Build
=====

    cd emeocv
    make


Usage
=====

    emeocv [-i <dir>|-c <cam>] [-l|-t|-a|-w|-o <dir>] [-s <delay>] [-v <level>] [-d] [-C] [-A] [-P]

    Image input:
        -i <image directory> : read image files (png) from directory.
        -c <camera number> : read images from camera.

    Operation:
        -a : adjust camera.
        -o <directory> : capture images into directory.
        -l : learn OCR.
        -t : test OCR.
        -w : write OCR data to RR database. This is the normal working mode.

    Options:
        -s <n> : Sleep n milliseconds after processing of each image (default=1000).
        -v <l> : Log level. One of DEBUG, INFO, ERROR (default).
        -d : Enable debug/test mode - saves intermediate processing steps as images.
        -C : Enable digit cropping and fragment filtering (removes disturbing frames/fragments around digits; configurable; recommended for meters with mirror front plate).
        -A : Area-of-Interest detection for 7th digit (decimal place); improves detection for meters with additional digits.
        -P : Perspective correction for images taken at an angle; compensates for perspective distortion and improves digit recognition.


New Features and Improvements (by Daniel Hofmann)
=================================================

### Option -C: Digit Cropping & Fragment Filtering
Removes disturbing frames and fragments around digits, especially for meters with mirror/silver front plates. Uses morphological operations (dilate, contour filter, erode) to keep only the main digit and remove noise. Parameters are configurable in `config.yml`.

**Advantages:**
- Significantly improves digit recognition for meters with reflective frames
- Removes unwanted fragments and frames
- Fully compatible with Area-of-Interest (-A) and debug mode (-d)
- Backwards compatible (off by default)

**Usage Example:**
    ./emeocv -i images/ -C -A -t -d

### Option -A: Area-of-Interest (AOI) for 7th Digit
Detects and processes the decimal digit (7th digit) for meters with additional decimal places. AOI is automatically calculated and validated. Works seamlessly with -C and -P.

### Option -P: Perspective Correction
Automatically corrects perspective distortion for images taken at an angle. Detects meter display corners and transforms the image to a frontal view. Improves recognition for meters with difficult mounting positions or strong reflections.

### Reflection and Fragment Filtering
Special filters and parameters (see `config.yml`) to suppress reflections and noise caused by mirror front plates. Includes histogram and geometry-based filtering, as well as multi-scale analysis for robust digit detection.

### Enhanced Debug Output
Debug images now include information about cropping, filtering, AOI, and perspective correction. Filenames indicate which filters and options were active (e.g., `filter=fragments_removed`, `aoi=on`, `perspective=corrected`).

### Configuration
All new features are configurable via `config.yml` (see also `CROP_PARAMETERS.md`).

---

Debug Mode
==========

The debug mode (`-d` option) is particularly useful for testing and development. When enabled, it saves intermediate images from each processing step to help analyze the image processing workflow.

### Debug Output

All debug images are saved to the `debug_output/` directory with descriptive filenames that include:
- Function name
- Processing parameters
- Step sequence
- Active filters/options (e.g., cropping, fragment filtering, AOI, perspective correction)

### Example Debug Files

When processing an electricity meter image with debug mode enabled:

    ./emeocv -i images/ -C -A -P -t -d

The following debug images will be generated:

    debug_output/ImageProcessor_grayscale.png
    debug_output/ImageProcessor_rotate_degrees=0.png
    debug_output/ImageProcessor_perspective_corrected_method=homography_corners=4.png
    debug_output/ImageProcessor_cannyEdges_threshold1=100_threshold2=200.png
    debug_output/ImageProcessor_digit_digit_index=0_x=45_y=120_width=18_height=25_filter=fragments_removed.png
    debug_output/ImageProcessor_digit_digit_index=1_x=65_y=120_width=20_height=25_aoi=on.png
    debug_output/KNearestOcr_recognize_recognized_digit=3_result=3_distance=1250.5.png
    debug_output/KNearestOcr_recognize_recognized_digit=?_result=7_distance=750000.0_aoi=on.png

### Debug Image Types

- **Grayscale conversion**: Shows the original image converted to grayscale
- **Rotation steps**: Images after initial rotation and skew correction
- **Perspective correction**: Images after perspective transformation
- **Edge detection**: Result of Canny edge detection with threshold parameters
- **Individual digits**: Each detected digit region with position and size information
- **Fragment filtering**: Shows digits after fragment removal (if -C enabled)
- **AOI detection**: Shows area-of-interest for 7th digit (if -A enabled)
- **OCR recognition**: Each digit with recognition result, confidence distance, and acceptance status

This debug functionality helps with:
- **Parameter tuning**: Visualize the effect of different thresholds and settings
- **Algorithm validation**: Verify that each processing step works correctly
- **Troubleshooting**: Identify where the processing chain fails for specific images
- **Training data creation**: Extract individual digit images for OCR training



There is a tutorial that explains use case and function of the program:
[OpenCV practice: OCR for the electricity meter](https://en.kompf.de/cplus/emeocv.html) or
[OpenCV Praxis: OCR für den Stromzähler](https://www.kompf.de/cplus/emeocv.html) (in German language).

For details on the new features and technical background, see also:
- `doku/Changes/04_digit_cropping_option_C.md`
- `doku/Changes/05_perspective_correction_option_P.md`
- `doku/Changes/06_area_of_interest_option_A.md`
- `CROP_PARAMETERS.md`

---

**Modifications and Extensions by Daniel Hofmann, 2023–2025.**
These improvements were specifically developed for meters with reflective front plates and are fully optional and backwards compatible.

License
=======

Copyright 2015,2021 Martin Kompf

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
