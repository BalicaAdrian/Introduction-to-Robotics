const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

const int potRedPin = A0;
const int potGreenPin = A1;
const int potBluePin = A2;

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

}

void loop() {

  int analogValueRed = analogRead(potRedPin);
  int analogValueGreen = analogRead(potGreenPin);
  int analogValueBlue = analogRead(potBluePin);

   int valPotRed = map(analogValueRed, 0, 1023, 0, 255);
   int valPotGreen = map(analogValueGreen, 0, 1023, 0, 255);
   int valPotBlue = map(analogValueBlue, 0, 1023, 0, 255);

   analogWrite(redPin, valPotRed); 
   analogWrite(greenPin, valPotGreen);
   analogWrite(bluePin, valPotBlue);
    
}
