const int NUM_SLIDERS = 1;
const int ANALOG_INPUTS[NUM_SLIDERS] = {A0};

// tuples of {is, should} values. has to be sorted in ascending order by first number in tuple.
const int MAP[][2] = {
  {0, 0}, {16, 85}, {42, 171}, {122, 256}, {264, 341}, {407, 426}, {544, 512}, {678, 597}, {813, 682}, {952, 767}, {998, 853}, {1020, 938}, {1023, 1023}
};
const int MAP_SIZE = 13; //sizeof(MAP) / sizeof(int);

int analogSliderValues[NUM_SLIDERS];

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(ANALOG_INPUTS[i], INPUT);
  }

  Serial.begin(9600);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderDebug();
  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    int inputValue = analogRead(ANALOG_INPUTS[i]);  
    analogSliderValues[i] = mapValue(inputValue);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

int mapValue(int val)
{
  // make sure the value is within range, otherwise return limits
  if (val <= MAP[0][0]) return MAP[0][1];
  if (val >= MAP[MAP_SIZE-1][0]) return MAP[MAP_SIZE-1][1];

  // search until 
  uint8_t pos = 1; // MAP[0][0] already tested
  while(val > MAP[pos][0]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == MAP[pos][0]) return MAP[pos][1];

  // interpolate in the right segment for the rest
  return (val - MAP[pos-1][0]) * (MAP[pos][1] - MAP[pos-1][1]) / (MAP[pos][0] - MAP[pos-1][0]) + MAP[pos-1][1];
}

#pragma region debug
void printSliderDebug() {
  String builtString = String("[DEBUG] ");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]) + "~" + String(((float)analogSliderValues[i] / (float)1023) * 100) + "%";

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}
#pragma endregion debug
