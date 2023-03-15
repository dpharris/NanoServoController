// Nano/AVR Servo controller Native
//
// == It uses servo-easing to obtain a slow smooth movement
//    See: https://github.com/ArminJo/ServoEasing
//
// It controls a variable number of input pins (nservos<16) controlling an equal number of Servos
//
// Each input is monitored for change, and on a change the assocoaited servo is moved to one of two postions.  
//   -- the inputs can be constant, or used as a toggle (uncomment the ALTERNATE define statement)
//
// Each input is monitored for change, and on a change the assocoaited servo is moved to one of two postions.  
//   -- the inputs can be constant, or used as a toggle (uncomment the ALTERNATE define statement)
//
// Four buttons control the two end positions of each servo:
//   Nextservo - selects the next servo, and rolls over.  If held >2 seconds will reset to the first servo.  
//   Nextposn - selects betwen the two positions of the selected servo
//   Up - increases the selected postition
//   Down - decreases the selected position
//  To program servo-positions:
//   Use nextservo to move to one of the servos, you can tell which servo by using nextposn to see which servo responds.
//   Then use Up and Down to change the selected position, press nextposn and then Up/Down to change the other position.  
// The position data is saved to EEPROM after 20 seconds of inactivity of the program buttons or the inputs.

//          +------+
//  input1--| Nano |--servo1
//         ...
//  input8--|      |--servo9
//          +------+
//          / | \  \
// nextservo  |  up down
//         nextposn

// Using a SPDT switch per line:
//   Connect one side to 5V, and the other to ground, and the centre to one input
//   Comment out the ALTERNATE code line.
//   The servo should follow the switch position.

// Using a pair of buttons per line:
//   Connect one side of both buttons to an input.  
//   Connect the other side of one button to 5V, and the other side of the second button to ground
//   Comment out the ALTERNATE line in the code
//   Pushing one button should cause the servo to go to one position, and pushing the other button
//     should cause the servo to go to the other position.  

// Using single button:
//   Connect one side to ground and the other side to an input
//   Uncomment the ALTERNATE line in the code
//   The servo should go to teh other postion on each button push.  

// Connecting to LCC-Tower
//   Choose a LCC Tower port, connect its pins to the eight input pins on this board.
//   For each channel:
//     Select ouput "Active Hi"
//     Program a "command event" to "Set output high"
//     Program a second "command event" to "Set output low"
//     On receiving these eventids, the servo should move to one position of the other.  

#if defined(__AVR__)
#else
  #error "This sketch is primarily for the AVR series, like the UNO or NANO"
#endif

#define AllowDebug   // uncomment this line to be able to use 8 inputs and 8 servos, 
                // leave-in to do usb debugging and have 6 inputs and 6 servos
#ifdef AllowDebug
  #define debug(x) Serial.print(x)  // commnet out this line if you do not want the debugging
#else
  #define debug(x)
#endif

//#include <Servo.h>
#include <ServoEasing.hpp>  // great library for gettin slow servo action, including bounces
#include <EEPROM.h>         // will save servo positions, including current position for each


//#define ALTERNATE         // activate Alternate action -- good for buttons, as each button push will toggle the servo position
//#define INIT              // uncomment to initialize the EEPROM to default positions
#define SERVOSPEED 50       // set to your preference
#define MINPOSN 0           // minimum position for a servo in degrees
#define MAXPOSN 180         // minimum position for a servo in degrees


#ifdef AllowDebug
  #define nservos 6
  uint8_t servoPin[nservos]   = {2, 3, 4, 5, 6, 7};  // digital, not rx/tx
  uint8_t controlPin[nservos] = {8, 9,10,11,12,13};  // only using 6 servos, cannot use  0&1 as Serial is used for debugging
#else
  #define nservos 8
  uint8_t servoPin[nservos]   = {0, 1, 2, 3, 4, 5, 6, 7};  // remaining digital + two analog
  uint8_t controlPin[nservos] = {8, 9,10,11,12,13,18,19};  // including rx and tx -- DO NOT USE SERIAL for debugging
#endif

uint8_t spos[3][nservos];   // 0=firstPos, 1=secondPos, 2=currentPos  This is saved to EEPROM
ServoEasing servo[nservos];     // use this with regular Servo lib

// Button pins
#define nextservo 14  // A0   Next-Servo button
#define nextposn  15  // A1   Next-Position button
#define up        16  // A2   Up button
#define down      17  // A3   Down buttom

// activecontrol variables
uint8_t activeServo = 0;
uint8_t activePos = 0;
long lastchange = 0;     // remember when last input or button change occurs

void inputScan() {     // set servo to match input, if it has changed
  static uint8_t state[nservos];
 #ifdef ALTERNATE
  for(uint8_t s=0; s<nservos; s++) {
    if( !digitalRead(controlPin[s]) ) {
      if( state[s]==0 ) {  // button newly down
        state[s] = 1; // mark button down
        spos[2][s] = !spos[2][s];
        servo[s].startEaseTo(spos[ spos[2][s] ][ s ]);    // ServoEasing command, done behind the scenes
        lastchange = millis();
        debug("\n toggle input:"); debug(s); debug("="); debug( spos[ spos[2][s] ][ s ] );  
      }     
    } else 
      state[s] = 0;
  }
 #else     
  for(uint8_t s=0; s<nservos; s++) {
    if( digitalRead(controlPin[s]) != spos[2][s] ) {  // if the input does not match the servo's position
      spos[2][s] = !spos[2][s];
      servo[s].startEaseTo(spos[ spos[2][s] ][ s ]);    // ServoEasing command, done behind the scenes
      lastchange = millis();
      debug("\n input:"); debug(s); debug("="); debug(spos[ spos[2][s] ][ s ]);
    } 
  }
 #endif
}

void setup() {
  #ifdef AllowDebug
    Serial.begin(115200);
    delay(2000);
    debug("\n\n NAno/AVR ServoCOntroller Native\n");
  #endif

  // check EEPROM, and if not intialized, intialize it
  uint8_t mn = EEPROM.read(0);                    // read magic number at location 0
  if(mn != 0x5A) {                                // if not present, write it and initialize EEPROM
    debug("\nInitialize eeprom\n");
    EEPROM.write(0, 0x5A);
    for(uint8_t i=0; i<nservos; i++) {            // .. then for each servo, set its positions
      spos[0][i] = (MAXPOSN-MINPOSN)/2 + 5;       // first position half-way plus a bit    
      spos[1][i] = (MAXPOSN-MINPOSN)/2 - 5;       // second position
      spos[2][i] = 0;                             // active position: 0-1
    }
    EEPROM.put(1,spos);                           // and save spos to the EEPROM location 1
  }
 
  EEPROM.get(1,spos);                      // retrieve servo positions and current position
  debug("\nRetrieve data from eeprom\n");
  for(int i=0; i<nservos; i++) {
    debug("    "); debug(i); debug(":"); debug(spos[0][i]);debug(","); debug(spos[1][i]);debug(","); debug(spos[2][i]); debug("\n");
  }
  debug("\nInitialized");

  // Attach the servos
  for(int i=0; i<nservos; i++) {           // and setup and update the servos
    servo[i].attach(servoPin[i]);
    debug("\nAttached "); debug(i);
    servo[i].setEasingType(EASE_CUBIC_IN_OUT); // user choice
    servo[i].write( spos[spos[2][i]][i] );
  }
  debug("\nAll attached");
    
  setSpeedForAllServos(SERVOSPEED);  // common to all servos
  pinMode(up, INPUT_PULLUP);         // setup the button pins
  pinMode(down, INPUT_PULLUP);
  pinMode(nextservo, INPUT_PULLUP);
  pinMode(nextposn, INPUT_PULLUP);
  for(int i=0; i<nservos; i++) {     // setup the servo pins
    pinMode(controlPin[i], INPUT_PULLUP);
    pinMode(servoPin[i], OUTPUT);
  }
}

void loop() {
  uint8_t delayt = 200;              // default delay for button presses
  // Button processing
  while(digitalRead(up)==0) {         // "Up" button
    lastchange = millis();
    if( spos[activePos][activeServo] < MAXPOSN ) spos[activePos][activeServo]++;
    servo[activeServo].write(spos[activePos][activeServo]);
    debug("\n Up ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" endpoint="); debug(spos[activePos][activeServo]);
    delayt -= 10;               // speed up the movement with longer press of the button
    if(delayt<50) delayt=50;
    delay(delayt);
  }
  while(digitalRead(down)==0) {      // "Down" button
    lastchange = millis();             // prolonged press gives accelerating movement
    if( spos[activePos][activeServo] > MINPOSN ) spos[activePos][activeServo]--;
    servo[activeServo].write(spos[activePos][activeServo]);
    debug("\n Down ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" endpoint="); debug(spos[activePos][activeServo]);
    delayt -= 10;                    // decrease the period between movements
    if(delayt<50) delayt=50;         // .. but not too fast!
    delay(delayt);                   // delay between each movement
  }
  if(digitalRead(nextservo)==0) {        // "Next Servo" button
    lastchange = millis();
    servo[activeServo].write( spos[spos[2][activeServo]][activeServo] );
    while( digitalRead(nextservo)==0 ) {   // if button pressed for 2 seconds, revert to the first servo
      if( (millis()-lastchange)>2000 ) activeServo = nservos;
    }
    if( ++activeServo >= nservos) activeServo = 0;
    servo[activeServo].startEaseTo(  spos[activePos][activeServo] );
    debug("\n next servo ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" endpoint="); debug(spos[activePos][activeServo]);
    delay(200);
  }
  if(digitalRead(nextposn)==0) {        // "Next Position" button
    lastchange = millis();
    if( ++activePos > 1) activePos = 0;
    servo[activeServo].startEaseTo( spos[activePos][activeServo] );
    debug("\n next pos ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" endpoint="); debug(spos[activePos][activeServo]);
    delay(200);
  }
  // update EEPROM after servo mods
  if(lastchange!=0 && millis()>(lastchange+20000)) {   // save the positions of the servos if last change was 20s ago
    EEPROM.put(1,spos);                               // save settings
    lastchange = 0;
    debug("\n Saved\n");
    for(int i=0; i<nservos; i++) {
      debug("    "); debug(i); debug(":"); debug(spos[0][i]);debug(","); debug(spos[1][i]);debug(","); debug(spos[2][i]); debug("\n");
    }
    debug("\n");
    activeServo = 0;
    // put servos back to their saved positions
    for(int i=0; i<nservos; i++) {
      servo[i].startEaseTo( spos[spos[2][i]][i] ); 
    }
  }
 
  // process all servo inputs
  inputScan();
 
  // delay loop
  delay(100);
}
