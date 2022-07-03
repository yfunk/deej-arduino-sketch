/**
 * Enable debug mode.
 *
 * Prints additional information useful for debugging and calibrating sliders to serial.
 */
// #define DEBUG

/**
 * Enable resistance curve calibration.
 *
 * Many cheap sliding potentiometers (e.g. B103) don't have a particularly linear resistance curve,
 * despite being advertised as such. This can cause the reported slider value to deviate from its
 * actual position (e.g. physical slider is at 50%, reported value is 30% volume).
 */
#define ENABLE_CALIBRATION

/**
 * Analog input definition.
 */
const int ANALOG_INPUTS[] = {A0, A1, A2, A3, A4};

#ifdef ENABLE_CALIBRATION
/**
 * Resistance curve calibration data.
 *
 * Calibration data has to be provided in tuples of {[measured resistance], [expected resistance]}.
 * The list of calibration data points has to be sorted in ascending order by the first number of
 * the tuple.
 *
 * The default values match cheap 60 mm B130 linear sliding potentiometers.
 */
const int CALIBRATION_DATA[][2] = {{13, 0},    {24, 43},   {35, 85},   {46, 128},   {82, 171},
                                   {137, 213}, {193, 256}, {249, 298}, {306, 341},  {364, 384},
                                   {421, 426}, {477, 469}, {530, 512}, {584, 554},  {638, 597},
                                   {690, 639}, {743, 682}, {799, 725}, {855, 767},  {912, 810},
                                   {967, 853}, {982, 895}, {993, 938}, {1004, 980}, {1008, 1023}};

const int NUM_CAL_POINTS = sizeof(CALIBRATION_DATA) / sizeof(CALIBRATION_DATA[0]);
#endif  // ENABLE_CALIBRATION

const int NUM_SLIDERS = sizeof(ANALOG_INPUTS) / sizeof(ANALOG_INPUTS[0]);

int rawSliderValues[NUM_SLIDERS];

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(ANALOG_INPUTS[i], INPUT);
  }

  Serial.begin(9600);

#ifdef DEBUG
  printSetupDebug();
#endif  // DEBUG
}

void loop() {
  updateSliderValues();
  sendSliderValues();

#ifdef DEBUG
  printSliderValuesDebug();
#endif  // DEBUG

  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    int inputValue = analogRead(ANALOG_INPUTS[i]);
    rawSliderValues[i] = inputValue;
  }
}

void sendSliderValues() {
  String serialString = "";

  for (int i = 0; i < NUM_SLIDERS; i++) {
#ifdef ENABLE_CALIBRATION
    serialString += String(calibrateValue(rawSliderValues[i]));
#else
    serialString += String(rawSliderValues[i]);
#endif  // ENABLE_CALIBRATION

    if (i < NUM_SLIDERS - 1) {
      serialString += "|";
    }
  }

  Serial.println(serialString);
}

#ifdef ENABLE_CALIBRATION
int calibrateValue(int val) {
  // make sure the value is within range, otherwise return nearest limit
  if (val <= CALIBRATION_DATA[0][0]) return CALIBRATION_DATA[0][1];
  if (val >= CALIBRATION_DATA[NUM_CAL_POINTS - 1][0])
    return CALIBRATION_DATA[NUM_CAL_POINTS - 1][1];

  // search calibration data until we find the nearest (smaller or equal) measured value
  uint8_t pos = 1;  // first calibration point was already tested
  while (val > CALIBRATION_DATA[pos][0]) pos++;

  // if we hit an exact data point return its expected value
  if (val == CALIBRATION_DATA[pos][0]) return CALIBRATION_DATA[pos][1];

  // otherwise interpolate in the right segment
  return (val - CALIBRATION_DATA[pos - 1][0]) *
             (CALIBRATION_DATA[pos][1] - CALIBRATION_DATA[pos - 1][1]) /
             (CALIBRATION_DATA[pos][0] - CALIBRATION_DATA[pos - 1][0]) +
         CALIBRATION_DATA[pos - 1][1];
}
#endif  // ENABLE_CALIBRATION

#ifdef DEBUG
void printSetupDebug() {
  Serial.println("[DEBUG] Using " + String(NUM_SLIDERS) + " sliders.");
#ifdef ENABLE_CALIBRATION
  Serial.println("[DEBUG] Calibration data contains " + String(NUM_CAL_POINTS) + " data points.");
#endif  // ENABLE_CALIBRATION
}

void printSliderValuesDebug() {
  String serialString = "[DEBUG] ";

  for (int i = 0; i < NUM_SLIDERS; i++) {
    serialString += String(rawSliderValues[i]);

    if (ENABLE_CALIBRATION) {
      serialString += "->" + String(calibrateValue(rawSliderValues[i]));
    }

    serialString += "~" + String(((float)rawSliderValues[i] / (float)1023) * 100) + "%";

    if (i < NUM_SLIDERS - 1) {
      serialString += "|";
    }
  }

  Serial.println(serialString);
}
#endif  // DEBUG
