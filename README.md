# arduino_windowshutter

I would like to share some info on a small home project I did using an Arduino Yun.  It is controlling the window shutters in my home already since 2016.

Recently upgraded the OS since it was using the old Linino to OpenWRT.  I did some cleaning up of the code recently and implemented a new idea to close the shutters automatically  on hotter days. So thought I would share it here, not saying it's perfect but maybe something can be helpful for others. 

The Arduino is in closed box, only connected to power and WIFI and normally not easily reachable, for the upgrade I had to take it out to connect with ethernet cable.  I was a bit afraid to break the unit but the upgrade went fine using run-sysupgrade. 

Also had some issues with the relays steering the shutter-motor. I had 10A ones initially, but they broke down after a few weeks. 
I bought new 30A ones and these are now running several years already without issue, I did not have to weld or add a "snubber network". (fingers crossed!) 
See also : https://forum.arduino.cc/index.php?topic=391317.0

Project info:  

2 triggers in Arduino can change the state of the window shutters.
- Mailbox messages (3 possible sources, see below) > retrieved by Mailbox.readMessage(message);
- Analog input (via manual pushbutton) >  function checkButton() (not from me, I found this code somewhere on this forum)

Mailbox messages :  Go-up / Go-down / Stop

Different sources of the Mailbox messages : 
- Crontab on Arduino (automated daily up/down) > curl-command > Mailbox (running since 2016)
- Crontab on Arduino (automated check max temperature coming day - if hot day close shutters at 11am) > Python script > Mailbox (new since 2020)
- static HTML page hosted on Arduino (Manual), bookmarked on eg. mobile device > Javascript-XMLHttpRequest > Mailbox  
- Mobile apps that can send GET requests (eg. HTTPBot)  

All code and some pictures are shared here : xxx

Some ideas but had not had the time to look further into this : 
1) Try steering the Arduino with Python pyfirmata (instead of Arduino code)  
https://realpython.com/arduino-python/#hello-world-with-arduino-and-python

2) IFTTT

Let me know what you think ! :)
