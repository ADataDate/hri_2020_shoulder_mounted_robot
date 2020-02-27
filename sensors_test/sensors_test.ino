
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "SparkFunBME280.h"

#define GSR A0

MAX30105 particleSensor;
BME280 mySensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
long sum = 0;

int thresholds = 0;
int sensorValue;

void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing...");
  Serial.println("Reading basic values from BME280");
  delay(1000);
  Wire.begin();

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1); //Freeze
  }

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  for (int i = 0; i < 500; i++)
  {
    sensorValue = analogRead(GSR);
    sum += sensorValue;
    delay(5);
  }
  thresholds = sum / 500;
  Serial.print("thresholds =");
  Serial.println(thresholds);
}

void loop()
{
  Serial.println();
  Serial.print("Humidity: ");
  Serial.println(mySensor.readFloatHumidity(), 0);
  Serial.print(" Temp: ");
  //Serial.print(mySensor.readTempC(), 2);
  Serial.println(mySensor.readTempF(), 2);
  Serial.println();

  int GSRtemp;
  sensorValue = analogRead(GSR);
  Serial.print("sensorValue=");
  Serial.println(sensorValue);
  GSRtemp = thresholds - sensorValue;
  if (abs(GSRtemp) > 60)
  {
    sensorValue = analogRead(GSR);
    GSRtemp = thresholds - sensorValue;
    if (abs(GSRtemp) > 60) {
      Serial.println("Emotion Changes Detected!");
    }
  }
    else {
      Serial.println("No Change in Emotions");
    }


    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
      Serial.println("We sensed a beat!");
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

    Serial.print("IR= ");
    Serial.println(irValue);
    Serial.print("BPM= ");
    Serial.println(beatsPerMinute);
    Serial.print("Avg BPM= ");
    Serial.println(beatAvg);

    if (irValue < 50000) {
      Serial.println(" No finger?");
    }
  delay (2000);
}
