
const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;
const int buttonPin = A2; 
const int pinX = A0; 
const int pinY = A1; 

const int segSize = 8;
const int noOfDisplays = 4;
const int noOfDigits = 10;


int dpState = LOW;
int currentDisplay = 0;
int currentNumber = 0;
int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;
int minThreshold = 400;
int maxThreshold = 600;
bool joyXMoved = false;
bool joyYMoved = false;
int lastSwState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDellay = 50;
int buttonState = 0;
int digit = 0;
int outputDigit[noOfDigits] = {0, 0, 0, 0};
int outputDP[noOfDigits] = {1, 1, 1, 1};
bool locked = false;

// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
};

byte digitMatrix[noOfDigits][segSize - 1] = {
  // a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};


void displayNumber(byte digit, int decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  if (decimalPoint == LOW) {
    analogWrite(segments[segSize - 1], 1023);

  }
  else {
    analogWrite(segments[segSize - 1], 0);

  }
 
}

void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}


void setup() {

  displayNumber(digit, HIGH);
  for (int i = 0; i < segSize - 1; i++){
    pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < noOfDisplays; i++){
    pinMode(digits[i], OUTPUT);
  }
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {

  yAxisValue = analogRead(pinY);
  buttonValue = digitalRead(buttonPin);

  for (int i = 0; i < 4; i++){
      showDigit(i);
      displayNumber(outputDigit[i],outputDP[i]);
      delay(9);

  }


  int reading = digitalRead(buttonPin);
  if (reading != lastSwState){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDellay) {

    if (reading !=  buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        dpState = !dpState;
        outputDP[currentDisplay] = !dpState;
      }
    }
  }
  lastSwState = reading;


  if (yAxisValue > maxThreshold && joyYMoved == false && dpState == false) {
   if (currentDisplay > 0) {
      currentDisplay -= 1 ;
    } else {
      currentDisplay = 3;
    }
    joyYMoved = true;
  }

  if (yAxisValue < minThreshold && joyYMoved == false && dpState == false) {
     if (currentDisplay < 3) {
      currentDisplay += 1 ;
    } else {
      currentDisplay = 0;
    }
    
    joyYMoved = true;
  }

  if (yAxisValue >= minThreshold && yAxisValue <= maxThreshold) {
    joyYMoved = false;
  }


  if (dpState == true) {
    xAxisValue = analogRead(pinX);
    if (xAxisValue > maxThreshold && joyXMoved == false) {
      if (outputDigit[currentDisplay] < 9) {
        outputDigit[currentDisplay]++;
      }
      else {
        outputDigit[currentDisplay] = 0;
      }
      joyXMoved = true;
    }

    if (xAxisValue < minThreshold && joyXMoved == false) {
      if (outputDigit[currentDisplay] > 0) {
        outputDigit[currentDisplay]--;
      } else {
        outputDigit[currentDisplay] = 9;
      }
      joyXMoved = true;
    }

    if (xAxisValue >= minThreshold && xAxisValue <= maxThreshold) {
      joyXMoved = false;
    }
  }
}
