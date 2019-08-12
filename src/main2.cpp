const int ANALOG_DART_PIN_SIZE = 4;
const int analogDartPins[ANALOG_DART_PIN_SIZE] = { A0, A1, A2, A3 };

const int DART_MASTER_LINE_SIZE = 10;
const int DART_SLAVE_LINE_SIZE = 7;

int INPUT_LINES[2][10] = {
  { 117, 115, 113, 111, 109, 107, 106, 104, 97, 90 },
  { 63, 57, 52, 48, 44, 41, 38, 36, 28, 23 }
};

const int INPUT_MATRIX_COEFF = 8; // meaning 1024 (HIGH VOLTAGE) input will become 128



// detect hit pins
void detectPins(int inputs[4]) {
	int masterLine = 0;
  	int slaveLine = 0;
  	int slaveLineGroup = 0;
  
  	int coeff = 8;
  	int targetValue = 0;
  
  for(int i=0; i<4; i++) {
  	// Serial.println(analogDartPins[i]);
    int value = analogRead(analogDartPins[i]) / coeff;
    //Serial.println(value);
    if (value > 0) {
    	slaveLineGroup = i+1;
      	targetValue = value;
      	break;
    };
  };
  
  int diff = 0;
  int indexValidLine = 0;
  int indexValidMaster = 0;
  for(int j=0; j<2; j++) {
      for(int k=0; k<DART_MASTER_LINE_SIZE; k++) {
      	const int tableValue = INPUT_LINES[j][k];
        const int tableDiff = abs(tableValue - targetValue);
        if ((j==0) && (k==0)) {
          diff = tableDiff;
        } else if (diff < tableDiff) {
        	break;
        } else {
        	diff = tableDiff;
          	indexValidLine = j;
          	indexValidMaster = k;
        };
      };
  };
  
  if (slaveLineGroup == 1) {
  	slaveLine = 1;
  } else {
  	slaveLine = slaveLineGroup * 2 - 2 + indexValidLine;
  };
  
  masterLine = indexValidMaster + 1;
  // Serial.println(slaveLineGroup);
  const int shot[2] = { masterLine, slaveLine };

  
  // Map score
  int groupAMapping[] = { 4, 18, 1, 20, 8, 11, 14, 9, 12, 5 };
  int groupBMapping[] = { 13, 6, 10, 15, 16, 7, 19, 3, 17, 2 };
  
  int multipliers[7] = { 3, 2, 1, 0, 1, 2, 3}; 
  
  int multiplier = 0;
  int score = 0;
  
  if (slaveLine == 4) {
    score = 25;
    if (masterLine == 1) {
    	multiplier = 2;
    } else multiplier = 1;
  } else {
  	multiplier = multipliers[slaveLine - 1];
    if (slaveLine <4) {
    	score = groupAMapping[masterLine - 1];
    } else {
    	score = groupBMapping[masterLine - 1];
    }
  }
  
  Serial.println(score);
  Serial.println(multiplier);
  

};


void trigger() {
	int inputs[ANALOG_DART_PIN_SIZE];
  
    for(int i=0; i < ANALOG_DART_PIN_SIZE; i++) {
      pinMode(analogDartPins[i], INPUT);
      attachInterrupt(analogDartPins[i], trigger, CHANGE);
    };
  
  	//handleAnalogMatrix(inputs);
	detectPins(inputs);
};


void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

int analogPin = A1;
int val = 0;

void loop()
{
  //digitalWrite(13, HIGH);
  //delay(100); // Wait for 1000 millisecond(s)
  //digitalWrite(13, LOW);
  //delay(100); // Wait for 1000 millisecond(s)
  
  //val = analogRead(analogPin);
  //Serial.println(val / 8);
  //delay(2000);
  trigger();
  delay(5000);
}