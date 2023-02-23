/*  Pico Sysex MIDI Programmer
 *  
 *  LINK A INSTRUCTABLE
 *  
 *  by Barito 2023 (last update 17 feb. 2023)
 */

#include <LiquidCrystal.h>
#include <MIDI.h>
#include <SoftwareSerial.h>

#define SYNTHESIZERS 4

//initiAlize LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);//LiquidCrystal(rs, enable, d4, d5, d6, d7)

//MIDI_CREATE_DEFAULT_INSTANCE();
using Transport = MIDI_NAMESPACE::SerialMIDI<SoftwareSerial>;
int rxMIDIPin = 18;
int txMIDIPin = 19;
SoftwareSerial mySerial(rxMIDIPin, txMIDIPin);
Transport serialMIDI(mySerial);
MIDI_NAMESPACE::MidiInterface<Transport> MIDI((Transport&)serialMIDI);

//COUNTERS
int MIDI_CHANNEL;
int parCatCount;
int PARAMETER;
//int chCount;
int synthCount;
bool catMODE;//0-cathegory; 1-subcathegory
bool MODE;//0-default; 1-menu

int maxSub;
int minSub;

const char* synthName[SYNTHESIZERS] = {"KORG EX/DW8000", "ROLAND a-JUNO", "OBERHEIM MATRIX6", "Generic CC"};

const int parameters[SYNTHESIZERS] = {46, 36, 52, 127};//DW8000, Alpha JUNO, MATRIX 6, Generic CC
const int cathegories[SYNTHESIZERS] = {8, 6, 9, 0};//DW8000, Alpha JUNO, MATRIX 6, Generic CC

//Arrays to store each parameter actual value
int DW8K_parVal[46]; //DW8000
int aJU_parVal[36]; // a-JUNO
int MX6_parVal[52]; // MATRIX 6

//---------------- DW8000 -------------------------------------
const char* DW8K_CatName[8] = {"OSC1", 
                               "OSC2",
                               "VCF",
                               "VCA",
                               "LFO",
                               "WHEEL", 
                               "D. DELAY",
                               "OTHERS"};

const char* DW8K_SubName[46] = {"OCTAVE", "WAVE", "MIX LEVEL", "BEND DESTINATION","BEND MODE", "BEND TIME", "BEND INTENSITY", 
                                "OCTAVE", "WAVE", "MIX LEVEL", "TRANSPOSE", "DETUNE", "NOISE LEVEL", 
                                "CUTOFF", "RESONANCE", "KEY FOLLOW", "ENV MODE", "ENVELOPE", "ENV ATTACK", "ENV DECAY","ENV BREAKPOINT", "ENV SLOPE", "ENV SUSTAIN", "ENV RELEASE","ENV DYNAMICS", 
                                "ENV ATTACK", "ENV DECAY", "ENV BREAKPOINT","ENV SLOPE", "ENV SUSTAIN", "ENV RELEASE", "ENV DYNAMICS",
                                "WAVE", "RATE", "DELAY", "->OSC", "->VCF", 
                                "OSC BEND", "VCF BEND", 
                                "TIME", "TIME FINE", "FEEDBACK", "FREQUENCY", "INTENSITY","LEVEL",
                                "PORTAMENTO"};

//define the number of sub elements for each cathegory
const int DW8K_parIndex [8] = {7, 6, 12, 7, 5, 2, 6, 1}; //total: 46 elements

//define the paramenter number (DW8000)
const int DW8K_parNum[46] =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 15,
                       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
                       
//define the parameters range (DW8000)
const int DW8K_valRange[46] = {3, 15, 31, 3, 1, 31, 31, 3, 15, 31, 7, 7, 31, 63,
                       31, 3, 1, 31, 31, 31, 31, 31, 31, 31, 7, 31, 31, 31, 31, 31,
                       31, 7, 3, 31, 31, 31, 31, 15, 1, 7, 15, 15, 31, 31, 15, 31};

//-------------------- a-JUNO --------------------------------------------
const char* aJU_CatName[6] = {"DCO",
                              "VCF",
                              "VCA",
                              "ENVELOPE",
                              "LFO",
                              "CHORUS"};

const char* aJU_SubName[36] = {"OCTAVE", "PULSE", "SAWTOOTH", "SUB", "SUB LEVEL", "NOISE LEVEL", "<-LFO", "<-ENV", "ENV MODE", "AFTERTOUCH", "<-PWM", "PWM RATE", "BEND RANGE",
                               "LPF CUTOFF",  "RESONANCE", "<-LFO", "<-ENV", "ENV MODE", "KEY FOLLOW", "AFTERTOUCH", "HPF CUTOFF", 
                               "LEVEL", "AFTERTOUCH", "ENV MODE",
                               "ATTACK TIME", "ATTACK LEVEL", "BRAKE TIME", "BRAKE LEVEL", "DECAY", "SUSTAIN", "RELEASE", "KEY FOLLOW",
                               "RATE", "DELAY TIME",
                               "ON/OFF", "RATE"};

//define the number of sub elements for each cathegory
const int aJU_parIndex[6] = {13, 8, 3, 8, 2, 2}; //total: 36 elements

//define the paramenter number (a-JUNO)
const int aJU_parNum[36] =  {6, 3, 4, 5, 7, 8, 11, 12, 0, 13, 14, 15, 35,
                             16, 17, 18, 19, 1, 20, 21, 9,
                             22, 23,  2,
                             26, 27, 28, 29, 30, 31, 32, 33,
                             24, 25,
                             10, 34};
                       
//define the parameters range (a-JUNO)
const int aJU_valRange[36] = {3, 3, 7, 7, 3, 3, 127, 127, 3, 15, 127, 127, 12,
                              127, 127, 127, 127, 3, 15, 15, 3,
                              127, 15,  3,
                              127, 127, 127, 127, 127, 127, 127, 15,
                              127, 127, 
                              1, 127};        

//-------------------- MATRIX 6 --------------------------------------------
const char* MX6_CatName[9] = {"OSC1",
                              "OSC2",
                              "VCF",
                              "VCA",
                              "RAMPS",
                              "ENVELOPE1",
                              "ENVELOPE2",
                              "LFO1",
                              "LFO2"};

const char* MX6_subName[52] = {"PITCH", "PITCH<-LFO1", "SYNC", "PWM", "PWM <-LFO2", "WAVESHAPE", "WAVEFORM", 
                               "PITCH", "PITCH<-LFO1", "DETUNE", "PWM", "PWM <-LFO2", "WAVESHAPE", "WAVEFORM", "DCO MIX", 
                               "VCF CUTOFF", "<-ENV1", "RESONANCE", "FM",
                               "VCA1 VOLUME", "VCA1 VELOCITY", "VCA2 ENV",
                               "RAMP1 RATE", "RAMP1 TRIGGER", "RAMP2 RATE", "RAMP2 TRIGGER", 
                               "DELAY", "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "AMPLITUDE", "TRIG MODE", "MODE", 
                               "DELAY", "ATTACK", "DECAY", "SUSTAIN", "RELEASE", "AMPLITUDE", "TRIG MODE", "MODE", 
                               "SPEED", "WAVEFORM", "RETRIG POINT", "AMPLITUDE", "TRIG MODE", 
                               "SPEED", "WAVEFORM", "RETRIG POINT", "AMPLITUDE", "TRIG MODE"};

//define the number of sub elements for each cathegory
const int MX6_parIndex[9] = {7, 8, 4, 3, 4, 8, 8, 5, 5}; //total: 52 elements

//define the paramenter number (MATRIX 6)
const int MX6_parNum[52] =  {0, 1, 2, 3, 4, 5, 6, 
                             10, 11, 12, 13, 14, 15, 16, 20, 
                             21, 22, 24, 30,
                             27, 28, 29,
                             40, 41, 42, 43, 
                             50, 51, 52, 53, 54, 55, 57, 58, 
                             60, 61, 62, 63, 64, 65, 67, 68, 
                             80, 82, 83, 84, 86, 
                             90, 92, 93, 94, 96};
                       
//define the parameters range (MATRIX 6)
const int MX6_valRange[52] = {63, 127, 3, 63, 127, 63, 3, 
                              63, 127, 63, 63, 127, 63, 7, 63, 
                              127, 127, 63, 63,
                              63, 63, 63,
                              63, 3, 63, 3,
                              63, 63, 63, 63, 63, 63, 7, 3, 
                              63, 63, 63, 63, 63, 63, 3, 3, 
                              63, 7, 31, 63, 3, 
                              63, 7, 31, 63, 3};

//-------------------------GENERIC CONTROL CHANGE ---------------------
byte CCval[127];

//---------------------------------------------------------------------

//buttons
const int btn1Pin = 1;
const int btn2Pin = 4;
const int btn3Pin = 13;

bool btn1State, btn2State, btn3State;

//optical inputs
const int optA1Pin = 3;
const int optB1Pin = 2;
const int optA2Pin = 6;
const int optB2Pin = 5;
int optA1_state;
int optB1_state;
int optA2_state;
int optB2_state;
int last1Encoded = 0;
int last2Encoded = 0;
int encoder1Value = 0;
int encoder2Value = 0;
int sum1;
int sum2;
int prevSum2;
//int debT = 0;//debounce time

void setup() {
  pinMode(btn1Pin, INPUT_PULLUP);
  pinMode(btn2Pin, INPUT_PULLUP);
  pinMode(btn3Pin, INPUT_PULLUP);
  pinMode(optA1Pin, INPUT_PULLUP);
  pinMode(optB1Pin, INPUT_PULLUP);
  pinMode(optA2Pin, INPUT_PULLUP);
  pinMode(optB2Pin, INPUT_PULLUP);

  optA1_state = digitalRead(optA1Pin);
  optB1_state = digitalRead(optB1Pin);
  optA2_state = digitalRead(optA2Pin);
  optB2_state = digitalRead(optB2Pin);

  attachInterrupt(optA1Pin, Update1Encoder, CHANGE);
  attachInterrupt(optB1Pin, Update1Encoder, CHANGE);
  attachInterrupt(optA2Pin, Update2Encoder, CHANGE);
  attachInterrupt(optB2Pin, Update2Encoder, CHANGE);

  parCatCount = 0;
  PARAMETER = 0;
  synthCount = 0; // 0 - DW8000; 1 - a-jUNO; 2 - MATRIX 6; 3 generic CC
  //chCount = 0;
  MIDI_CHANNEL = 1;
  catMODE = 0;
  MODE = 1; //0 - PATCHER; 1 - MENU
  
  MIDI.setHandleNoteOn(Handle_Note_On);
  MIDI.setHandleNoteOff(Handle_Note_Off);
  MIDI.setHandleControlChange(Handle_CC);
  MIDI.setHandlePitchBend(Handle_PB);
  
  MIDI.begin(MIDI_CHANNEL_OMNI); //start MIDI and listen to ALL MIDI channels
  MIDI.turnThruOff();

  lcd.begin(16, 2);
  SplashScreen(3000);
  DispUpdate();
  //Serial.begin(9600); //DEBUG
}

void loop() {
  MIDI.read(); //calls MIDI.handles
  BtnCounter();
  //delay(1);
}

void BtnCounter(){
  //BUTTON 1 (ENCODER 1)
  if(digitalRead(btn1Pin) != btn1State){//on state change
    btn1State = !btn1State;
    if(btn1State == LOW){
      if(MODE == 0){//MAIN MENU
        catMODE = !catMODE;//0-cathegory; 1-subcathegory
      }
      else{//sub MENU
        SynthSelect(1);
      }        
    }
    delay(100);//cheap debounce
  }
  //BUTTON 2 (ENCODER 2)
  if(digitalRead(btn2Pin) != btn2State){//on state change
    btn2State = !btn2State;
    if(btn2State == LOW){
      if(MODE == 0){//MAIN MENU
        ParameterValueChange(10);
      }
      else{
        MIDI_ChannelChange(+1);
      }
      DispUpdate();
    }
    delay(100);//cheap debounce
  }
  //BUTTON 3 (MIDDLE BUTTON)
  if(digitalRead(btn3Pin) != btn3State){//on state change
    btn3State = !btn3State;
    if(btn3State == LOW){
      MODE = !MODE;//MENU
      DispUpdate();
    }
    delay(100);//cheap debounce
  }
}

void SplashScreen(int dT){
  lcd.setCursor(0, 0);
  lcd.print("SysEx Pico");
  lcd.setCursor(6, 1);
  lcd.print("Programmer");
  delay(dT);
  lcd.clear();
}

void DispUpdate(){
  lcd.clear();
  if(MODE == 0){//MAIN MENU
    switch (synthCount){
      case 0: //DW8000
        //parameter cathegory name
        lcd.setCursor(0, 0);
        lcd.print(DW8K_CatName[parCatCount]);
        //parameter sub cathegory name
        lcd.setCursor(0, 1);
        lcd.print(DW8K_SubName[PARAMETER]);
        //parameter value
        lcd.setCursor(13, 0);
        lcd.print(DW8K_parVal[PARAMETER]);
       break;
       case 1: //a-JUNO
        //parameter cathegory name
        lcd.setCursor(0, 0);
        lcd.print(aJU_CatName[parCatCount]);
        //parameter sub cathegory name
        lcd.setCursor(0, 1);
        lcd.print(aJU_SubName[PARAMETER]);
        //parameter value
        lcd.setCursor(13, 0);
        lcd.print(aJU_parVal[PARAMETER]);
       break;
       case 2: //MATRIX 6
        //parameter cathegory name
        lcd.setCursor(0, 0);
        lcd.print(MX6_CatName[parCatCount]);
        //parameter sub cathegory name
        lcd.setCursor(0, 1);
        lcd.print(MX6_subName[PARAMETER]);
        //parameter value
        lcd.setCursor(13, 0);
        lcd.print(MX6_parVal[PARAMETER]);
       break;
       default: //GENERIC CC
        //parameter name
        lcd.setCursor(0, 0);
        lcd.print("Control Change");
        //parameter value
        lcd.setCursor(13, 1);
        lcd.print(CCval[PARAMETER]);
       break;
     }
  }
  else{//SUB MENU
    //MIDI channel
    lcd.setCursor(0, 0);
    lcd.print("MIDI CHANNEL");
    lcd.setCursor(14, 0);
    lcd.print(MIDI_CHANNEL);
    //SYNTH name
    lcd.setCursor(0, 1);
    lcd.print(synthName[synthCount]);
  }
}

void Update1Encoder(){
  optA1_state = digitalRead(optA1Pin); //MSB = most significant bit
  optB1_state = digitalRead(optB1Pin); //LSB = least significant bit
  
  encoder1Value = (optA1_state << 1) | optB1_state; //converting the 2 pin value to single number
  sum1  = (last1Encoded << 2) | encoder1Value; //adding it to the previous encoded value
  if (sum1 == 0b1000){
    if(MODE == 0){//MAIN MENU
      ParameterNameChange(1);
    }
    else{//SUB MENU
      MIDI_ChannelChange(1);
    }
  }
  else if (sum1 == 0b0010){
    if(MODE == 0){//MAIN MENU
      ParameterNameChange(-1);
    }
    else{//SUB MENU
      MIDI_ChannelChange(-1);
    }
  }
  last1Encoded = encoder1Value; //store this value for next time
}

void Update2Encoder(){
  optA2_state = digitalRead(optA2Pin); //MSB = most significant bit
  optB2_state = digitalRead(optB2Pin); //LSB = least significant bit
  
  encoder2Value = (optA2_state << 1) | optB2_state; //converting the 2 pin value to single number
  sum2  = (last2Encoded << 2) | encoder2Value; //adding it to the previous encoded value
  if (sum2 == 0b1000){
    if(MODE == 0){//MAIN MENU
      ParameterValueChange(+1);
    }
    else{
      SynthSelect(1);
    }
  }
  else if (sum2 == 0b0010){ 
    if(MODE == 0){//MAIN MENU
      ParameterValueChange(-1);
    }
    else{
      SynthSelect(-1);
    }
  }
  last2Encoded = encoder2Value; //store this value for next time
}

void MIDI_ChannelChange(int delta){
  MIDI_CHANNEL = MIDI_CHANNEL + delta;
  if(MIDI_CHANNEL >16){
    MIDI_CHANNEL = 1;
  }
  else if(MIDI_CHANNEL<=0){
    MIDI_CHANNEL = 16;
  }
  DispUpdate();
}
      
void ParameterValueChange(int delta){
  switch (synthCount){
            case 0://DW8000
              DW8K_parVal[PARAMETER] = DW8K_parVal[PARAMETER]+delta;
              if (DW8K_parVal[PARAMETER]>DW8K_valRange[PARAMETER]){
                DW8K_parVal[PARAMETER] = DW8K_valRange[PARAMETER];
              }
              else if (DW8K_parVal[PARAMETER]<0){
                DW8K_parVal[PARAMETER] = 0;
              }
              SendSysExDW8000(DW8K_parNum[PARAMETER], DW8K_parVal[PARAMETER]);//SEND SYSEX
            break;
            case 1://a-JUNO
              aJU_parVal[PARAMETER] = aJU_parVal[PARAMETER]+delta;
              if (aJU_parVal[PARAMETER]>aJU_valRange[PARAMETER]){
                aJU_parVal[PARAMETER] = aJU_valRange[PARAMETER];
              }
              else if (aJU_parVal[PARAMETER]<0){
                aJU_parVal[PARAMETER] = 0;
              }
              SendSysExJuno(aJU_parNum[PARAMETER], aJU_parVal[PARAMETER]);//SEND SYSEX
            break;
            case 2://MATRIX 6
              MX6_parVal[PARAMETER] = MX6_parVal[PARAMETER]+delta;
              if (MX6_parVal[PARAMETER]>MX6_valRange[PARAMETER]){
                MX6_parVal[PARAMETER] = MX6_valRange[PARAMETER];
              }
              else if (MX6_parVal[PARAMETER]<0){
                MX6_parVal[PARAMETER] = 0;
              }
              SendSysExMX6(MX6_parNum[PARAMETER], MX6_parVal[PARAMETER]);//SEND SYSEX
            break;
            default://generic CC
              CCval[PARAMETER] = CCval[PARAMETER] + delta;
              if(CCval[PARAMETER]>127){
                CCval[PARAMETER] = 127;
              }
              else if (CCval[PARAMETER]<0) {
                CCval[PARAMETER] = 0;
              }
              MIDI.sendControlChange(PARAMETER+1, CCval[PARAMETER], MIDI_CHANNEL);
            break;
   }
   DispUpdate();
}

void SynthSelect(int delta){
  synthCount = synthCount + delta;
  if(synthCount>=SYNTHESIZERS){
    synthCount = 0;
  }
  else if(synthCount<0){
    synthCount = SYNTHESIZERS-1;
  }
  if(synthCount == 2){//MATRIX 6
    quickEditMX6();  //sets Matrix 6/6R in quick edit mode
  }
  Init_Patch();
  ParBoundariesUpdate();
  DispUpdate();
}

//parameter cathegory and sub cathegory names change
void ParameterNameChange(int delta){
  if (catMODE == 0){//we are changing cathegory name
    parCatCount = parCatCount + delta;
    if(delta>0){
      if(parCatCount >= cathegories[synthCount]){
        parCatCount = cathegories[synthCount]-1; //no cycle
      }
    }
    if(delta<0){
      if(parCatCount <=0){
        parCatCount = 0;
      }
    }
    ParBoundariesUpdate();
  }
  else {//if(catMODE==1) we are changing sub cathegory
    PARAMETER = PARAMETER + delta;
      if(PARAMETER >= maxSub){
        //PARAMETER = minSub; //cycle
        PARAMETER = maxSub; //no cycle 
      }
      else if(PARAMETER <= minSub){
        //PARAMETER = maxSub;//cycle
        PARAMETER = minSub;    //no cycle 
      }
  }
  DispUpdate();
  //Serial.println(parCatCount);
}

void ParBoundariesUpdate(){
  maxSub = 0;
  minSub = 0;
  switch (synthCount){
    case 0: //DW8000
      for (int a = 0; a <= parCatCount; a++){
        maxSub = maxSub + DW8K_parIndex[a];
      }
      minSub = maxSub - DW8K_parIndex[parCatCount];
    break;
    case 1: //a-JUNO
      for (int a = 0; a <= parCatCount; a++){
        maxSub = maxSub + aJU_parIndex[a];
      }
       minSub = maxSub - aJU_parIndex[parCatCount];
    break;
    case 2: //MATRIX 6
      for (int a = 0; a <= parCatCount; a++){
        maxSub = maxSub + MX6_parIndex[a];
      }
       minSub = maxSub - MX6_parIndex[parCatCount];
    break;
  }
  maxSub = maxSub - 1;
  PARAMETER = minSub;
  //Serial.println(minSub);
  //Serial.println(maxSub);
}

void Init_Patch(){
  parCatCount = 0;//initialize parameter cathegory count
  PARAMETER = 0;//initialize ABSOLUTE parameter count
  switch (synthCount){
    case 0: //DW8000
      for (int a; a < parameters[0]; a++){
        DW8K_parVal[a] = 0;
      }
    break;
    case 1: //a-JUNO
      for (int a; a < parameters[1]; a++){
        aJU_parVal[a] = 0;
      }
    break;
    case 2: //MATRIX 6
      for (int a; a < parameters[2]; a++){
        MX6_parVal[a] = 0;
      }
    break;
    default: //generic CC
      for (int a; a < parameters[3]; a++){
        CCval[a] = 0;
      }
   }
}

void SendSysExDW8000(int par, int value) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x42);                     // Korg ID#
    mySerial.write(0x30+MIDI_CHANNEL-1);      // MIDI basic channel
    mySerial.write(0x03);                     // Device ID (DW8000/EX8000)
    mySerial.write(0x41);                     // message: parameter change
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}

void SendSysExJuno(int par, int value) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x41);                     // Roland ID#
    mySerial.write(0x36);                     // operation code = individual tone parameters
    mySerial.write(MIDI_CHANNEL-1);           // Unit # = MIDI basic channel
    mySerial.write(0x23);                     // Format type (JU-1, JU-2)
    mySerial.write(0x20);                     // Level # = 1 (?)
    mySerial.write(0x01);                     // Group # (?)
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}

void quickEditMX6() {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x10);                     // Oberheim ID#
    mySerial.write(0x06);                     // Device ID: 0x02 for M12 or Xpander, 0x06 for M6/6R
    mySerial.write(0x05);                     // opcode: quick mode
    mySerial.write(0xF7);                     // SysEx end
}

void SendSysExMX6(int par, int value) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x10);                     // Oberheim ID#
    mySerial.write(0x06);                     // Device ID: 0x02 for M12 or Xpander, 0x06 for M6/6R
    mySerial.write(0x06);                     // opcode: change parameter
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}

void SendSysExMAX(int par, int value) {     //WORKING BUT UNUSED
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x01);                     // SCI ID#
    mySerial.write(0x08);                     // Device ID: 0x05 for SixTrak, 0x08 for MAX.
    mySerial.write(0xB0+MIDI_CHANNEL-1);      // opcode: change parameter. By default MAX MIDI channel = 3!
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}

void editEnableMAX(){
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x01);                     // SCI ID#
    mySerial.write(0x7E);                     // ENABLE
    mySerial.write(0xF7);                     // SysEx end 
}

/*void SendSysExToneJuno(int value, int toneName) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x41);                     // Roland ID#
    mySerial.write(0x35);                     // operation code = all parameters
    mySerial.write(MIDI_BASIC_CHANNEL);       // Unit # = MIDI basic channel
    mySerial.write(0x23);                     // Format type (JU-1, JU-2)
    mySerial.write(0x20);                     // Level # = 1 (?)
    mySerial.write(0x01);                     // Group # (?)
    mySerial.write(value);                    // tone value
    mySerial.write(toneName);                 // tone name
    mySerial.write(0xF7);                     // SysEx end
}*/

/*void SendSysExJ106(int par, int value) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x41);                     // Roland ID#
    mySerial.write(0x32);                     // operation code = individual tone parameters
    mySerial.write(MIDI_BASIC_CHANNEL);       // Unit # = MIDI basic channel
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}*/

void SendSysExJX8P(int par, int value) {
    mySerial.write(0xF0);                     // SysEx start
    mySerial.write(0x41);                     // Roland ID#
    mySerial.write(0x36);                     // operation code = individual tone parameters
    mySerial.write(MIDI_CHANNEL-1);           // Unit # = MIDI basic channel
    mySerial.write(0x21);                     // Format type (JX8P)
    mySerial.write(0x20);                     // Level # = 1 (?)
    mySerial.write(0x01);                     // Group # (?)
    mySerial.write(par);                      // SysEx parameter
    mySerial.write(value);                    // parameter value
    mySerial.write(0xF7);                     // SysEx end
}

void Handle_Note_On(byte mchannel, byte pitch, byte velocity){
  if(mchannel == MIDI_CHANNEL) {
    MIDI.sendNoteOn(pitch, velocity, mchannel); //Echoes MIDI note on messages
  } 
}

void Handle_Note_Off(byte mchannel, byte pitch, byte velocity){ 
  if(mchannel == MIDI_CHANNEL) {
    MIDI.sendNoteOff(pitch, velocity, mchannel); //Echoes MIDI note off messages
  }
}

void Handle_CC(byte mchannel, byte number, byte value){
  if(mchannel == MIDI_CHANNEL) {
    MIDI.sendControlChange(number, value, mchannel); //Echoes MIDI control change messages
  } 
}

void Handle_PB(byte mchannel, int bend){
  if(mchannel == MIDI_CHANNEL) {
    MIDI.sendPitchBend(bend, mchannel); //Echoes MIDI pitch bend messages
  } 
}
