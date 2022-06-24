/**
 * Enable debug mode.
 * 
 * Prints additional information useful for debugging and calibrating sliders to serial (e.g. raw
 * and calibrated slider values).
 */
// #define DEBUG

/**
 * Analog input definition. 
 */
const int ANALOG_INPUTS[] = {A0};

/**
 * Slider calibration data.
 * 
 * Many cheap sliding potentiometers (e.g. B103) don't have a particularly linear resistance taper,
 * despite being advertised as such. This can cause the reported to slider value to deviate from its
 * actual position (e.g. physical slider is at 50%, reported value is 30% volume).
 * 
 * Calibration data has to be provided in tuples of {[measured resistance], [expected resistance]}.
 * The list of calibration data points has to be sorted in ascending order by the first number of
 * the tuple.
 * 
 * The default values match cheap 60 mm B130 'linear' sliding potentiometers.
 */
const int CALIBRATION_DATA[][2] = {
  {0, 0}, {16, 85}, {42, 171}, {122, 256}, {264, 341}, {407, 426}, {544, 512}, {678, 597}, {813, 682}, {952, 767}, {998, 853}, {1020, 938}, {1023, 1023}
};

const int NUM_SLIDERS = sizeof(ANALOG_INPUTS)/sizeof(ANALOG_INPUTS[0]);
const int NUM_CAL_POINTS = sizeof(CALIBRATION_DATA)/sizeof(CALIBRATION_DATA[0]);

int analogSliderValues[NUM_SLIDERS];

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(ANALOG_INPUTS[i], INPUT);
  }

  Serial.begin(9600);

  #ifdef DEBUG
  printSetupDebug();
  #endif // DEBUG
}

void loop() {
  updateSliderValues();
  sendSliderValues();

  #ifdef DEBUG
  printSliderValuesDebug();
  #endif // DEBUG

  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    int inputValue = analogRead(ANALOG_INPUTS[i]);  
    analogSliderValues[i] = mapValue(inputValue);
  }
}

void sendSliderValues() {
  String serialString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    serialString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      serialString += String("|");
    }
  }
  
  Serial.println(serialString);
}

int mapValue(int val)
{
  // make sure the value is within range, otherwise return limits
  if (val <= CALIBRATION_DATA[0][0]) return CALIBRATION_DATA[0][1];
  if (val >= CALIBRATION_DATA[NUM_CAL_POINTS-1][0]) return CALIBRATION_DATA[NUM_CAL_POINTS-1][1];

  // search until 
  uint8_t pos = 1; // MAP[0][0] already tested
  while(val > CALIBRATION_DATA[pos][0]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == CALIBRATION_DATA[pos][0]) return CALIBRATION_DATA[pos][1];

  // interpolate in the right segment for the rest
  return (val - CALIBRATION_DATA[pos-1][0]) * (CALIBRATION_DATA[pos][1] - CALIBRATION_DATA[pos-1][1]) / (CALIBRATION_DATA[pos][0] - CALIBRATION_DATA[pos-1][0]) + CALIBRATION_DATA[pos-1][1];
}

#ifdef DEBUG
void printSetupDebug(){
  Serial.println("[DEBUG] Using " + NUM_SLIDERS + " sliders.")
  Serial.println("[DEBUG] Calibration data contains " + CALIBRATION_DATA_POINTS + " data points.")
}

void printSliderValuesDebug() {
  String serialString = String("[DEBUG] ");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    serialString += String((int)analogSliderValues[i]) + "~" + String(((float)analogSliderValues[i] / (float)1023) * 100) + "%";

    if (i < NUM_SLIDERS - 1) {
      serialString += String("|");
    }
  }
  
  Serial.println(serialString);
}
#endif // DEBUG
