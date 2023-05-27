#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//Teensy Audio Library setup code
AudioSynthWaveform       waveformTriangle;                //xy=1371.7500343322754,792.2500247955322
AudioSynthWaveform       waveformSquare;                  //xy=1373.2500343322754,844.2500246763229
AudioSynthWaveform       waveformSine;                    //xy=1374.2500343322754,687.7500190734863
AudioSynthWaveform       waveformSaw;                     //xy=1377.2500343322754,737.7500190734863
AudioMixer4              waveformMixer;                   //xy=1571.2500343322754,761.7500190734863
AudioMixer4              dryMixer;                        //xy=1754.2500038146973,934.2500371932983
AudioEffectEnvelope      ADSR;                            //xy=1950.0000305175781,926.0000743865967
AudioEffectWaveshaper    effectDistortion;                //xy=2104,877
AudioMixer4              effectDistortionDryWet;          //xy=2354,962
AudioEffectBitcrusher    effectRedux;                     //xy=2505,871
AudioMixer4              effectReduxDryWet;               //xy=2676,962
AudioEffectFreeverbStereo effectReverb;                   //xy=2846,823
AudioMixer4              effectReverbDryWetRight;         //xy=3174,943
AudioMixer4              effectReverbDryWetLeft;          //xy=3175,865
AudioMixer4              panMixerLeft;                    //xy=3404.0000495910645,875.0000114440918
AudioMixer4              panMixerRight;                   //xy=3404,947
AudioOutputAnalogStereo  DACS_OUT;                        //xy=3584,870
AudioConnection          patchCord1(waveformTriangle, 0, waveformMixer, 2);
AudioConnection          patchCord2(waveformSquare, 0, waveformMixer, 3);
AudioConnection          patchCord3(waveformSine, 0, waveformMixer, 0);
AudioConnection          patchCord4(waveformSaw, 0, waveformMixer, 1);
AudioConnection          patchCord5(waveformMixer, 0, dryMixer, 0);
AudioConnection          patchCord6(dryMixer, ADSR);
AudioConnection          patchCord7(ADSR, effectDistortion);
AudioConnection          patchCord8(ADSR, 0, effectDistortionDryWet, 1);
AudioConnection          patchCord9(effectDistortion, 0, effectDistortionDryWet, 0);
AudioConnection          patchCord10(effectDistortionDryWet, effectRedux);
AudioConnection          patchCord11(effectDistortionDryWet, 0, effectReduxDryWet, 1);
AudioConnection          patchCord12(effectRedux, 0, effectReduxDryWet, 0);
AudioConnection          patchCord13(effectReduxDryWet, effectReverb);
AudioConnection          patchCord14(effectReduxDryWet, 0, effectReverbDryWetLeft, 1);
AudioConnection          patchCord15(effectReduxDryWet, 0, effectReverbDryWetRight, 1);
AudioConnection          patchCord16(effectReverb, 0, effectReverbDryWetLeft, 0);
AudioConnection          patchCord17(effectReverb, 1, effectReverbDryWetRight, 0);
AudioConnection          patchCord18(effectReverbDryWetRight, 0, panMixerRight, 0);
AudioConnection          patchCord19(effectReverbDryWetLeft, 0, panMixerLeft, 0);
AudioConnection          patchCord20(panMixerLeft, 0, DACS_OUT, 0);
AudioConnection          patchCord21(panMixerRight, 0, DACS_OUT, 1);

//initialize key variables
int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
bool keyState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
bool lastKeyState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

//initialize note variables
float c = 261.63;
float cSharp = 277.18;
float d = 293.66;
float dSharp = 311.13;
float e = 329.63;
float f = 349.43;
float fSharp = 369.99;
float g = 392.00;
float gSharp = 415.30;
float a = 440.00;
float aSharp = 466.16;
float b = 493.88;
int octaveCount = 0;

//initialize waveform morph knob
int morphKnob = A13;
int morphPosition = 0;

//initialize level knob
int levelKnob = A12;
int levelPosition = 0;
int mappedLevelPosition = 0;
float dryMixerPosition = 0.0;

//initialize pan knob
int panKnob = A16;
int panPosition = 0;
int mappedPanPosition = 0;
float panMixerPositionLeft = 0.0;
float panMixerPositionRight = 0.0;

//initialize buttons
int button1 = 33;
int button2 = 34;
bool button1State = LOW;
bool lastButton1State = LOW;
bool button2State = LOW;
bool lastButton2State = LOW;
int buttonSwitchMode = 0;

//initialize LEDs
int leds[] = {17, 16, 15, 14};

//initialize effect/ADSR knobs
int controlKnob1 = A9;
int controlKnob2 = A8;
int controlKnob3 = A7;
int controlKnob4 = A6;
int lastKnob1 = 0;
int lastKnob2 = 0;
int lastKnob3 = 0;
int lastKnob4 = 0;

//initialize ADSR
int ADSRAttack = 0;
int ADSRDecay = 0;
int ADSRSustain = 0;
int ADSRRelease = 0;
float mappedAttack = 0.0;
float mappedDecay = 0.0;
float mappedSustain = 0.0;
float mappedRelease = 0.0;

//initialize distortion
int distortionDryWetKnob = 0;
float distortionDryWetKnobMapped = 0.0;
float waveshapeArray[] = {            //waveform values for waveshaper modulator
  -0.588,
  -0.579,
  -0.549,
  -0.488,
  -0.396,
  -0.320,
  -0.228,
  -0.122,
  0,
  0.122,
  0.228,
  0.320,
  0.396,
  0.488,
  0.549,
  0.579,
  0.588,
  -0.588,
  -0.579,
  0.549,
  0.488,
  -0.396,
  -0.320,
  0.228,
  0.122,
  -0.122,
  -0.228,
  0.320,
  0.396,
  -0.488,
  -0.549,
  0.579,
  0.588,
  -0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.228,
  0.122,
  -0.122,
  -0.228,
  0.320,
  0.396,
  -0.488,
  -0.549,
  0.579,
  0.588,
  -0.9,
  0.9,
  -0.9,
  0.9,
  -0.9,
  0.9,
};

//initialize bitcrusher
int reduxBitsKnob = 0;
int reduxBitsKnobMapped = 0;
int reduxSampleRateKnob = 0;
int reduxSampleRateKnobMapped = 0;
int reduxDryWetKnob = 0;
float reduxDryWetKnobMapped = 0.0;

//initialize reverb
int reverbDryWetKnob = 0;
int reverbRoomSizeKnob = 0;
int reverbDampingKnob = 0;
float reverbDryWetKnobMapped = 0.0;
float reverbRoomSizeKnobMapped = 0.0;
float reverbDampingKnobMapped = 0.0;

//calls setup functions for different portions of the synth
void setup() {
  AudioMemory(5);
  for (int i = 0; i <= 11; i++)  {
    pinMode(keys[i], INPUT);
  }
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  setWaveform();
  setMixers();
  setEffects();
  setLED();
}

//main function
void loop() {
  checkPan();
  checkEffectsState();
  checkLevel();
  checkKeysState();
  oscOn();
}

//sets basic waveform shapes, frequencies, and amplitudes
void setWaveform()  {
  waveformSine.begin(WAVEFORM_SINE);
  waveformSaw.begin(WAVEFORM_SAWTOOTH);
  waveformSquare.begin(WAVEFORM_SQUARE);
  waveformTriangle.begin(WAVEFORM_TRIANGLE);
  waveformSine.amplitude(0.7);
  waveformSaw.amplitude(0.7);
  waveformSquare.amplitude(0.7);
  waveformTriangle.amplitude(0.7);
  waveformSine.frequency(261);
  waveformSaw.frequency(261);
  waveformSquare.frequency(261);
  waveformTriangle.frequency(261);
}

//sets default mixer gains
void setMixers()  {
  for (int i = 0; i <= 3; i++)  {
    waveformMixer.gain(i, 0.0);
  }
  dryMixer.gain(0, 1.0);
  effectReduxDryWet.gain(0, 0.0);
  effectReduxDryWet.gain(1, 1.0);
  effectDistortionDryWet.gain(0, 0.0);
  effectDistortionDryWet.gain(1, 1.0);
  effectReverbDryWetLeft.gain(0, 0.0);
  effectReverbDryWetLeft.gain(1, 1.0);
  effectReverbDryWetRight.gain(0, 0.0);
  effectReverbDryWetRight.gain(1, 1.0);
  panMixerLeft.gain(0, 1.0);
  panMixerRight.gain(0, 1.0);
}

//sets up distortion tone
void setEffects() {
  effectDistortion.shape(waveshapeArray, 65);
}

//sets up LED's for output
void setLED()  {
  for (int i = 0; i <= 3; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

//determines the output frequency based on the key pressed
void checkKeysState()  {
  for (int i = 0; i <= 11; i++)  {
    lastKeyState[i] = keyState[i];
    keyState[i] = digitalRead(keys[i]);
    if (lastKeyState[i] == LOW and keyState[i] == HIGH)  {
      checkNote(i);       //determines which note to output
      ADSR.noteOn();      //calls ADSR function
      delay(5);
      break;
    }
    else if (lastKeyState[i] == HIGH and keyState[i] == LOW) {
      ADSR.noteOff();     //ends ADSR function
      delay(5);
    }
  }
}

void checkOctaveState(bool octaveDirection) {
  if (octaveDirection == HIGH)  {
    if (octaveCount == 3) {
      octaveCount = 3;          //logs octave count
    }
    else  {                     //octave up math
      c = c * 2.00;
      cSharp = cSharp * 2.00;
      d = d * 2.00;
      dSharp = dSharp * 2.00;
      e = e * 2.00;
      f = f * 2.00;
      fSharp = fSharp * 2.00;
      g = g * 2.00;
      gSharp = gSharp * 2.00;
      a = a * 2.00;
      aSharp = aSharp * 2.00;
      b = b * 2.00;
      octaveCount = octaveCount + 1;
    }
  }
  else {
    if (octaveCount == -3) {
      octaveCount = -3;       //logs octave count
    }
    else  {                   //octave down math
      c = c / 2.00;
      cSharp = cSharp / 2.00;
      d = d / 2.00;
      dSharp = dSharp / 2.00;
      e = e / 2.00;
      f = f / 2.00;
      fSharp = fSharp / 2.00;
      g = g / 2.00;
      gSharp = gSharp / 2.00;
      a = a / 2.00;
      aSharp = aSharp / 2.00;
      b = b / 2.00;
      octaveCount = octaveCount - 1;
    }
  }
}

//determines what note to set the waveforms to (dependant on what key is pressed)
void checkNote(int i) {
  if (i == 0) {
    waveformSine.frequency(c);
    waveformSaw.frequency(c);
    waveformSquare.frequency(c);
    waveformTriangle.frequency(c);
  }
  else if (i == 1) {
    waveformSine.frequency(cSharp);
    waveformSaw.frequency(cSharp);
    waveformSquare.frequency(cSharp);
    waveformTriangle.frequency(cSharp);
  }
  else if (i == 2) {
    waveformSine.frequency(d);
    waveformSaw.frequency(d);
    waveformSquare.frequency(d);
    waveformTriangle.frequency(d);
  }
  else if (i == 3) {
    waveformSine.frequency(dSharp);
    waveformSaw.frequency(dSharp);
    waveformSquare.frequency(dSharp);
    waveformTriangle.frequency(dSharp);
  }
  else if (i == 4) {
    waveformSine.frequency(e);
    waveformSaw.frequency(e);
    waveformSquare.frequency(e);
    waveformTriangle.frequency(e);
  }
  else if (i == 5) {
    waveformSine.frequency(f);
    waveformSaw.frequency(f);
    waveformSquare.frequency(f);
    waveformTriangle.frequency(f);
  }
  else if (i == 6) {
    waveformSine.frequency(fSharp);
    waveformSaw.frequency(fSharp);
    waveformSquare.frequency(fSharp);
    waveformTriangle.frequency(fSharp);
  }
  else if (i == 7) {
    waveformSine.frequency(g);
    waveformSaw.frequency(g);
    waveformSquare.frequency(g);
    waveformTriangle.frequency(g);
  }
  else if (i == 8) {
    waveformSine.frequency(gSharp);
    waveformSaw.frequency(gSharp);
    waveformSquare.frequency(gSharp);
    waveformTriangle.frequency(gSharp);
  }
  else if (i == 9) {
    waveformSine.frequency(a);
    waveformSaw.frequency(a);
    waveformSquare.frequency(a);
    waveformTriangle.frequency(a);
  }
  else if (i == 10) {
    waveformSine.frequency(aSharp);
    waveformSaw.frequency(aSharp);
    waveformSquare.frequency(aSharp);
    waveformTriangle.frequency(aSharp);
  }
  else if (i == 11) {
    waveformSine.frequency(b);
    waveformSaw.frequency(b);
    waveformSquare.frequency(b);
    waveformTriangle.frequency(b);
  }
}

//main oscillator waveform mixer, determines waveform morphing position
void oscOn() {
  morphPosition = analogRead(morphKnob);
  if (morphPosition >= 0 && morphPosition < 341)  {                //sine -> saw
    waveformMixer.gain(0, 1.0 - (morphPosition / 341.000));
    waveformMixer.gain(1, (morphPosition / 341.000));
    waveformMixer.gain(2, 0.0);
    waveformMixer.gain(3, 0.0);
    delay(5);
  }
  else if (morphPosition >= 341 && morphPosition < 682)  {         //saw -> triangle
    waveformMixer.gain(0, 0.0);
    waveformMixer.gain(1, 1.0 - (morphPosition / 341.000) + 1.000);
    waveformMixer.gain(2, (morphPosition / 341.000) - 1.000);
    waveformMixer.gain(3, 0.0);
    delay(5);
  }
  else if (morphPosition >= 682 && morphPosition < 1022) {         //triangle -> square
    waveformMixer.gain(0, 0.0);
    waveformMixer.gain(1, 0.0);
    waveformMixer.gain(2, 1.0 - (morphPosition / 341.000) + 2.000);
    waveformMixer.gain(3, (morphPosition / 341.000) - 2.000);
    delay(5);
  }
  else if (morphPosition == 1023) {                               //ends on square wave
    waveformMixer.gain(0, 0.0);
    waveformMixer.gain(1, 0.0);
    waveformMixer.gain(2, 0.0);
    waveformMixer.gain(3, 1.0);
    delay(5);
  }
}

//determines input trim level
void checkLevel() {
  levelPosition = analogRead(levelKnob);
  mappedLevelPosition = map(levelPosition, 0, 1023, 0, 1000);
  dryMixerPosition = (mappedLevelPosition / 1000.0);
  dryMixer.gain(0, dryMixerPosition);
}

//determines pan position
void checkPan() {
  panPosition = analogRead(panKnob);
  mappedPanPosition = map(panPosition, 0, 1023, 0, 2000);
  panMixerPositionLeft = mappedPanPosition / 1000.0;
  panMixerPositionRight = (mappedPanPosition - 1000) / 1000.0;
  if (mappedPanPosition >= 0 and mappedPanPosition < 1000) {              //pan left
    panMixerLeft.gain(0, 1.0 - (panMixerPositionLeft / 2.0));
    panMixerRight.gain(0, panMixerPositionLeft / 2.0);
  }
  else if (mappedPanPosition >= 1000 and mappedPanPosition <= 2000)  {    //pan right
    panMixerLeft.gain(0, 0.5 - (panMixerPositionRight / 2.0));
    panMixerRight.gain(0, 0.5 + (panMixerPositionRight / 2.0));
  }
}

//checks button inputs to determine octave state and what effect state the synth is currently on
void checkEffectsState() {
  lastButton1State = button1State;
  button1State = digitalRead(button1);
  lastButton2State = button2State;
  button2State = digitalRead(button2);
  if ((lastButton1State == LOW and button1State == HIGH) and (lastButton2State == LOW and button2State == HIGH))  {
    buttonSwitchMode++;             //counter determines what effect to call
    if (buttonSwitchMode > 3) {     //limits range to usable values
      buttonSwitchMode = 0;
    }
  }
  if (buttonSwitchMode == 0)  {     //ADSR
    checkLED(0, HIGH);
    checkLED(1, LOW);
    checkLED(2, LOW);
    checkLED(3, LOW);
    checkADSR();
  }
  else if (buttonSwitchMode == 1) { //Distortion
    checkLED(0, LOW);
    checkLED(1, HIGH);
    checkLED(2, LOW);
    checkLED(3, LOW);
    checkDistortion();
  }
  else if (buttonSwitchMode == 2) { //Bitcrush
    checkLED(0, LOW);
    checkLED(1, LOW);
    checkLED(2, HIGH);
    checkLED(3, LOW);
    checkRedux();
  }
  else if (buttonSwitchMode == 3) { //Reverb
    checkLED(0, LOW);
    checkLED(1, LOW);
    checkLED(2, LOW);
    checkLED(3, HIGH);
    checkReverb();
  }
  if (lastButton1State == LOW and button1State == HIGH) {     //octave up
    checkOctaveState(HIGH);
  }
  if (lastButton2State == LOW and button2State == HIGH) {     //octave down
    checkOctaveState(LOW);
  }
}

//checks LED state
void checkLED(int i, bool ledState)  {
  if (ledState == HIGH) {
    digitalWrite(leds[i], HIGH);
  }
  else  {
    digitalWrite(leds[i], LOW);
  }
}

//checks ADSR values
void checkADSR()  {
  ADSRAttack = analogRead(controlKnob1);
  if (lastKnob1 != ADSRAttack) {
    mappedAttack = map(ADSRAttack, 0, 1023, 1, 1000) * 1.00;
    ADSR.attack(mappedAttack);
    lastKnob1 = ADSRAttack;
  }
  ADSRDecay = analogRead(controlKnob2);
  if (lastKnob2 != ADSRDecay) {
    mappedDecay = map(ADSRDecay, 0, 1023, 1, 1000) * 1.00;
    ADSR.decay(mappedDecay);
    lastKnob2 = ADSRDecay;
  }
  ADSRSustain = analogRead(controlKnob3);
  if (lastKnob3 != ADSRSustain) {
    mappedSustain = (map(ADSRSustain, 0, 1023, 1, 1000)) / 1000.0;
    ADSR.sustain(mappedSustain);
    lastKnob3 = ADSRSustain;
  }
  ADSRRelease = analogRead(controlKnob4);
  if (lastKnob4 != ADSRRelease) {
    mappedRelease = map(ADSRRelease, 0, 1023, 1, 1000) * 1.0;
    ADSR.release(mappedRelease);
    lastKnob4 = ADSRRelease;
  }
}

//checks distortion values
void checkDistortion()  {
  distortionDryWetKnob = analogRead(controlKnob4);
  if (lastKnob4 != distortionDryWetKnob) {
    distortionDryWetKnobMapped = map(distortionDryWetKnob, 0, 1023, 0, 999) / 1000.0;
    effectDistortionDryWet.gain(0, distortionDryWetKnobMapped);
    effectDistortionDryWet.gain(1, 1.0 - distortionDryWetKnobMapped);
    lastKnob4 = distortionDryWetKnob;
  }
}

//checks bitcrush values
void checkRedux()  {
  reduxSampleRateKnob = analogRead(controlKnob1);
  if (lastKnob1 != reduxSampleRateKnob) {
    reduxSampleRateKnobMapped = map(reduxSampleRateKnob, 0, 1023, 1, 44100);
    effectRedux.sampleRate(reduxSampleRateKnobMapped);
    lastKnob1 = reduxSampleRateKnob;
  }
  reduxBitsKnob = analogRead(controlKnob2);
  if (lastKnob2 != reduxBitsKnob) {
    reduxBitsKnobMapped = map(reduxBitsKnob, 0, 1023, 16, 1);
    effectRedux.bits(reduxBitsKnobMapped);
    lastKnob2 = reduxBitsKnob;
  }
  reduxDryWetKnob = analogRead(controlKnob4);
  if (lastKnob4 != reduxDryWetKnob)  {
    reduxDryWetKnobMapped = map(reduxDryWetKnob, 0, 1023, 0, 999) / 1000.0;
    effectReduxDryWet.gain(0, reduxDryWetKnobMapped);
    lastKnob4 = reduxDryWetKnob;
  }
}

//checks reverb values
void checkReverb()  {
  reverbRoomSizeKnob = analogRead(controlKnob1);
  if (lastKnob1 != reverbRoomSizeKnob)  {
    reverbRoomSizeKnobMapped = map(reverbRoomSizeKnob, 0, 1023, 0 , 999) / 1000.0;
    effectReverb.roomsize(reverbRoomSizeKnobMapped);
    lastKnob1 = reverbRoomSizeKnob;
  }
  reverbDampingKnob = analogRead(controlKnob2);
  if (lastKnob2 != reverbDampingKnob)  {
    reverbDampingKnobMapped = map(reverbDampingKnob, 0, 1023, 0 , 999) / 1000.0;
    effectReverb.damping(reverbDampingKnobMapped);
    lastKnob2 = reverbDampingKnob;
  }
  reverbDryWetKnob = analogRead(controlKnob4);
  if (lastKnob4 != reverbDryWetKnob) {
    reverbDryWetKnobMapped = map(reverbDryWetKnob, 0, 1023, 0, 1000) / 1000.0;
    effectReverbDryWetLeft.gain(0, reverbDryWetKnobMapped);
    effectReverbDryWetRight.gain(0, reverbDryWetKnobMapped);
    lastKnob4 = reverbDryWetKnob;
  }
}
