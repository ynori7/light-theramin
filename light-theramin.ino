#include "pitches.h"

int sensorValue;
int sensorLow = 1023; //sensor low and high are the expected light range based on the ambient lighting in the room
int sensorHigh = 0; 

int rangeLow = 0; //adjustable minimum value for the tone frequency
int rangeHigh = 0; //adjustable maximum value for the tone frequency

const int pitchDurationLow = 10; //produces a choppy, beeping tone
const int pitchDurationHigh = 120; //produces a smooth tone

int lastButtonState = LOW;
int pitchDuration = pitchDurationHigh;

const int ledPin = 13;
const int sensorPin = A0;
const int speakerPin = 12;
const int rangeLowPin = A1;
const int rangeHighPin = A2;
const int buttonPin = 2;

int notes[] = {NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5}; //fixed notes which can be played in a certain mode
const int notesLength = 5;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  //Calibrate the sensor to determine the min and max expected light amounts
  digitalWrite(ledPin, HIGH);
  Serial.println("Calibrating...");
  while(millis() < 5000) {
    sensorValue = analogRead(sensorPin);
    if(sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    if(sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  Serial.print("Calibration complete. Range: ");
  Serial.print(sensorLow);
  Serial.print(" - ");
  Serial.println(sensorHigh);
  digitalWrite(ledPin, LOW);
}

void loop() {
  //The button toggles the duration of the tone between pitchDurationLow and pitchDurationHigh
  checkToggleButton();

  //the light sensor
  sensorValue = analogRead(sensorPin);

  //potentiometer1 adjusts volume (no input from it)

  //potentiometer2 adjusts the low range of the tone
  rangeLow = analogRead(rangeLowPin);
  rangeLow = map(rangeLow, 0, 1023, 50, 1000);
  Serial.print("Range low: ");
  Serial.print(rangeLow);

  //potentiometer3 adjusts the high range of the tone
  rangeHigh = analogRead(rangeHighPin);
  rangeHigh = map(rangeHigh, 0, 1023, rangeLow, 4000);
  Serial.print("\tRange high: ");
  Serial.print(rangeHigh);

  int pitch;
  if(rangeLow == rangeHigh) {
    //if both potentiometer's are turned all the way down, switch modes to play fixed notes
    sensorValue = constrain(sensorValue, sensorLow, sensorHigh);
    int note = map(sensorValue, sensorLow, sensorHigh, 0, notesLength-1);
    Serial.print("\tNote: ");
    Serial.print(note);
    pitch = notes[note];
  } else {
    //play variable frequencies
    pitch = map(sensorValue, sensorLow, sensorHigh, rangeLow, rangeHigh);
  }

  Serial.print("\tPitch: ");
  Serial.print(pitch);
  Serial.print("\tDuration: ");
  Serial.println(pitchDuration);
  
  tone(speakerPin, pitch, pitchDuration);

  delay(10);
}

//The button toggles the duration of the tone between pitchDurationLow and pitchDurationHigh
void checkToggleButton() {
  int buttonValue = digitalRead(buttonPin);
  
  //button was released
  if(buttonValue == LOW) {
    lastButtonState = LOW;
    return;
  }

  //button is still pressed
  if(lastButtonState == HIGH) {
    return;
  }

  lastButtonState = HIGH;
  if(pitchDuration == pitchDurationHigh) {
    pitchDuration = pitchDurationLow;
  } else {
    pitchDuration = pitchDurationHigh;
  }
}
