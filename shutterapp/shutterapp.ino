/*
  Project WindowShutters
  By Hannes Defloo / updated 2017 / 2020 
 */
 
#include <FileIO.h>
#include <Mailbox.h>

const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int relay1Pin = 8;
const int relay2Pin = 9;
const int highPin = 10;
const int lowPin = 11;
const int delayTime = 60000;

const int windowShutterLowToMiddle = 10000;
const int windowShutterHighToMiddle = 15000;
const int windowShutterHighToLow = 25000;

char logfile[] = "/mnt/sda1/log/shutter2.log";

//const int autoTimeShutterDown = 21;
//const int autoTimeShutterUp = 7;
boolean autoTimeShutterDownExecuted = false;
boolean autoTimeShutterUpExecuted = false;

// Variables will change:
//int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
//int lastButtonState = HIGH;   // the previous reading from the input pin

// time variables
int month = 0;
int hours = 0;
int minutes = 0;
int seconds = 0; 
unsigned long lastUpdatedTime = 0;
int refreshIntervalDuration = 30000; // every 30 secs minute
String timeString;

// Window and relays states
int windowShutterPositionState = 1; // -1 = LOW ; 0 = MIDDLE ; 1 = HIGH
int windowShutterMovingDirectionState = 0; // 0 = NO ; -1 = goingdown ; 1 = goingup
unsigned long windowShutterMovingStartTime = 0;
unsigned long windowShutterMovingDuration = 0;
int windowShutterLastMovingDirectionState = 1; // -1 = goingdown ; 1 = goingup

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(ledPin, OUTPUT); // for LED (13)
  pinMode(buttonPin, INPUT_PULLUP); // to ground (2)
  pinMode(relay1Pin, OUTPUT);  //output > relay 1 (8)
  pinMode(relay2Pin, OUTPUT);  //output > relay 2 (9)
  // relay testing
  pinMode(highPin, OUTPUT); // (10)
  pinMode(lowPin, OUTPUT) ; // (11)

  // led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
 
  // set initial states
  digitalWrite(ledPin, LOW);
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);  
  
  //relay testing
  digitalWrite(highPin, HIGH);
  digitalWrite(lowPin, LOW);  

  // starting stuff
  Bridge.begin();
  delay(1000);
  
  //wait for serial connection to become active
  Serial.begin(9600); // Initialize Serial
  //while (!Serial); // Wait until a Serial Monitor is connected.
  delay(1000);
  
  
  
  
  FileSystem.begin();
  delay(1000);

  appendLog("setup() ---- startup after FileSystem!!");

  Mailbox.begin();
  appendLog("setup() ---- startup after Mailbox!!");

}

// the loop function runs over and over again forever
void loop() {
  // set led based on button position
  buttonState = digitalRead(buttonPin);
  digitalWrite(ledPin, !buttonState);
  
   // Get button event and act accordingly
   int b = checkButton();
   if (b == 1) clickEvent();
   //if (b == 2) doubleClickEvent();
   if (b == 2) clickEvent();
   if (b == 3) holdEvent();
   if (b == 4) longHoldEvent();
     
   checkMovingDuration(); // stop moving window shutter after defined time  
  
//   checkAutoTimeTrigger(); // check if we should automatically open or close window shutter based on time

   checkMessages();
   
   
   
   setRelays(); // set relays in right position
  
   delay(10);  

   digitalWrite(LED_BUILTIN, HIGH); //led off
}

/*
// Window and relays states
int windowShutterPositionState = 1; // -1 = LOW ; 0 = MIDDLE ; 1 = HIGH
int windowShutterMovingDirectionState = 0; // 0 = NO ; -1 = goingdown ; 1 = goingup
int windowShutterMovingStartTime = 0;
int windowShutterMovingEndTime = 0;
int windowShutterLastMovingDirectionState = 1; // -1 = goingdown ; 1 = goingup

const int windowShutterLowToMiddle = 3000;
const int windowShutterHighToMiddle = 5000;
const int windowShutterHighToLow = 10000;
*/

void checkMessages(){
    String message;
    while (Mailbox.messageAvailable()){
      Mailbox.readMessage(message);
      appendLog("checkMessages() received : " + message);
      
      if (message.equals("go-up")){           
           shutterMove(1);          
       }
       else if (message.equals("go-down")){
           shutterMove(-1);
          
       }
       else if (message.equals("stop")){
           shutterMove(0);
       }
    }
}

void clickEvent() {
  appendLog("clickEvent()");
  if (windowShutterMovingDirectionState == 0){ // not moving > move
    appendLog("clickEvent() start moving");
    shutterMove(windowShutterLastMovingDirectionState * -1);
    
    //appendLog("clickEvent() windowShutterMovingStartTime = ", windowShutterMovingStartTime);
    //appendLog("clickEvent() windowShutterMovingDuration = ", windowShutterMovingDuration);
    //appendLog("clickEvent() windowShutterMovingDirectionState = ", windowShutterMovingDirectionState);
  }
  else{ // moving > stop 
    appendLog("clickEvent() stop moving");
    shutterMove(0);
    /*windowShutterLastMovingDirectionState = windowShutterMovingDirectionState;
    windowShutterMovingDirectionState = 0;*/
  }  
}

void shutterMove(int shutterDirection){
  appendLog("shutterMove() shutterDirection = ", shutterDirection);
  if (shutterDirection != 0){
    windowShutterMovingStartTime = millis();
    windowShutterMovingDuration = windowShutterHighToLow;
    windowShutterMovingDirectionState = shutterDirection;
   }
   else {
     windowShutterLastMovingDirectionState = windowShutterMovingDirectionState;
     windowShutterMovingDirectionState = 0;
   }
 }

void doubleClickEvent() {
   appendLog("doubleClickEvent()");
}

void holdEvent() {
  appendLog("holdEvent()");
}

void longHoldEvent() {
  appendLog("longHoldEvent()");
}

void setRelays(){
  switch (windowShutterMovingDirectionState) {
    
    case 0: // stop   
           digitalWrite(LED_BUILTIN, LOW); //led off
           digitalWrite(relay1Pin, LOW);   // relay 1 //  windowshutter off (default)
           delay(10);        
           digitalWrite(relay2Pin, LOW);   // relay 1 //  windowshutter goingdown (default)
           break;
    case 1: // going up 
           digitalWrite(LED_BUILTIN, HIGH); // led on          
           digitalWrite(relay2Pin, HIGH);   // relay 1 //  windowshutter goingup
           delay(10);  
           digitalWrite(relay1Pin, HIGH);   // relay 1 // windowshutter on
           break;
    case -1: // going down
           digitalWrite(LED_BUILTIN, HIGH); // led on         
           digitalWrite(relay2Pin, LOW);   // relay 1 //  windowshutter goindown 
           delay(10); 
           digitalWrite(relay1Pin, HIGH);   // relay 1 // windowshutter on
           break;
      }
}

/*  if (windowShutterMovingDirectionState != 0){
    digitalWrite(relay1Pin, LOW);   // relay 1 // windowshutter on
    
    if (windowShutterMovingDirectionState == 1){
    digitalWrite(relay2Pin, LOW);   // relay 1 //  windowshutter goingdown 
    }
    else{
      digitalWrite(relay2Pin, HIGH);   // relay 1 // windowshutter goingup (default)
    }
  }
  else {
    digitalWrite(relay1Pin, HIGH);   // relay 1 //  windowshutter off (default)
    digitalWrite(relay2Pin, HIGH);   // relay 1 //  windowshutter goingdown (default)
  } 
}*/

void checkMovingDuration(){   
  if ( (windowShutterMovingDirectionState != 0) && (millis() - windowShutterMovingStartTime > windowShutterMovingDuration)) { // if moving and overtime
    appendLog("checkEndTime() stop moving");
    
    windowShutterLastMovingDirectionState = windowShutterMovingDirectionState;  
    windowShutterMovingDirectionState = 0;
    
    appendLog("clickEvent() windowShutterMovingDirectionState = ", windowShutterMovingDirectionState);
    appendLog("clickEvent() windowShutterLastMovingDirectionState = ", windowShutterLastMovingDirectionState);
  
 }
}

/*void checkAutoTimeTrigger(){
    if ( (hours == autoTimeShutterDown) && (!autoTimeShutterDownExecuted) ){ // time to go down
      appendLog("checkAutoTimeTrigger()  time to go down");
      shutterMove(-1);
      autoTimeShutterDownExecuted = true;
      autoTimeShutterUpExecuted = false;
    }
    
    if ( (hours == autoTimeShutterUp) && (!autoTimeShutterUpExecuted) ){
      appendLog("checkAutoTimeTrigger()  time to go up");
      shutterMove(1);
      autoTimeShutterDownExecuted = false;
      autoTimeShutterUpExecuted = true;
     }      
}*/

int appendLog(String s, int i){
  String s1 = s;
  String s2 = String(i);
  s1 += s2;
  return appendLog(s1);
}

int appendLog(String s, long l){
  String s1 = s;
  String s2 = String(l);
  s1 += s2;
  return appendLog(s1);
}

int appendLog(String s, unsigned long l){
  String s1 = s;
  String s2 = String(l);
  s1 += s2;
  return appendLog(s1);
}
  
int appendLog(String s){
    String dataString;
    int result = -1;
    dataString += getTimeStamp();
    dataString += " " + s;
    
    if (FileSystem.exists(logfile)){    
      File dataFile = FileSystem.open(logfile, FILE_APPEND);
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        result = 0;
      }
    }
    
    return result;    
}

String getTimeStamp() {
  updateTimeStamp();  // get time from linux
  return timeString;
}

void updateTimeStamp(){
  if (lastUpdatedTime == 0 || millis() - lastUpdatedTime > refreshIntervalDuration){
    lastUpdatedTime = millis();    
  
    Process time;
    time.begin("date");
    time.addParameter("+%D-%T");  
    time.run(); 
    
    if(time.available()>0) {
      timeString  = time.readString();           
      
      /*int firstColon = timeString.indexOf(":");
      int secondColon= timeString.lastIndexOf(":");
      String hourString = timeString.substring(8, firstColon); 
      String minString = timeString.substring(firstColon+1, secondColon);
      String secString = timeString.substring(secondColon+1);
      String monthString = timeString.substring(0,2);
      String dayString = timeString.substring(3,5);
      String yearString = timeString.substring(6,8);
      
      month = monthString.toInt();
      hours = hourString.toInt();
      minutes = minString.toInt();
      seconds = secString.toInt();
      
      timeString = dayString + "-" + monthString + "-" + yearString + " / " + hourString + ":" + minString + ":" + secString;  
      */
    }
    else timeString = "TIME NOT AVAILABLE"; 
  }
} 


//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

// Button timing variables
int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
//int DCgap = 250;            // max ms between clicks for a double click event
int DCgap = 350;            // max ms between clicks for a double click event
int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button variables
boolean buttonVal = HIGH;   // value read from button
boolean buttonLast = HIGH;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already

int checkButton() {    
   int event = 0;
   buttonVal = digitalRead(buttonPin);
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (not longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}
