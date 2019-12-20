#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"


#define MAX_LEN_MATRIX 7
#define MIN_LEN_MATRIX 0


///game setup--------------------------------------------------------------------------------------------
byte matrix[8][8] = 
{0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,                    
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0
  
};

byte tree[8][8] = 
{0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 1, 1, 0, 0, 0,
 0, 0, 1, 1, 1, 1, 0, 0,
 0, 1, 1, 1, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 1, 1, 1,                    
 1, 1, 1, 1, 1, 1, 1, 1,
 0, 0, 0, 1, 1, 0, 0, 0,
 0, 0, 0, 1, 1, 0, 0, 0
  
};

int sadFace[8][8] = 
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0}
};



const int joyX = A1;
int playerPosition = 4;//beggining position of player
const int TRESHHOLD_JOYSTICK_MAXIM = 600;
const int TRESHHOLD_JOYSTICK_MIN = 400;
long lastRefreshScreen = 0;
long lastStarFall = 0;

//default values for game score, lives and level
int Score = 0;
int lives = 3;
int level = 0;
bool livesHasChanged=false;


//timers for increase speed of falling and generete stars
long lastFalls = 0;
int timeToFalls = 400;
int timeToRegenerate = 2200;

int increaseTimeFalls = 0;//value for set the level

LedControl lc =LedControl(12,11,10,1);//DIN, CLK , LOAD, NO ofDrivers
//---------------------------------------------------------------------------------------------------------

// MENU++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//initialize lcd
const int RSPin = 13;
const int EPin = 6;
const int D4Pin = 5;
const int D5Pin = 4;
const int D6Pin = 3;
const int D7Pin = 2;

//set pins
const int xAxisPin = A1;
const int yAxisPin = A0;
const int buttonPin = 7;

int xAxisValue = 0;
int yAxisValue = 0;
int buttonValue = 0;

int positionSettings = 1;
int position = 1;
int lastButtonState = 0;
int buttonState = 0;
bool YjoyMoved = false;
bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;

int highScore = 0;
int ok = 0;
int flag = 0;
unsigned long StartGameTime = 0;
int delayTime = 250;
unsigned long inputTime = 0;
bool refresh = false;
int setLevel = 0;
bool enter = false;
const int noActiontime = 50;
int lastDebounceTime = 0;
int currentPositionMenu1 = 1;
unsigned long  currentTime = 0;

int adresaEeprom = 0;
int SelectedLevel = 1;

const String Meniuri[] = {" Start", ">Start", " HS", ">HS",
                          " Settings", ">Settings", " Info", ">Info", "Lives:", "Level:", "Score:",
                          "highScore:", "Starting level:"
                         };
//initilize matrix
LiquidCrystal lcd(RSPin, EPin, D4Pin, D5Pin, D6Pin, D7Pin);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void createPlayer(int middle){

  lc.setLed(0, MAX_LEN_MATRIX, middle, 1);
  lc.setLed(0, MAX_LEN_MATRIX, middle - 1, 1);
  lc.setLed(0, MAX_LEN_MATRIX, middle + 1, 1);
  lc.setLed(0, MAX_LEN_MATRIX - 1, middle + 1, 1);
  lc.setLed(0, MAX_LEN_MATRIX - 1, middle - 1, 1);
  }

 void updatePositionOFPlayer(){
  
  int xValue = analogRead(joyX);
  
  if ( xValue > TRESHHOLD_JOYSTICK_MAXIM){
    if( playerPosition > MIN_LEN_MATRIX +1)
    {
      playerPosition--;
    }
  }
    if ( xValue < TRESHHOLD_JOYSTICK_MIN){

      if( playerPosition < MAX_LEN_MATRIX -1 ){
        playerPosition++;
      }
    }
}

void generateRandomFallsStars(int timeLevel){
  if( millis() - lastStarFall > timeLevel){
    lastStarFall = millis();
    long randomPlace = random(MIN_LEN_MATRIX  , MAX_LEN_MATRIX + 1 );
    
    //Serial.print(randomPlace);
    matrix[MIN_LEN_MATRIX][randomPlace] = 1;
    
  }
  
}

void starsFalls(){ 

  int playerLocation  = playerPosition; // I take player position to  create the colision

  for(int i = MAX_LEN_MATRIX; i > MIN_LEN_MATRIX ; i--){
     for(int j = MIN_LEN_MATRIX; j <= MAX_LEN_MATRIX; j++){
        
        matrix[i][j] = matrix[i-1][j];
        matrix[i-1][j] = 0;
     }
  }




  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 0){
      Score = Score + 1;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 0){
     Score = Score + 1;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 0){
     Score = Score + 1;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 1){
      Score = Score + 2;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 1){
     Score = Score + 2;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 1){
     Score = Score + 2;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 2){
      Score = Score + 3;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 2){
     Score = Score + 3;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 2){
     Score = Score + 3;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 3){
      Score = Score + 4;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 3){
     Score = Score + 4;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 3){
     Score = Score + 4;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 4){
      Score = Score +5;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 4){
     Score = Score +5;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 4){
     Score = Score +5;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  if( matrix[MAX_LEN_MATRIX - 1][playerLocation] == 1){
    if(level == 5){
      Score = Score + 6;
      matrix[MAX_LEN_MATRIX - 1][playerLocation] = 0;
    }
  }   
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] == 1){
    if(level == 5){
     Score = Score +6;
     matrix[MAX_LEN_MATRIX - 1][playerLocation - 1] = 0;
    }  
  }
  if( matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] == 1){
     if(level == 5){
     Score = Score + 6;
     matrix[MAX_LEN_MATRIX - 1][playerLocation + 1] = 0;
     }
  }

  
  if( Score > 5 && increaseTimeFalls == 0){
    timeToRegenerate = timeToRegenerate - 900;
    timeToFalls = timeToFalls - 100;
    increaseTimeFalls = 1;
    level = 1;
  }

   if( Score > 15 && increaseTimeFalls == 1){
    timeToRegenerate = timeToRegenerate - 300;
    timeToFalls = timeToFalls - 75;
    increaseTimeFalls = 2;
    level = 2;
    
  }

   if( Score > 45 && increaseTimeFalls == 2){
    timeToRegenerate = timeToRegenerate - 75;
    timeToFalls = timeToFalls - 30;
    increaseTimeFalls = 3;
    level = 3;
    
  }

  if( Score > 65 && increaseTimeFalls == 3){
    timeToRegenerate = timeToRegenerate - 100;
    timeToFalls = timeToFalls - 40;
    increaseTimeFalls = 4;
    level = 4;
    

  }

  if( Score > 100 && increaseTimeFalls == 4){
    timeToRegenerate = timeToRegenerate - 200;
    timeToFalls = timeToFalls - 40;
    increaseTimeFalls = 5;
    level = 5;

  }
    
  
  for( int j = MIN_LEN_MATRIX ; j <= MAX_LEN_MATRIX; j++)
      if(matrix[MAX_LEN_MATRIX][j] == 1){
        lives --;
      }

     
}

void drawMatrix(){
  
       for(int i = MIN_LEN_MATRIX; i <= MAX_LEN_MATRIX; i++){
             for(int j = MIN_LEN_MATRIX; j <= MAX_LEN_MATRIX; j++){
                if(matrix[i][j] == 1){
                  lc.setLed(0, i, j, 1);
                }
             }

       }
  
}

void finish(){
  lc.clearDisplay(0);
   for(int i = MIN_LEN_MATRIX; i <= MAX_LEN_MATRIX; i++){
             for(int j = MIN_LEN_MATRIX; j <= MAX_LEN_MATRIX; j++){
                if(sadFace[i][j] == 1){
                  lc.setLed(0, i, j, 1);
                }
             }

       }
}

void treeChristmas(){
  lc.clearDisplay(0);
   for(int i = MIN_LEN_MATRIX; i <= MAX_LEN_MATRIX; i++){
             for(int j = MIN_LEN_MATRIX; j <= MAX_LEN_MATRIX; j++){
                if(tree[i][j] == 1){
                  lc.setLed(0, i, j, 1);
                }
             }

       }
}

void startGame(){
  if( lives != 0){
      generateRandomFallsStars(timeToRegenerate);
      if( millis() - lastFalls > timeToFalls ){
      updatePositionOFPlayer();
      lc.clearDisplay(0);
      createPlayer(playerPosition);
      drawMatrix();
      lastFalls= millis();
      starsFalls();
//      Serial.print("Score:");
//      Serial.println(Score);
//      Serial.print("Lives:");
//      Serial.println(Lives);
      }
  }

  
}


//Menu part

void readXAxis() {
  int xValue = analogRead(xAxisPin);
  if (xValue < minThreshold && joyMoved == false) {
    if (currentPositionMenu1 < 4) {
      currentPositionMenu1++;
    } else {
      currentPositionMenu1 = 1;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentPositionMenu1 > 1) {
      currentPositionMenu1--;
    } else {
      currentPositionMenu1 = 4;
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void principalMenu()//there is where I initlize variable for starting game as level score 
{
  treeChristmas();
  readXAxis();
  switch (currentPositionMenu1)
  {
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[1]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[4]);
      lcd.setCursor(0, 1);
      lcd.print(Meniuri[2]);
      lcd.setCursor(5, 1);
      lcd.print(Meniuri[6]);
      
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[0]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[4]);
      lcd.setCursor(0, 1);
      lcd.print(Meniuri[3]);
      lcd.setCursor(5, 1);
      lcd.print(Meniuri[6]);
      
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[0]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[5]);
      lcd.setCursor(0, 1);
      lcd.print(Meniuri[2]);
      lcd.setCursor(5, 1);
      lcd.print(Meniuri[6]);
      break;
      
    case 4:
      lcd.setCursor(0, 0);
      lcd.print(Meniuri[0]);
      lcd.setCursor(7, 0);
      lcd.print(Meniuri[4]);
      lcd.setCursor(0, 1);
      lcd.print(Meniuri[2]);
      lcd.setCursor(5, 1);
      lcd.print(Meniuri[7]);
      break;
  }
  if (inputTime == 0 || millis() - inputTime > 300) {
    //Serial.print(buttonPressed());
    if (buttonPressed() == 1) {
      if (currentPositionMenu1 == 2) {
        //delay(250);
        inputTime = millis();
        position = 2;
        lcd.clear();
      }
    }

    if (buttonPressed() == 1) {
      if (currentPositionMenu1 == 3) {
        //delay(250);
        inputTime = millis();
        position = 3;
        lcd.clear();
      }
    }
    if (buttonPressed() == 1) {
      if (currentPositionMenu1 == 1) {
        position = 4;
        lcd.clear();
        StartGameTime = millis();
        Score =0;
        if(livesHasChanged == false){
          lives = 3;
        }
        livesHasChanged = false;
        level = setLevel;
        if(level == 0){
           timeToRegenerate = 2200;
           timeToFalls = 400;
           //Score = 0;
           increaseTimeFalls = 0;

        }

          if(level == 1){
           timeToRegenerate = 1300;
           timeToFalls = 300;
           //Score = 6;
           increaseTimeFalls = 1;
        }

          if(level == 2){
           timeToRegenerate = 1000;
           timeToFalls = 225;
           //Score = 11;
           increaseTimeFalls = 2;
        }

          if(level == 3){
           timeToRegenerate = 925;
           timeToFalls = 195;
           //Score = 16;
           increaseTimeFalls = 3;
        }

          if(level == 4){
           timeToRegenerate = 825;
           timeToFalls = 155;
           //Score = 21;
           increaseTimeFalls = 4;
        }

          if(level == 5){
           timeToRegenerate = 625;
           timeToFalls = 115;
           //Score = 26;
           increaseTimeFalls = 5;
        }
       
        refresh = false;
      }
    }

     if (buttonPressed() == 1) {
      if (currentPositionMenu1 == 4) {
        //delay(250);
        inputTime = millis();
        position = 5;
        lcd.clear();
      }
    }
  }


}

void changeLevel() {
  int yValue = analogRead(yAxisPin);
  if (yValue < minThreshold && YjoyMoved == false) {
    if (setLevel < 5) {
      setLevel++;
    } else {
      setLevel = 0;
    }
    YjoyMoved = true;
  }

  if (yValue > maxThreshold && YjoyMoved == false) {
    if (setLevel > 0) {
      setLevel--;
    } else {
      setLevel = 5;
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

void settingsChange() {   // thsi function si to move between settings options
  int xValue = analogRead(xAxisPin);
  if (xValue < minThreshold && joyMoved == false) {
    if (positionSettings < 2) {
      positionSettings++;
    } else {
      positionSettings = 1;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (positionSettings > 1) {
      positionSettings--;
    } else {
      positionSettings = 2;
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void changeLives(){
  {
  int yValue = analogRead(yAxisPin);
  if (yValue < minThreshold && YjoyMoved == false) {
    if (lives < 3) {
      livesHasChanged = true;
      lives++;
    } else {
      lives = 1;
      livesHasChanged=true;
    }
    YjoyMoved = true;
  }

  if (yValue > maxThreshold && YjoyMoved == false) {
    if (lives > 1) {
      livesHasChanged=true;
      lives--;
    } else {
      lives = 3;
      livesHasChanged=true;
    }
    YjoyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    YjoyMoved = false;
  }
}
}

void settingsMenu() {
  settingsChange();
  if( positionSettings == 1){
    
  
  lcd.setCursor(0, 0);
  lcd.print(">Level:");
  lcd.print(setLevel);
  changeLevel();
  //Serial.println("da");
  //Serial.print(buttonPressed());

  lcd.setCursor(0, 1);
  lcd.print(" Lives:");
  lcd.print(lives);
  if (buttonPressed() == 1 && millis() - inputTime > 350) {
    //delay(250);
    inputTime = millis();
    lcd.clear();
    position = 1;
  }
}

if( positionSettings == 2){
    
  
  lcd.setCursor(0, 0);
  lcd.print(" Level:");
  lcd.print(setLevel);
  //changeLevel();
  //Serial.println("da");
  //Serial.print(buttonPressed());

  lcd.setCursor(0, 1);
  lcd.print(">Lives:");
  lcd.print(lives);
  changeLives();
  if (buttonPressed() == 1 && millis() - inputTime > 350) {
    //delay(250);
    inputTime = millis();
    lcd.clear();
    position = 1;
  }
}


}

void highScoreMenu() {
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
  if (lives != 0) {
    startGame();

    lcd.setCursor(0, 0);
    lcd.print("Lives:");
    lcd.setCursor(6, 0);
    lcd.print(lives);
    lcd.setCursor(8, 0);
    lcd.print("Level:");
    lcd.setCursor(14, 0);
    lcd.print(level);
    lcd.setCursor(0, 1);
    lcd.print(Score);
  }
  else {
    
    refreshF();
    lcd.setCursor(0, 0);
    lcd.print("Well done! Press");
    lcd.setCursor(0, 1);
    lcd.print("Score:");
    lcd.setCursor(6, 1);
    lcd.print(Score);
    lcd.setCursor(14, 1);
    lcd.print("JS");
    for(int i = MIN_LEN_MATRIX; i <= MAX_LEN_MATRIX; i++){
      for(int j = MIN_LEN_MATRIX; j <= MAX_LEN_MATRIX; j++){
          matrix[i][j] = 0;
      }
    }
    finish();
     

    if (Score > highScore) {
      highScore = Score;
      EEPROM.write(adresaEeprom, highScore);
    }
    if (buttonPressed() == 1 && millis() - inputTime > 350) {
       
      lc.clearDisplay(0);
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

void infoMenu(){
  lcd.setCursor(0, 0);
  lcd.print("Craciun fericit!");
  lcd.setCursor(0, 1);  
  lcd.print("@UnibucRobotics");
  
  if (buttonPressed() == 1 && millis() - inputTime > 350) {
    //delay(250);
    inputTime = millis();
    lcd.clear();
    position = 1;
  }
  
}
void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.clear();

  
  randomSeed(analogRead(5));
  lc.shutdown(0, false);
  lc.setIntensity(0, 2); // 0,15
  lc.clearDisplay(0);

}



void loop()
{
  switch (position)
  {

    case 3:
      highScoreMenu();
      break;
    case 2:
      settingsMenu();
      break;
    case 4:
      startMenu();
      break;
    case 5:
      infoMenu();
      break;
    default:
      principalMenu();
      break;
  }
}
