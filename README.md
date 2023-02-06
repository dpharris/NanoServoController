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

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216855232-6ab23894-a9aa-4f50-a7a8-fb32ff9afed3.png">

# Using mother board and DPST switches

Inexpensive motherboards can be used to make life easier, since they have a power supply and servo connectors, for exxample: https://www.aliexpress.com/item/1005002054420577.html

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216853470-04390733-bc96-47d3-9606-a6607a1aa41e.png">

# Using buttons
This will require the "#define ALTERNATE" line to be uncommented in the sketch.

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216852630-f47145d2-90c4-4add-acea-3f3274a1a7bd.png">

# Using with LCC Tower

<img width="800" alt="image" src="https://user-images.githubusercontent.com/538698/216852902-4efff823-82a0-4a34-801e-233004028c4b.png">
