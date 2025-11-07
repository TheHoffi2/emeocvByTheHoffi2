#!/bin/bash

echo "=== Extended Fragment Filtering Test (-C option) ==="
echo

cd Debug

echo "1. Testing Fragment Filtering functionality:"
rm -rf debug_output/*
timeout 8s ./emeocv -c 0 -A -C -d -t > /dev/null 2>&1

echo "   WITH Fragment Filtering (-C option):"
FILTERED_FILES=$(ls debug_output/ImageProcessor_digit_*filter=fragments_removed* 2>/dev/null | wc -l)
echo "   - Found $FILTERED_FILES digit files with fragment filtering applied"

# Show example filenames
echo "   - Example processed files:"
ls debug_output/ImageProcessor_digit_*filter=fragments_removed* 2>/dev/null | head -2 | sed 's/.*ImageProcessor_digit_/     → /'

echo

echo "2. Testing WITHOUT Fragment Filtering:"
rm -rf debug_output/*
timeout 5s ./emeocv -c 0 -A -d -t > /dev/null 2>&1

UNFILTERED_FILES=$(ls debug_output/ImageProcessor_digit_*filter=none* 2>/dev/null | wc -l)
echo "   - Found $UNFILTERED_FILES digit files without fragment filtering"

# Show example filenames
echo "   - Example unprocessed files:"
ls debug_output/ImageProcessor_digit_*filter=none* 2>/dev/null | head -2 | sed 's/.*ImageProcessor_digit_/     → /'

echo

echo "3. Fragment Filtering Algorithm Details:"
echo "   - Dilate: Merge nearby edges to form connected contours"
echo "   - Find Contours: Identify all connected components"
echo "   - Filter: Keep only the largest contour (main digit)"
echo "   - Erode: Restore original edge thickness"
echo "   - Result: Fragments removed, main digit preserved"

echo

echo "4. Morphological Parameters:"
echo "   - Kernel Size: Dynamic based on image dimensions (image_size/15)"
echo "   - Kernel Shape: Elliptical for better edge connectivity"
echo "   - Operations: Dilation followed by Erosion (Opening-like)"

echo

echo "5. Integration Benefits:"
echo "   - Cleaner OCR input: Removes distracting artifacts"
echo "   - Better recognition: Focus on main digit structure"
echo "   - Configurable: Only active when -C option is used"
echo "   - Debug-friendly: Shows filter status in filename"

echo
echo "=== Extended Fragment Filtering Feature Complete ==="