

const int piezzoPin = A0;
const int piezzoSoundPin = 11;
const int buttonPin = 2;


const int treshhold = 100;
int piezzoOn = 0;
unsigned long readTime = 0;
int treshholdTime = 5000;


void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(piezzoPin, INPUT);
  pinMode(piezzoSoundPin, OUTPUT);
  Serial.begin(9600);
}



void loop() {
  // put your main code here, to run repeatedly:
  int valuePiezzo = analogRead(piezzoPin);

  if (valuePiezzo > treshhold && piezzoOn == 0){
    readTime = millis();
    Serial.println("Start");
     piezzoOn = 1;
    
  }

  if (millis() - readTime >= treshholdTime && readTime != 0 && piezzoOn == 1){
      digitalWrite(piezzoSoundPin, HIGH);
      int buttonValue = !digitalRead(buttonPin);

      if (buttonValue == 1){
         piezzoOn = 0;
         digitalWrite(piezzoSoundPin, LOW);
      }
  }
}
