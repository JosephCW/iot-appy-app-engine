/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "c:/Users/s519653/Documents/IoT/Projects/lubdub/src/lubdub.ino"
/*
 * Project lubdub
 * Description: Heart rate integration with nodejs on GCP
 * Author: Joseph Watts
 * Date: 11/12/2019
 */

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

void setup();
void loop();
#line 13 "c:/Users/s519653/Documents/IoT/Projects/lubdub/src/lubdub.ino"
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int timeLastBtnPress;

const int btn = A0;
// setup() runs once, when the device is first turned on.
void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");
  pinMode(btn, INPUT_PULLUP);
  timeLastBtnPress = Time.now();
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

// // loop() runs over and over again, as quickly as it can execute.
 void loop() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  if (!digitalRead(btn) && Time.now() > timeLastBtnPress) {
    timeLastBtnPress = Time.now();
    Particle.publish("heartRateGCP", String(beatsPerMinute), PRIVATE);
    Serial.println("Publishing to Particle Cloud");
  }
  Serial.println();
}