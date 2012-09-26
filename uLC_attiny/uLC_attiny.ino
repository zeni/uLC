// Program for live coding of algo music on u-controller
// Program for sound/parsing/algo computation (for ATTINY85)
// ATTINY is connected serially with interface microcontroller.
// SSP 2012.09.24

#include <ATSerial.h>
// this is a modified SoftwareSerial library. Only the rate 2400 is available (to reduce size)

const byte MAX_CHAR = 50; // max number of characters in formula
const byte RX_PIN=3; // serial RX pin

char output[MAX_CHAR],sortie[MAX_CHAR],startChar; // char stacks / first character received
unsigned long pileRPN[MAX_CHAR]; // stack for RPN computation
byte pileVAR[6]; // stack for variable computation
unsigned long t=0; // time variable
byte pp=0,s=0,ss=0,x=1,y=1; // counters for pile, sortie, algo / variables
boolean firstAlgo=true,multipleDigit=false,firstChar=true; // new algo is received? / multiple digits? / first char received ?
SoftwareSerial serial = SoftwareSerial(RX_PIN, 4); // TX pin is not used

void setup() {  
  cli(); // Disable global interrupts 
  // use 64Mhz clock
  PLLCSR= (1 << PLLE); 
  for(int i=0;i<100;i++);
  PLLCSR|= (1 << PCKE);
  // PB0 (LED), PB1 (OC1A) and PB4 (TX_PIN) as outputs 
  DDRB =  (1 << PB0)|(1 << PB1);
  // PWM based on OCR1A / toggle output on OC1A=PB1 / prescaler=1 (64MHz)
  TCCR1 = (1 << PWM1A)|(1 << COM1A1)|(1 << CS10);
  sei(); // Enable global interrupts
  //serial
  serial.begin(2400);
} 

ISR(TIMER1_OVF_vect) // Interrupt Service Routine 
{
  byte p=0;
  for (byte i=0;i<ss;i++) { // Loop on the RPN formula as an array of char (=sortie)
    switch(output[i]) { 
    case 't': // variable=> just add to pile
      pileRPN[p++]=t;
      multipleDigit=false; // not a digit
      break; 
    case 'x': // variable=> just add to pile
      pileRPN[p++]=x;
      multipleDigit=false; // not a digit
      break; 
    case 'y': // variable=> just add to pile
      pileRPN[p++]=y;
      multipleDigit=false; // not a digit
      break; 
    case '+': // perform ADDITION, de-pile
      p--; //de-pile
      pileRPN[p-1]+=pileRPN[p]; // perform operation
      multipleDigit=false; // not a digit
      break;
    case '|':  // perform OR, de-pile
      p--;
      pileRPN[p-1]|=pileRPN[p];
      multipleDigit=false;
      break;
    case '&': // perform AND, de-pile
      p--;
      pileRPN[p-1]&=pileRPN[p];
      multipleDigit=false;
      break;
    case '^':  // perform XOR, de-pile
      p--;
      pileRPN[p-1]^=pileRPN[p];
      multipleDigit=false;
      break;
    case '-':  // perform SUBSTRACTION, de-pile
      p--;
      pileRPN[p-1]-=pileRPN[p];
      multipleDigit=false;
      break;
    case '*':  // perform MULTIPLICATION, de-pile
      p--;
      pileRPN[p-1]*=pileRPN[p];
      multipleDigit=false;
      break;
    case '/':  // perform DIVISION, de-pile
      p--;
      pileRPN[p-1]/=pileRPN[p];
      multipleDigit=false;
      break;
    case '>':  // perform BIT SHIFT RIGHT, de-pile
      p--;
      pileRPN[p-1]=pileRPN[p-1]>>pileRPN[p];
      multipleDigit=false;
      break;
    case '<':  // perform BIT SHIFT LEFT, de-pile
      p--;
      pileRPN[p-1]=pileRPN[p-1]<<pileRPN[p];
      multipleDigit=false;
      break;
    case '%':  // perform MODULO, de-pile
      p--;
      pileRPN[p-1]=pileRPN[p-1]%pileRPN[p];
      multipleDigit=false;
      break;
    default: // digit, add to pile except if multiple digits (in this case process the digits)
      if (multipleDigit) {
        pileRPN[p-1]*=10;
        pileRPN[p-1]+=output[i]-48;
      } 
      else {
        pileRPN[p++]=output[i]-48;
        multipleDigit=true;
      }
      break;
    }
  }
  OCR1A=pileRPN[0]; // PWM output
  if(OCR1A>250) PORTB^=(1 << PB0); // some LED fun
  t++; // variable inc.
}

void loop() {
  if (serial.available()) {
    char a=serial.read();
    if(firstChar) {
      TIMSK = (0 << TOIE1); // disable interrupt
      startChar=a; // first char received
      s=0; // counter of chars received 
      firstChar=false;
    }
    if(a=='E') {
      switch(startChar) {
      case 'x': // var x
      case 'y': // var y
        pp=0;
        for (byte j=0;j<s;j++) {
          switch(sortie[j]) { 
          case 'x':
            pileVAR[pp++]='x';
            multipleDigit=false;
            break; 
          case 'y':
            pileVAR[pp++]='y';
            multipleDigit=false;
            break; 
          case '=':
            pp--;
            multipleDigit=false;
            if(pileVAR[pp-1]=='x')
              x=pileVAR[pp];
            else
              y=pileVAR[pp]; 
            break;
          default: // digit
            if (multipleDigit) {
              pileVAR[pp-1]*=10;
              pileVAR[pp-1]+=sortie[j]-48;
            } 
            else {
              pileVAR[pp++]=sortie[j]-48;
              multipleDigit=true;
            }
            break;
          }
        }
        break;
      case 'E':  // next algo is a new algo
        firstAlgo=true;
        break;
      case 'r':// reset t
        t=0;
        break;
      case 's': // stop sound, new algo
        firstAlgo=true;
        ss=0;
        break;
      default: // input is algo
        if(firstAlgo) {
          ss=0;
          t=0;
        }
        for(byte i=0;i<s;i++) output[ss+i]=sortie[i];
        ss+=s;
        firstAlgo=false;
        break;
      }
      firstChar=true;
      TIMSK = (1 << TOIE1);
    }
    else
      sortie[s++]=a;
  }
}



















































