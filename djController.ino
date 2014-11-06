#include <Bounce.h>  // Bounce library makes button change detection easy
#include <Encoder.h>
const int channel = 1;
const int debounce = 5;
const int buttonVel = 127;
const int numButtons = 28;
const int analogChange = 2;

int muxChannel[16][4]={ 
  {0,0,0,0}, //channel 0
  {1,0,0,0}, //channel 1
  {0,1,0,0}, //channel 2
  {1,1,0,0}, //channel 3
  {0,0,1,0}, //channel 4
  {1,0,1,0}, //channel 5
  {0,1,1,0}, //channel 6
  {1,1,1,0}, //channel 7
  {0,0,0,1}, //channel 8
  {1,0,0,1}, //channel 9
  {0,1,0,1}, //channel 10
  {1,1,0,1}, //channel 11
  {0,0,1,1}, //channel 12
  {1,0,1,1}, //channel 13
  {0,1,1,1}, //channel 14
  {1,1,1,1}  //channel 15
};

int controlPin[] = {14, 15, 16, 17}; //set contol pins in array

int buttonPins[numButtons] = {
  //top
  4, 5, 7, 8, 9, 10,
  //red
  11, 12, 13, 20, 21,
  //bottom
  39, 38, 27, 26, 22, 23, 24, 25,
  //encoder 40 41 not working
  40, 41, 42,
  //Flick
  43, 44,
  28, 29,
  30, 31
};

Encoder encs[3] = {Encoder(0, 1), Encoder(2, 3), Encoder(18, 19)};
long encPos[3] = {0, 0, 0};

Bounce buttons[numButtons] = {Bounce(4, debounce), Bounce(5, debounce), Bounce(7, debounce), Bounce(8, debounce), Bounce(9, debounce), Bounce(10, debounce),
                      Bounce(11, debounce), Bounce(12, debounce), Bounce(13, debounce), Bounce(20, debounce), Bounce(21, debounce),
                      Bounce(39, debounce), Bounce(38, debounce), Bounce(27, debounce), Bounce(26, debounce), Bounce(22, debounce), Bounce(23, debounce), Bounce(24, debounce), Bounce(25, debounce), 
                      Bounce(40, debounce), Bounce(41, debounce), Bounce(42, debounce),
                      Bounce(43, debounce), Bounce(44, debounce),
                      Bounce(28, debounce), Bounce(29, debounce),
                      Bounce(30, debounce), Bounce(31, debounce) };

int SIG_pin = 45; //read pin

int potVal[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int readMux(int channel){  
  //loop through the four control pins
  for(int i = 0; i < 4; i ++){ 
    //turn on/off the appropriate control pins according to what channel we are trying to read 
    digitalWrite(controlPin[i], muxChannel[channel][i]); 
  }
  //read the value of the pin
  int val = analogRead(SIG_pin); 
  //return the value
  return val; 
}

void setup() {
  Serial.begin(31250);//open serail port @ midi speed
  for(int i=0;i<numButtons;i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  pinMode(SIG_pin,INPUT);
  //set our control pins to output
  
  for (int i = 0;i<4;i++) {
    pinMode(controlPin[i],OUTPUT);
    digitalWrite(controlPin[i],LOW);
  }
}



void loop() {
  for(int i=0;i<numButtons;i++) {
    buttons[i].update();
    // Note On messages when each button is pressed
    if (buttons[i].fallingEdge()) {
      usbMIDI.sendNoteOn(i+60, buttonVel, channel);  // 62 = D4
    }
    // Note Off messages when each button is released
    if (buttons[i].risingEdge()) {
      usbMIDI.sendNoteOff(i+60, 0, channel);  // 60 = C4
    }
  }
  //encoders
  for(int i = 0;i<3;i++) {
    long newVal;
    newVal = encs[i].read();
    if (newVal > encPos[i]) {
      usbMIDI.sendNoteOn(i,127,127); 
       encPos[i] = newVal; 
    }
    if (newVal < encPos[i]) {
      usbMIDI.sendNoteOn(i,1,127); 
       encPos[i] = newVal; 
    }
  }
  
  //Analog
  for(int i = 0;i<16;i++) {
    int tempa = readMux(i);
    if (tempa <= analogChange) {
      tempa = 0;
    }
    if (tempa >= 1023 - analogChange) {
      tempa = 1023;
    }
    if (abs(tempa - potVal[i]) > analogChange || (tempa == 0 && potVal[i] != 0)) {
      potVal[i] = tempa;
      usbMIDI.sendPitchBend(map(tempa,0,1023,0,16363),i); 
    }
  }
}

  // MIDI Controllers should discard incoming MIDI messages.
  //while (usbMIDI.read()) {
  //}
//}
