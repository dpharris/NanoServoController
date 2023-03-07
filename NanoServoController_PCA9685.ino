
// Servo controller
// == It uses servo-easing to obtain a slow smooth movement
// == It uses a PCA9685 PWM board to drive the servos
//   NB: this library supprts the use of an I2C PCA9685 servo expander
//       but would need modifications to this code
// Variable number of input pins (nservos), controlling an equal number of Servos
// Each input is monitored for change, and on a change the assocoaited servo is moved to one of two postions.  
//   -- the inputs can be constant, or used as a toggle (uncomment the ALTERNATE define statement)
// Four buttons control the two end positions of each servo:
//   nextservo - selects the next servo, and rolls over
//   nextposn - selects betwen the two positions of the selected servo
//   up - increases the selected postition
//   down - decreases the selected position
//  To program servo-positions:
//   Use nextservo to move to one of the servos, you can tell which servo by using nextposn to see which servo responds.
//   Then use Up and Down to change the selected position, press nextposn and then Up/Down to change the other position.  
// The position data is saved to EEPROM after 20 seconds of inactivity of the program buttons or the inputs.

//           +----------+   +----------+
//   input1--| Nano  SCL|---|          |----servo1
//          ...      SDA|---| PCA9685 ...
//  input16--|          |   |          |----servo16
//           +----------+   +----------+
//             / | \  \     
//    nextservo  |  up down
//            nextposn

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

#define DEBUG   // uncomment this line to be able to use 16 inputs and 16 servos, 
                // leave-in to do usb debugging and have 14 inputs and 14 servos
#ifdef DEBUG
  #define debug(x) Serial.print(x)  // commnet out this line if you do not want the debugging
#else
  #define debug(x)
#endif

// expander
#define USE_PCA9685_SERVO_EXPANDER    // Activating this enables the use of the PCA9685 I2C expander chip/board.
#define EASE_CUBIC_IN_OUT

//#define USE_SERVO_LIB                 // If USE_PCA9685_SERVO_EXPANDER is defined, Activating this enables force additional using of regular servo library.

//#include <Servo.h>
#include <ServoEasing.hpp>  // great library for gettin slow servo action, including bounces
#include <EEPROM.h>         // will save servo positions, including current position for each


//#define ALTERNATE         // activate Alternate action -- good for buttons, as each button push will toggle the servo position
//#define INIT              // uncomment to initialize the EEPROM to default positions
#define SERVOSPEED 50       // set to your preference
#define MINPOSN 0           // minimum position for a servo in degrees
#define MAXPOSN 180         // minimum position for a servo in degrees


#ifdef DEBUG  
  #define nservos 14
  uint8_t controlPin[nservos] = {2,3,4,5,6,7,8, 9,10,11,12,13};  // only using 6 servos, cannot use  0&1 as Serial is used for debugging
#else
  #define nservos 16
  uint8_t controlPin[nservos] = {0,1,2,3,4,5,6,7,8, 9, 10, 11, 12, 13,20,21};  // including rx and tx -- DO NOT USE SERIAL for debugging
#endif

uint8_t spos[3][nservos];   // 0=firstPos, 1=secondPos, 2=currentPos  This is saved to EEPROM
ServoEasing servo[nservos](PCA9685_DEFAULT_ADDRESS);

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
  uint8_t cpos;

  for(uint8_t s=0; s<nservos; s++) {
    #ifdef ALTERNATE
      if(!digitalRead(controlPin[s])) cpos = !spos[2][s];
      else cpos = spos[2][s];
    #else
      cpos = digitalRead(controlPin[s]);
    #endif
    if( cpos != spos[2][s] ) {
      spos[2][s] = cpos;
      //servo[s].write(spos[cpos][s]);        // regular Servo command
      servo[s].startEaseTo(spos[cpos][s]);    // ServoEasing command, done behind the scenes
      lastchange = millis();
      debug("\n input:"); debug(s); debug("="); debug(spos[cpos][s]);
    }
  }
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    delay(2000);
    debug("\n\n Hello\n");
  #endif
  
  // check EEPROM, and if not intialized, intialize it
  uint8_t mn = EEPROM.read(0);                    // read magic number at location 0
  if(mn != 0xA5) {                                // if not present, write it and initialize EEPROM
    debug("\nInitialize eeprom\n");
    EEPROM.write(0, 0xA5);
    for(uint8_t i=0; i<nservos; i++) {            // .. then for each servo, set its positions
      spos[0][i] = (MAXPOSN-MINPOSN)/2 + 5;       // first position half-way plus a bit    
      spos[1][i] = (MAXPOSN-MINPOSN)/2 - 5;       // second position
      spos[2][i] = 0;                             // active position: 0-1
    }
    EEPROM.put(1,spos);                           // and save spos to the EEPROM location 1
  }

  // Retreive the saved positions from EEPROM
  EEPROM.get(1,spos);                      // retrieve servo positions and current position
  debug("\nRetrieve data from eeprom\n");
  for(int i=0; i<nservos; i++) {
    debug("    "); debug(i); debug(":"); debug(spos[0][i]);debug(","); debug(spos[1][i]);debug(","); debug(spos[2][i]); debug("\n");
  }
  debug("\n\n");
  
  // Attach the servos
  for(int i=0; i<nservos; i++) {           // and setup and update the servos
    servo[i].attach(i);
    servo[i].setEasingType(EASE_CUBIC_IN_OUT);     // user choice, see the ServoEasing library
    servo[i].startEaseTo( spos[spos[2][i]][i] );   // set the servos to their saved positions
  }
  
  setSpeedForAllServos(SERVOSPEED);  // common to all servos
  pinMode(up, INPUT_PULLUP);         // setup the button pins
  pinMode(down, INPUT_PULLUP);
  pinMode(nextservo, INPUT_PULLUP);
  pinMode(nextposn, INPUT_PULLUP);
  for(int i=0; i<nservos; i++) {     // setup the servo pins
    pinMode(controlPin[i], INPUT_PULLUP);
  }
}

void loop() {
  uint8_t delayt = 200;              // default delay for button presses
  // Button processing
  while(digitalRead(up)==0)          // test 'up'-button
  {
    lastchange = millis();
    if( spos[activePos][activeServo] < MAXPOSN ) spos[activePos][activeServo]++;
    servo[activeServo].write(spos[activePos][activeServo]);
    debug("\n Up ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" pos[][]="); debug(spos[activePos][activeServo]);
    delayt -= 10;               // speed up the movement with longer press of the button
    if(delayt<50) delayt=50;
    delay(delayt);
  }
  while(digitalRead(down)==0)        // "Down" button
  {
    lastchange = millis();            // prolonged press gives accelerating movement
    if( spos[activePos][activeServo] > MINPOSN ) spos[activePos][activeServo]--;
    servo[activeServo].write(spos[activePos][activeServo]);
    debug("\n Down ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" pos[][]="); debug(spos[activePos][activeServo]);
    debug("\n");
    delayt -= 10;                    // decrease the period between movements
    if(delayt<50) delayt=50;         // .. but not too fast!
    delay(delayt);                   // delay between each movement
  }
  if(digitalRead(nextservo)==0)          // "Next Servo" button
  {
    lastchange = millis();
    while( digitalRead(nextservo)==0 ) {   // if button pressed for 2 seconds, revert to the first servo
      if( (millis()-lastchange)>2000 ) activeServo = nservos;
    }
    if( ++activeServo >= nservos) activeServo = 0;
    activePos = spos[2][activeServo];
    servo[activeServo].startEaseTo(  spos[activePos][activeServo] );
    debug("\n next servo ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" pos[][]="); debug(spos[activePos][activeServo]);
    debug("\n");
    delay(200);
  }
  if(digitalRead(nextposn)==0)          // "Next Position" button
  {
    lastchange = millis();
    if( ++activePos > 1) activePos = 0;
    servo[activeServo].startEaseTo( spos[activePos][activeServo] );
    debug("\n next pos ");
    debug(" servo="); debug(activeServo);
    debug(" pos="); debug(activePos);
    debug(" pos[][]="); debug(spos[activePos][activeServo]);
    debug("\n");
    delay(200);
  }
  // update EEPROM after servo mods
  if(lastchange!=0 && millis()>(lastchange+20000))     // save the positions of the servos if last change was 20s ago
  {
    EEPROM.put(1,spos);                               // save settings
    lastchange = 0;
    debug("\n Saved\n");
    for(int i=0; i<nservos; i++) {
      debug("   "); debug(i); debug(":"); debug(spos[0][i]);debug(","); debug(spos[1][i]);debug(","); debug(spos[2][i]); debug("\n");
    }
    spos[2][activeServo] = 2;  // resync inputs by setting position to impossible value, so it gets updated by input scan
    activeServo = 0;
  }
 
  // process all servo inputs
  inputScan();
 
  // delay loop
  delay(100);
}
