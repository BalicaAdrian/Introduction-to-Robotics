#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int RSPin = 12;
const int EPin = 11;
const int D4Pin = 5;
const int D5Pin = 4;
const int D6Pin = 3;
const int D7Pin = 2;

const int xAxisPin = A0;
const int yAxisPin = A1;
const int buttonPin = 7;

int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;
int position = 1;
int lastButtonState = 0;
int buttonState = 0;
bool YjoyMoved = false;
bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;

int HighScore = 0;
int ok = 0;
int Score = 1;
int flag = 0;
unsigned long StartGameTime = 0;
int delayTime = 250;
unsigned long inputTime = 0;
bool refresh = false;

bool enter = false;
const int noActiontime = 50;
int lastDebounceTime = 0;
int level = 0;
int CurrentPositionMenu1 = 1;
unsigned long  currentTime = 0;

int adresaEeprom = 0;

int SelectedLevel = 1;
const String Meniuri[] = {" Start", ">Start", " High Score", ">High Score",
                          " Settings", ">Settings", "Lives:", "Level:", "Score:",
                          "HighScore:", "Starting level:"
                         };

LiquidCrystal lcd(RSPin, EPin, D4Pin, D5Pin, D6Pin, D7Pin);

void readXAxis() {
  int xValue = analogRead(xAxisPin);
  if (xValue < minThreshold && joyMoved == false) {
    if (CurrentPositionMenu1 > 1) {
      CurrentPositionMenu1--;
    } else {
      CurrentPositionMenu1 = 3;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (CurrentPositionMenu1 < 3) {
      CurrentPositionMenu1++;
    } else {
      CurrentPositionMenu1 = 1;
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void principalMenu()
{
  readXAxis();
  switch (CurrentPositionMenu1)
  {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[1]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[4]);
      lcd.setCursor(2, 1);
      lcd.print(Meniuri[2]);
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[0]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[4]);
      lcd.setCursor(2, 1);
      lcd.print(Meniuri[3]);
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[0]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[5]);
      lcd.setCursor(2, 1);
      lcd.print(Meniuri[2]);
  }
  if (inputTime == 0 || millis() - inputTime > 300) {
    //Serial.print(buttonPressed());
    if (buttonPressed() == 1) {
      if (CurrentPositionMenu1 == 2) {
        //delay(250);
        inputTime = millis();
        position = 2;
        lcd.clear();
      }
    }

    if (buttonPressed() == 1) {
      if (CurrentPositionMenu1 == 3) {
        //delay(250);
        inputTime = millis();
        position = 3;
        lcd.clear();
      }
    }
    if (buttonPressed() == 1) {
      if (CurrentPositionMenu1 == 1) {
        position = 4;
        lcd.clear();
        StartGameTime = millis();
        Score = 0;
        refresh = false;
      }
    }
  }


}

void changeLevel() {
  int yValue = analogRead(yAxisPin);
  if (yValue < minThreshold && YjoyMoved == false) {
    if (level > 0) {
      level--;
    } else {
      level = 3;
    }
    YjoyMoved = true;
  }

  if (yValue > maxThreshold && YjoyMoved == false) {
    if (level < 3) {
      level++;
    } else {
      level = 0;
    }
    YjoyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    YjoyMoved = false;
  }
}

int buttonPressed() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState)
  {
    ok = 0;
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > noActiontime)
  {

    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == LOW)
      {
        ok = 1;
      }
    }
  }
  lastButtonState = reading;
  if ( ok == 1) {
    return 1;
  }
  else {
    return 0;
  }
}

void settingsMenu() {
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  changeLevel();
  //Serial.println("da");
  //Serial.print(buttonPressed());
  if (buttonPressed() == 1 && millis() - inputTime > 350) {
    //delay(250);
    inputTime = millis();
    lcd.clear();
    position = 1;
  }
}

void HighScoreMenu() {
  lcd.setCursor(2, 0);
  lcd.print("Best");
  lcd.setCursor(5, 1);
  int BestScore = EEPROM.read(adresaEeprom);
  lcd.print(BestScore);
  // Serial.print(buttonPressed());
  if (buttonPressed() == 1 && millis() - inputTime > 350) {
    //delay(250);
    inputTime = millis();
    lcd.clear();
    position = 1;
  }
}


void startMenu() {
  if (millis() - StartGameTime < 11000) {
    lcd.setCursor(0, 0);
    lcd.print("Lives:3");
    lcd.setCursor(8, 0);
    lcd.print("Level:");
    lcd.setCursor(14, 0);
    lcd.print(level);
    //Serial.println(flag);

    if (flag == 0) {
      currentTime = millis();
      flag = 1;
    }

    if (millis() - currentTime >= 5000) {
      if (level < 3) {

        level++;

      }

      Score = Score + 3 * level;

      flag = 0;

    }

    lcd.setCursor(0, 1);
    lcd.print(Score);
  }
  else {
    refreshF();
    lcd.setCursor(0, 0);
    lcd.print("Well done!");
    lcd.setCursor(0, 1);
    lcd.print("Press button");

    if (Score > HighScore) {
      HighScore = Score;
      EEPROM.write(adresaEeprom, HighScore);
    }
    if (buttonPressed() == 1 && millis() - inputTime > 350) {
      inputTime = millis();
      lcd.clear();
      position = 1;
    }
  }
}

void refreshF() {
  if (refresh == false) {
    lcd.clear();
    refresh = true;
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.clear();
}



void loop()
{
  switch (position)
  {

    case 3:
      HighScoreMenu();
      break;
    case 2:
      settingsMenu();
      break;
    case 4:
      startMenu();
      break;
    default:
      principalMenu();
      break;
  }
}
