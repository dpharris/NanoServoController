# NanoServoController
This Servo Controller was originally designed for use with the Nano, but should work unchanged on the Uno, etc.  It has also been adapted to the Pico.  

# AVR_ServoCOntroller_Native.ino
THis sketch is written for the Nano and AVR series.  It has up to 8 input lines that controlup to 8 servos.  These inputs can be absolute (Active Low) or alternating, allowing use of toggles or push-buttons, or being connected to a accessory board.  

The inputs can be connected to SPDT slide or toggle switched with the centre to the board pins D0-D7 and the other two terminals connected to groaund and 5V.  Or, 8 pushbuttons connected to ground and to D0-D7.  In this case one has to uncomment one line in tne sketch: "#define ALTERNATE" to allow the pushbuttons to choose the two servo psotitions alternately.  

The outputs D8-D13, A4 and A5 are connected to eight servos.  

In addition there are four control lines, operated by push-buttons, used to program the endpoints of the servos:
 - *Next servo* - choose the next servo
 - *Next position* - choose the next servo position
 - *Up* -- move the chosen servo one way
 - *Down* -- move the chosen servo the other way
Changes to the servo positions will be stored in EEPROM after 20 seconds of inaction.  

# Extensions
The sketch has been modified to:
 - AVR using use an external PCA9685, see **AVR_ServoController_PCA9685.ino**.  This allows up to 16 servos.  
 - Pico using native (PIO) servo-driver to allow 8 servos.  (This uses all PIO, so be aware.). **Pico_ServoCOntroller_Native.ino**.
 - Pico using a PCA9685.  This allows 16 servos.  **Pico_ServoController_PCA9685.ino**.

# Compiling / Using

Copy one of the sketches into a new sketch in the Arduino IDE.  CHoose the correct oprocessor.  You will need to use Library Manager to download the ServoEasing library.  

# Using SPDT switches

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216859313-27cc5578-d9dc-42cc-be6b-92511c8e8620.png">

# Using Nano Mother board and SPDT switches

Inexpensive motherboards can be used to make life easier, since they have a power supply and servo connectors, for example: https://www.aliexpress.com/item/1005002054420577.html

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216858950-60f5566a-00ab-4bf4-8a68-7a956a337cb7.png">

# Using buttons
This will require the "#define ALTERNATE" line to be uncommented in the sketch.

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216859615-54c74fef-9944-403e-8a4f-5e01b2bcea6c.png">

# Using Nano Motherboard and PCA9685 PWM daughter board

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/222863756-b29e5ded-535e-4c69-9af6-d1678387f781.png">

# Using Pico with Native PWM

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/224518628-2f1e0c43-77bd-4670-8e1d-eb11628b9a20.png">

# Using Pico Motherboard and PCA9685 PWM daughter board

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/224518336-2e8d97ec-abfd-414a-a0ed-9e28cdc2224f.png">


# Using with LCC Tower

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216859938-4b44d31d-98fe-43fe-96b8-5b31501ca698.png">

# Real World Setup
<img width="539" alt="image" src="https://user-images.githubusercontent.com/538698/217139424-1f8224ca-cb6d-472a-86f0-e7b92857b0ff.png">

## Setting Servo Positions
https://youtu.be/eLYbbh9UrcI

## Operating the Servos from LCC
This set up has an LCC-Loconet-USB, and two LCC Towers.  I have programmed a pushbutton to send alternating eventids.  The same Tower consumes these, and moves a servo to one endpoint and then to the other endpoint.  

https://youtu.be/-KqmzUoACQ0
