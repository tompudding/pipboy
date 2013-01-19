#include <SPI.h>
#include <Adb.h>

// Adb connection.
Connection * connection;
long lastTime;
int ledState = HIGH;         // the current state of the output pin
int buttonState[4];             // the current reading from the input pin
int rotaryState[5];
int lastButtonState[4] = {LOW,LOW,LOW};   // the previous reading from the input pin
int lastRotaryState[5] = {LOW,LOW,LOW,LOW,LOW};
int currentButton = 0;
int currentRotary = 0;
int lastval = 0;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime[4] = {0};  // the last time the output pin was toggled
long lastShiftTime = 0;
long rotaryLastDebounce[5] = {0};
long debounceDelay = 50;    // the debounce time; increase if the output flickers
long shiftDelay = 100;

 
int outports[] = {4,3,5};
int rotary_pinorder[] = {7,0,1,2,3};
int switch_pinorder[] = {4,5};
int latchPin = A4;
int dataPin = A3;
int clockPin = 6;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             

int rotaryports[] = {A0,A1,A2,A3,A4};

////// ----------------------------------------shiftIn function
///// just needs the location of the data pin and the clock pin
///// it returns a byte with each bit in the byte corresponding
///// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;
  
  digitalWrite(latchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchPin,0);

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
//we will be holding the clock pin high 8 times (0,..,7) at the
//end of each time through the for loop

//at the begining of each loop when we set the clock low, it will
//be doing the necessary low to high drop to cause the shift
//register's DataPin to change state based on the value
//of the next bit in its serial information flow.
//The register transmits the information about the pins from pin 7 to pin 0
//so that is why our function counts down
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(5);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}


void setup()
{
    int i;
  for(i=0;i<3;i++) {
    //pinMode(inports[i], INPUT);
    pinMode(outports[i], OUTPUT);
  }
  for(i=0;i<5;i++) {
    pinMode(rotaryports[i],INPUT);
  }
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);
    // Initialise serial port
    Serial.begin(57600);
  
    // Initialise the ADB subsystem.  
    ADB::init();
    lastTime = millis();
    connection = ADB::addConnection("tcp:4919", true, NULL);  
    
}



void Send(int x,int y,int z,int zz) {
  uint8_t data[24] = {x,0,0,0,y,0,0,0,z,0,0,0,zz,0,0,0};
    
        if(NULL != connection) {
            connection->write(24, (uint8_t*)&data);
            digitalWrite(13, HIGH);   // set the LED on
            delay(100);
            digitalWrite(13, LOW);
        
        }
        lastTime = millis();
}

void loop()
{
  // read the state of the switch into a local variable:
  int reading[5];
  int i;
  int oldButton = currentButton;
  int oldRotary = currentRotary;
  int val = 0;
  int a;
  
  
  val = 255-(analogRead(A5)/4);
  if(val != lastval)
    Send(9,val,0,0);
  lastval = val;
  
  a = shiftIn(dataPin, clockPin);  
  //Serial.println(a, BIN);

//white space
//Serial.println("-------------------");
//delay so all these print satements can keep up. 
//delay(500);

  
  for(i=0;i<2;i++) {

      reading[i] = (a>>switch_pinorder[i])&1 ? HIGH : LOW;

    if(reading[i] != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    if((millis() - lastDebounceTime[i]) > debounceDelay) {
      if(buttonState[i] == LOW && reading[i] == HIGH) {
        if(i < 1) {
          currentButton = (currentButton+1)%3;
          //currentButton = i;
        }
        else {
          Send(8,0,0,0);
        }
      }
      buttonState[i] = reading[i];
    }
    lastButtonState[i] = reading[i];
  }
  if(oldButton != currentButton) 
    Send(6,currentButton,currentRotary,val);
    
  
    
    
    
    for(i=0;i<5;i++) {
    
      reading[i] = (a>>rotary_pinorder[i])&1;
      if(reading[i] != lastRotaryState[i]) {
        rotaryLastDebounce[i] = millis();
      }
    
      if((millis() - rotaryLastDebounce[i]) > debounceDelay) {
        rotaryState[i] = reading[i];
        if(rotaryState[i] == 1)
          currentRotary = i;
          //digitalWrite(outports[i],rotaryState[i]);
      }
      lastRotaryState[i] = reading[i];
    }
    if(oldRotary != currentRotary) 
      Send(7,currentRotary,val,0);
 
    
 
  
    
  for(i=0;i<3;i++) {
    if(i == currentButton)
      digitalWrite(outports[i],HIGH);
    else
      digitalWrite(outports[i],LOW);
  }
  
    
    
    // Poll the ADB subsystem.
    ADB::poll();
}

