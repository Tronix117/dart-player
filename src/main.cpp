#include <Arduino.h>
#include <pitches.h>

const uint8_t ANALOG_DART_PIN_SIZE = 3;
uint8_t analogDartPins[ANALOG_DART_PIN_SIZE] = { A0, A1, A2 };
const uint8_t SOUND_PIN = A5;

const uint8_t DART_MASTER_LINE_SIZE = 10;
const uint8_t DART_SLAVE_LINE_SIZE = 7;
const int INPUT_MATRIX[ANALOG_DART_PIN_SIZE][3][DART_MASTER_LINE_SIZE] = {{
    { 118, 116, 114, 112, 110, 108, 106, 104, 97, 91 },
    { 70, 64, 59, 54, 50, 47, 44, 42, 33, 27 },
    { 23, 20, 17, 15, 13, 12, 11, 10, 7, 6 }
  }, {
    { 117, 115, 113, 111, 109, 107, 106, 104, 97, 91 },
    { 64, 58, 52, 48, 44, 41, 39, 36, 28, 23 }
  }, {
    { 117, 115, 113, 111, 109, 107, 106, 104, 97, 91 },
    { 64, 58, 52, 48, 44, 41, 39, 36, 28, 23 }
  }
};

const uint8_t INPUT_MATRIX_GROUP_SIZES[ANALOG_DART_PIN_SIZE] = { 3, 2, 2 };

const uint8_t INPUT_MATRIX_COEFF = 8; // meaning 1024 (HIGH VOLTAGE) input will become 128

// Ignore unrelevant values, < to this
const uint8_t INPUT_MATRIX_THREASHOLD = 2 * INPUT_MATRIX_COEFF;


void play(uint16_t melody[], uint8_t noteDurations[], int length) {

  for (int i = 0; i < length; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(SOUND_PIN, melody[i], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(SOUND_PIN);
  }
}

void hit(int val, int multiplicator) {
  // uint16_t melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4 };
  // uint8_t durations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };
  // uint16_t melody[] = {
  //   NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  //   NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  //   NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
  // };
  // uint8_t durations[] = {
  //   2, 2, 4, 8, 2, 2, 4, 8,
  //   2, 2, 4, 8, 2, 2, 4, 8,
  //   2, 2, 4, 8, 2, 2, 4, 8
  // };
  // play( melody, durations, 24);
  // uint16_t melody[3] = { };
  // uint8_t durations[20] = { };

  // //  PORTD = val;

  // for (uint8_t i = 0; i < multiplicator; i++) {
  //   melody[i] = NOTE_C4;
  //   durations[i] = 4;
  // }

  // melody[multiplicator] = 0;
  // durations[multiplicator] = 2;

  // for (uint8_t i = 0; i < val; i++) {
  //   melody[multiplicator + i] = NOTE_C4;
  //   durations[multiplicator + i] = 4;
  // }

  uint16_t melody[] = {
    NOTE_G4, NOTE_C4, NOTE_E4,
  };
  uint8_t durations[] = {
    2, 2, 4,
  };
  play( melody, durations, 3);

  // play( melody, durations, multiplicator + val + 1);
}

void handleLines(uint8_t masterLine, uint8_t slaveLine) {
  uint8_t val = 0;
  uint8_t multiplicator = 0;

  // convert masterLine + slaveLine to val + multiplicator

  PORTD = slaveLine << 4 | masterLine;

  if (val) {
    hit(val, multiplicator);
  }
}

void handleAnalogMatrix(int inputs[ANALOG_DART_PIN_SIZE]) {
  uint8_t masterLine = 0;
  uint8_t slaveLine = 0;
    
  int cumSize = 0;
  for(uint8_t i=0; i < ANALOG_DART_PIN_SIZE; i++) {
    const int handledLineSize = INPUT_MATRIX_GROUP_SIZES[i];
    cumSize += handledLineSize;
    
    // Ignore unrelevant inputs
    if (inputs[i] <= INPUT_MATRIX_THREASHOLD) continue;

    int input = inputs[i];
    unsigned int curDiff = -1;
    bool reached = false;
    for(uint8_t j=0; j < handledLineSize; j++) {
      for(uint8_t k = 0; k < DART_MASTER_LINE_SIZE; k++) {
        unsigned int diff = abs(INPUT_MATRIX[i][j][k] * INPUT_MATRIX_COEFF - input);
        if (curDiff < 0) {
          curDiff = diff;
        }

        if (diff > curDiff) {
          // we can only go closer, if we start having higher diff, then previous value is correct
          reached = true;
          break;
        }

        masterLine = k + 1;
        slaveLine = cumSize + j - handledLineSize + 1;
        curDiff = diff;
      }
      if (reached) break;
    }

    // if it hasn't been continued before, then it's the relevant input
    break;
  }

  handleLines(masterLine, slaveLine);
}

void trigger() {
  int inputs[ANALOG_DART_PIN_SIZE];
  
  for(uint8_t i=0; i < ANALOG_DART_PIN_SIZE; i++) {
    inputs[i] = analogRead(analogDartPins[i]);
  }

  // PORTD = inputs[0] / 8;

  handleAnalogMatrix(inputs);
}

void setup() {
  // Set PORT READ/WRITE
  DDRD = 0xff; // Port D to write

  // Stop Loop, and jump to trigger method when value on analog pin change
  for(uint8_t i=0; i < ANALOG_DART_PIN_SIZE; i++) {
    pinMode(analogDartPins[i], INPUT);
    attachInterrupt(analogDartPins[i], trigger, CHANGE);
  }
}

void loop() {
  trigger();
  delay(1000);

  // Just to check if program is working
  // PORTD ^= 0b10000000;
  // delay(10);
}