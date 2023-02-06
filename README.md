# NanoServoController
A servo controller that uses a Nano, and has 8 channels of servos controlled by 8 inputs.  In addition, two servo endpoints can be set with 4 additional inputs.  

The inputs can be connected to DPST slide or toggle switched with the centre to the board pins D0-D7 and the other two terminals connected to groaund and 5V.  Or, 8 pushbuttons connected to ground and to D0-D7.  In this case one has to uncomment one line in tne sketch: "#define ALTERNATE" to allow the pushbuttons to choose the two servo psotitions alternately.  

The outputs D8-D13, A4 and A5 are connected to eight servos.  

There are four more buttons defined, connected to A0-3, to change the servos endpoints:
  A0 - Next servo - choose the next servo
  A1 - Next position - choose the next servo position
  A2 - Up -- move the chosen servo one way
  A3 - Down -- move the chosen servo the other way
  Changes to the servo positions will be stored in EEPROM after 20 seconds of inaction.  
  
It is also possible to connect this to model raliroad accessories, such as RRCirkits LCC Tower.  

# Using DPST switches

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216859313-27cc5578-d9dc-42cc-be6b-92511c8e8620.png">

# Using mother board and DPST switches

Inexpensive motherboards can be used to make life easier, since they have a power supply and servo connectors, for example: https://www.aliexpress.com/item/1005002054420577.html

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216858950-60f5566a-00ab-4bf4-8a68-7a956a337cb7.png">

# Using buttons
This will require the "#define ALTERNATE" line to be uncommented in the sketch.

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216852630-f47145d2-90c4-4add-acea-3f3274a1a7bd.png">

# Using with LCC Tower

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216858348-4461f7a8-7fe8-469c-8d54-a1ba425543e3.png">
