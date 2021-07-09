//Binary Decoder Soldering Kit, Version 1.5
//By: Jim Heaney, March 2021
//CC-BY-SA-NC

//Settings:
const int mode = 0; //Set Display Mode, 0 for common cathide, 1 for anode.
const int bright = 1000; //Sets delay on multiplexed display. Setting higher will increase brightness but may introduce flicker.
const int bitbright = 0; // Sets the duty cycle of the binary LEDs. Higher number is a lower duty cycle.

//Default Settings:
//Red: 1, 50, 0
//Blue: 0, 50, 5
//Green: 0, 1000, 0

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

//Number
byte integer = 0;

//Delay time
unsigned long delaytime = 0;
  
//Number button debouncing integers
bool upstate = 0;
bool downstate = 0;

//Byte button debouncing integers
bool bit0state = 0;
bool bit1state = 0;
bool bit2state = 0;
bool bit3state = 0;
bool bit4state = 0;
bool bit5state = 0;
bool bit6state = 0;
bool bit7state = 0;

//Number Butttons
const int upButton = 4;
const int downButton = 3;

//Byte buttons
const int Bit0I = 10;
const int Bit1I = 9;
const int Bit2I = 8;
const int Bit3I = 11;
const int Bit4I = 12;
const int Bit5I = 13;
const int Bit6I = 14;
const int Bit7I = 15;

//Soft reset button
const int resetbutton = 2;

// Byte LEDs
const int Bit0O = 7;
const int Bit1O = 6;
const int Bit2O = 5;
const int Bit3O = 4;
const int Bit4O = 3;
const int Bit5O = 2;
const int Bit6O = 1;
const int Bit7O = 0;

//Bit statuses
bool bit0 = 0;
bool bit1 = 0;
bool bit2 = 0;
bool bit3 = 0;
bool bit4 = 0;
bool bit5 = 0;
bool bit6 = 0;
bool bit7 = 0;

//Digit Segments
const int A = 15; 
const int B = 10; 
const int C = 12; 
const int D = 14; 
const int E = 16; 
const int F = 13; 
const int G = 11; 

//Digit Values
byte dig1val = 0;
byte dig2val = 0;
byte dig3val = 0;

//Digit Select
const int dig1 = 5; 
const int dig2 = 6; 
const int dig3 = 7; 

//Variables to store MCP23017 data:
int buttons = 0;
int bitbrightcurrent = 0;

Adafruit_MCP23017 mcp;

void setup() 
{
  mcp.begin();

  //Set all byte LEDs as outputs
  mcp.pinMode(Bit0O, OUTPUT);
  mcp.pinMode(Bit1O, OUTPUT);
  mcp.pinMode(Bit2O, OUTPUT);
  mcp.pinMode(Bit3O, OUTPUT);
  mcp.pinMode(Bit4O, OUTPUT);
  mcp.pinMode(Bit5O, OUTPUT);
  mcp.pinMode(Bit6O, OUTPUT);
  mcp.pinMode(Bit7O, OUTPUT);

  //Set all byte numbers as outputs
  mcp.pinMode(Bit0I, INPUT);
  mcp.pinMode(Bit1I, INPUT);
  mcp.pinMode(Bit2I, INPUT);
  mcp.pinMode(Bit3I, INPUT);
  mcp.pinMode(Bit4I, INPUT);
  mcp.pinMode(Bit5I, INPUT);
  mcp.pinMode(Bit6I, INPUT);
  mcp.pinMode(Bit7I, INPUT);

  //Set all segments to outputs
  pinMode(G, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(A, OUTPUT);
  
  //Set number buttons as inputs
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);

  //Set reset pin as an input
  pinMode(resetbutton, INPUT);

  //Set digit select pins as outputs
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  if (mode == 0){
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2,HIGH);
    digitalWrite(dig3, HIGH);
  }
  if (mode == 1){
    digitalWrite(dig1, LOW);
    digitalWrite(dig2,LOW);
    digitalWrite(dig3, LOW);
  }
  //Startup lights
  delay(200);
  mcp.digitalWrite(Bit0O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit1O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit2O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit3O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit4O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit5O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit6O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit7O, HIGH);
  delay(200);
  mcp.digitalWrite(Bit0O, LOW);
  mcp.digitalWrite(Bit1O, LOW);
  mcp.digitalWrite(Bit2O, LOW);
  mcp.digitalWrite(Bit3O, LOW);
  mcp.digitalWrite(Bit4O, LOW);
  mcp.digitalWrite(Bit5O, LOW);
  mcp.digitalWrite(Bit6O, LOW);
  mcp.digitalWrite(Bit7O, LOW);


}

// the loop function runs over and over again forever
void loop() 
{
  //If reset button is pressed, reset counter to 0 but do not go through setup phase.
  if(digitalRead(resetbutton) == 0){
    integer = 0;
  }
  
  //Read the state of the increase and decrease buttons. 
  if((digitalRead(upButton) == 1) && (upstate == 0)){
    upstate = 1;
    integer = integer + 1;
    delaytime = millis() + 200;
  }
  
  if(digitalRead(upButton) == 0){
    upstate = 0;
  }
  
  if((digitalRead(downButton) == 1) && (downstate == 0)){
    downstate = 1;
    integer = integer - 1;
    delaytime = millis() + 200;
  }
  
  if (digitalRead(downButton) == 0){
    downstate == 0;
  }
  
  if(delaytime <= millis()){
    upstate = 0;
    downstate = 0;
  }

  //Check that the stae of the bits matches the integer
  bit0 = bitRead(integer, 0);
  bit1 = bitRead(integer, 1);
  bit2 = bitRead(integer, 2);
  bit3 = bitRead(integer, 3);
  bit4 = bitRead(integer, 4);
  bit5 = bitRead(integer, 5);
  bit6 = bitRead(integer, 6);
  bit7 = bitRead(integer, 7);

  //Get the state of the buttons from the MCP as an 8-bit value.
  //This could be done one-at-a-time with mcp.digitalRead(), but getting all 8 at once speeds up the cycle time and therefore increases brightness of the multiplexed LEDs.
  buttons = mcp.readGPIO(1);

  //Read the state of the bit buttons, to see if a new one is released. If so, change the value of the bit and flip the LED on or off. This updating can be handled when needed, or at the end all at once.
  if((bitRead(buttons, 0) == 0) && (bit0state == 0)){
    bit0state = 1;
    bit0 = !bit0;
    //mcp.digitalWrite(Bit0O, bit0);
  }
  if((bitRead(buttons, 1) == 0) && (bit1state == 0)){
    bit1state = 1;
    bit1 = !bit1;
    //mcp.digitalWrite(Bit1O, bit1);
  }
  if((bitRead(buttons, 2) == 0) && (bit2state == 0)){
    bit2state = 1;
    bit2 = !bit2;
    //mcp.digitalWrite(Bit2O, bit2);
  }
  if((bitRead(buttons, 3) == 0) && (bit3state == 0)){
    bit3state = 1;
    bit3 = !bit3;
    //mcp.digitalWrite(Bit3O, bit3);
  }
  if((bitRead(buttons, 4) == 0) && (bit4state == 0)){
    bit4state = 1;
    bit4 = !bit4;
    //mcp.digitalWrite(Bit4O, bit4);
  }
  if((bitRead(buttons, 5) == 0) && (bit5state == 0)){
    bit5state = 1;
    bit5 = !bit5;
    //mcp.digitalWrite(Bit5O, bit5);
  }
  if((bitRead(buttons, 6) == 0) && (bit6state == 0)){
    bit6state = 1;
    bit6 = !bit6;
    //mcp.digitalWrite(Bit6O, bit6);
  }
  if((bitRead(buttons, 7) == 0) && (bit7state == 0)){
    bit7state = 1;
    bit7 = !bit7;
    //mcp.digitalWrite(Bit7O, bit7);
  }

  //See if any of the bit buttons have been pressed
  if(bitRead(buttons, 0) == 1){
    bit0state = 0;
  }
  if(bitRead(buttons, 1) == 1){
    bit1state = 0;
  }
  if(bitRead(buttons, 2) == 1){
    bit2state = 0;
  }
  if(bitRead(buttons, 3) == 1){
    bit3state = 0;
  }
  if(bitRead(buttons, 4) == 1){
    bit4state = 0;
  }
  if(bitRead(buttons, 5) == 1){
    bit5state = 0;
  }
  if(bitRead(buttons, 6) == 1){
    bit6state = 0;
  }
  if(bitRead(buttons, 7) == 1){
    bit7state = 0;
  }

  //Change any bits that were flipped by button presses
  bitWrite(integer, 0, bit0);
  bitWrite(integer, 1, bit1);
  bitWrite(integer, 2, bit2);
  bitWrite(integer, 3, bit3);
  bitWrite(integer, 4, bit4);
  bitWrite(integer, 5, bit5);
  bitWrite(integer, 6, bit6);
  bitWrite(integer, 7, bit7);

  //Update all of the LEDs
  UpdateAll();

  //Determine each of the decimal values
  dig1val = (integer / 100) % 10;
  dig2val = (integer / 10) % 10;
  dig3val = integer % 10;
  
  //Write each of the digits
  if (mode == 0){
  seg(dig1val);
  digitalWrite(dig1, LOW);
  delayMicroseconds(bright);
  digitalWrite(dig1, HIGH);
  seg(dig2val);
  digitalWrite(dig2, LOW);
  delayMicroseconds(bright);
  digitalWrite(dig2, HIGH);
  seg(dig3val);
  digitalWrite(dig3, LOW);
  delayMicroseconds(bright);
  digitalWrite(dig3, HIGH);
  }
  if (mode == 1){
    seg(dig1val);
    digitalWrite(dig1, HIGH);
    delayMicroseconds(bright);
    digitalWrite(dig1, LOW);
    seg(dig2val);
    digitalWrite(dig2, HIGH);
    delayMicroseconds(bright);
    digitalWrite(dig2, LOW);
    seg(dig3val);
    digitalWrite(dig3, HIGH);
    delayMicroseconds(bright);
    digitalWrite(dig3, LOW);
  }
}

void seg(int count){
  if (mode == 0){
    if (count == 0)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, LOW);
    }
    else if (count == 1)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else if (count == 2)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, LOW);
      digitalWrite(G, HIGH);
    }
    else if (count == 3)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, HIGH);
    }
    else if (count == 4)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else if (count == 5)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, LOW);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else if (count == 6)
    {
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    } 
    else if (count == 7)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else if (count == 8)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else if (count == 9)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, LOW);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
  }
  if (mode == 1){
    if (count == 0)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, HIGH);
    }
    else if (count == 1)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else if (count == 2)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, HIGH);
      digitalWrite(G, LOW);
    }
    else if (count == 3)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, LOW);
    }
    else if (count == 4)
      {
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else if (count == 5)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, HIGH);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else if (count == 6)
    {
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    } 
    else if (count == 7)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
    else if (count == 8)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else if (count == 9)
      {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, HIGH);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
    }
    else {
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    }
  } 
}

void UpdateAll(){
  //This function writes to all the output pins of the MCP23017's A register at once. 
  //The Adafruit library supports reading one whole register or writing to all the pins on both registers, but not writing to one register, oddly enough.

  //First, flip the bits of the byte. MSB to LSB.
  byte temp = 0;
  bitWrite(temp, 0, bitRead(integer, 7));
  bitWrite(temp, 1, bitRead(integer, 6));
  bitWrite(temp, 2, bitRead(integer, 5));
  bitWrite(temp, 3, bitRead(integer, 4));
  bitWrite(temp, 4, bitRead(integer, 3));
  bitWrite(temp, 5, bitRead(integer, 2));
  bitWrite(temp, 6, bitRead(integer, 1));
  bitWrite(temp, 7, bitRead(integer, 0));

  //Next, check if it the LEDs should be off or on based on the bitbright setting. 
  if (bitbrightcurrent >= bitbright){
    bitbrightcurrent = 0;
    Wire.beginTransmission(0x20);
    Wire.write(0x12);
    Wire.write(temp);
    Wire.endTransmission();    
  } else{
    bitbrightcurrent = bitbrightcurrent + 1;
    Wire.beginTransmission(0x20);
    Wire.write(0x12);
    Wire.write(0);
    Wire.endTransmission();    
  }
}
