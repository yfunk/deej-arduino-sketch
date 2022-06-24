# 🎚 deej-arduino-sketch

Advanced Ardiuno sketch for [deej](https://github.com/omriharel/deej), an open-source hardware volume mixer.

This version provides an option to calibrate the sliders resistance curve in order to improve the accuracy of cheap linear sliding potentiometers.

## Usage

This sketch works as a drop-in replacement for the original.

If you aren't using 60 mm B103 sliding potentiometers you might want to make your own resistance measurements for the calibration data. Otherwise the default values should work fine.