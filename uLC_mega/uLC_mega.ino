// Program for live coding of algo music on u-controller
// Program for interface (for Arduino MEGA1280): PS/2 keyboard, serial connection and LCD.

/********
 * - Inputs in RPN only.
 * - Two variables (x and y). Can be set at same time (for example: x6=y7= means x=6 and y=7)
 * - multiple sound microcontrollers support.
 * - ENTER without input means that next algo is a new algo.
 * - 'r' is for resetting t at zero.
 * - 's' stops sound and next algo is a new algo.
 * - Inputs (if algo) are added to the stack. For example: input 1 is t2< then ENTER and input 2 is t* then ENTER.
 * The resulting algo is t2<t*. 
 ********/
// SSP 2012.09.24

#include <PS2Keyboard.h> // PS/2 keyboard
#include <LiquidCrystal.h> // LCD
#include <SoftwareSerial.h> // software sserial

const byte MAX_CHAR = 50; // max number of characters in formula
const byte DATA_PIN = 4; // PS/2 keyboard (for MEGA)
const byte IRQ_PIN =  20; // PS/2 keyboard (for MEGA)

char algo[MAX_CHAR]; // the char array representing the input typed with keyboard
byte a; // counters for input size / channel
boolean firstChar,sendAlgo,sendChannel; // new input is received? / input completed (typed)? / send channel only
PS2Keyboard keyboard; // PS/2 keyboard
LiquidCrystal lcd(33, 32, 23, 22, 25, 24); // for MEGA
SoftwareSerial serial = SoftwareSerial(0,40); // RX pin is not used (for MEGA)


void setup() {  
  // Initial values
  firstChar=true;
  sendAlgo=false;
  // PS/2 Keyboard
  keyboard.begin(DATA_PIN, IRQ_PIN);
  // LCD
  lcd.begin(16, 2);
  lcd.print("Ready!");
  // Serial
  serial.begin(2400);
} 

void loop() {
  if (keyboard.available()) {
    char inChar = keyboard.read(); //read key
    if(firstChar) { // initialize variables
      firstChar=false;
      a=0;
      lcd.clear();
      lcd.blink();
    }
    switch(inChar) {
    case ':': // the digit before is the channel, 
      a=0; // write over channel infos
      sendChannel=true;
      serial=SoftwareSerial(0,algo[0]-8); // change channel
      serial.begin(2400);
      break;
    case PS2_ENTER: // special ENTER case (process input)
      if(sendChannel) { // only channel, do not send to attiny
        sendChannel=false;
        firstChar=true;
      } 
      else {
        sendAlgo=true;
        algo[a++]='E'; //store key
      }
      break;
    case PS2_DELETE:
      if(a>0){
        algo[a-1]=' ';
        lcd.setCursor(a-1, 0);
        lcd.print(' ');
        lcd.setCursor(a-1, 0);
        a--;
      }
      break;
    default:
      sendChannel=false;
      algo[a++]=inChar; // store key
      break;
    }
    if(a>15) {
      for(byte i=(a-16);i<a;i++) {
        lcd.setCursor(i-a+16, 0);
        lcd.print(algo[i]);
      }
    } 
    else {
      if(a>0) {
        lcd.setCursor(a-1, 0);
        lcd.print(algo[a-1]);
      }
    }
  }
  if(sendAlgo) {
    sendAlgo=false;
    for (byte j=0;j<a;j++)
      serial.print(algo[j]);
    firstChar=true;
  }
}


