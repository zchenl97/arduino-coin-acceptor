#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define COIN_PIN 2 //Or interrupt pin
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Keypad part
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {10, 4, 8, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 12, 13}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Motor part
Servo myservo;

// gets incremented by the ISR;
// gets reset when coin was recognized (after train of pulses ends);
volatile int pulses = 0;
volatile long timeLastPulse = 0;
unsigned long previousMillis = 0;

//Items price
float slot1 = 1.50; 
float slot2 = 2.00;
float slot3 = 2.50;
float slot4 = 3.00;
float price = 0.0;
float change = 0.0;
float credit = 0.0;

//State part
int STATE;
int SELECTING = 0;
int SELECTED = 1;
int FINISHING = 2;
int RESET = 3;
int SELECTED_ITEM = 0;

void setup(){
  //LCD part
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Welcome! Please");
  lcd.setCursor(0,1);
  lcd.print("choose ur choco!");
  STATE = SELECTING;
}

void loop(){
  if (STATE == SELECTING){
    char key = keypad.getKey();
    if (key == '1'){
      SELECTED_ITEM = 1;
      price = slot1;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(SELECTED_ITEM);
    }
    else if (key == '2'){
      SELECTED_ITEM = 2;
      price = slot2;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(SELECTED_ITEM);
    }
    else if (key == '3'){
      SELECTED_ITEM = 3;
      price = slot3;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(SELECTED_ITEM);
    }
    else if (key == '4'){
      SELECTED_ITEM = 4;
      price = slot4;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(SELECTED_ITEM);
    }
    
    if (key == '#' && SELECTED_ITEM > 0){
      lcd.clear();
      lcd.print("Price  : RM");
      lcd.print(price);
      STATE = SELECTED;
      previousMillis = millis();
    }
  }
  
  else if (STATE == SELECTED){
    lcd.setCursor(0,1);
    lcd.print("Credits: RM");
    long timeFromLastPulse = millis() - timeLastPulse;
    pinMode(COIN_PIN, INPUT);
    attachInterrupt(0, coinISR, RISING);  //coin wired to D2 pin

    if (credit < price){
      if (pulses > 0 && timeFromLastPulse > 200){
        //sequence of pulses stopped; determine the coin type;
        if (pulses == 2){
          credit += .1; //10sen detected
          }
        else if (pulses == 4){
          credit += .20; //20sen detected
        }
        else if (pulses == 6){
          credit += .50; //50sen detected
        }
        pulses = 0; 
      } 
      if(((unsigned long)(millis() - previousMillis)) >= 20000){ //If payment is not done in 10 seconds, reset & return credit
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Timeout bro");
        delay(2000);
        credit = 0.0;
        STATE = RESET;
        goto rst;
      }
      lcd.print(credit);
    }
    else if(credit == price){
      credit = 0.0;
      STATE = FINISHING;
    }
    else if(credit > price){  //display and return change
      change = credit - price;
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Your change:");
       lcd.setCursor(0,1);
       lcd.print(change);
       delay(3000);
       credit = 0.0;
       STATE = FINISHING;
    }
  }
  
  else if (STATE == FINISHING){
    test:
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Thank you");
    lcd.setCursor(3,1);
    lcd.print(" & enjoy!");
    
    //Motor part
    if (SELECTED_ITEM == 1) {
      rotateMotor(9);
    }
    else if (SELECTED_ITEM == 2) {
      rotateMotor(6);
    }
    else if (SELECTED_ITEM == 3) {
      rotateMotor(5);
    }
    else if (SELECTED_ITEM == 4) {
      rotateMotor(3);
    }
    delay(3000);
    lcd.clear();
    STATE = RESET;
  }
  else if (STATE == RESET){
    rst:
    lcd.clear();
    STATE == SELECTING;
    setup();
  }
}

// executed for every pulse;
void coinISR(){
  pulses++;
  timeLastPulse = millis();
}

void rotateMotor(int in1) { //rotate a specific number of degrees (negitive for reverse movement) //speed is any number from .01 -> 1 with 1 being fastest – Slower is stronger
  myservo.attach(in1);
  myservo.write(180);
  delay(3000);
  myservo.write(93);
  }
