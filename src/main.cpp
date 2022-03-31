/*!
 * @file main.cpp
 *
 * @section intro_sec Introduction
 *
 * This is a program to read and send to TTN AP-2000 sensor's measurements via LoRaWAN.
 *
 * @section author Author
 *
 * Written by The Last Resort.
 */


#include <Arduino.h>

#include "LowPower.h"
#include "RTCZero.h"
#include <SDI12.h>
#include <MKRWAN.h>
#include "ap2000.hpp"

#define RESET() digitalWrite(RESET_PIN, LOW)

constexpr uint16_t PORT_SERIE = 115200;
constexpr uint8_t DATA_PIN = 12;
constexpr uint8_t WAKE_UP_PIN = 6;
constexpr uint8_t RESET_PIN = 12;


LoRaModem modem;
Ap2000Sensor sensor(DATA_PIN, 0);
const String appEui = "";
const String appKey = "";

RTCZero rtc;

void alarm() 
{
  return;
}

void setup()
{
  Serial.begin(PORT_SERIE);

  // digitalWrite(10, HIGH);    // Power consumption test
  // pinMode(10, OUTPUT);

  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  rtc.begin();
  sensor.SensorBegin();

  if (!modem.begin(EU868)) // Setup LoRa
  { 
    #ifdef _DEBUG
      Serial.println("Failed to start module");
    #endif
    rtc.setDate(0,0,0);
    rtc.setTime(0,0,0);
    rtc.setAlarmTime(0,2,0);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.attachInterrupt(alarm);
    LowPower.standby();
    RESET();
  }

  Serial.println(modem.version());
  Serial.println(modem.deviceEUI());
  int connected = modem.joinOTAA(appEui, appKey);

  if (!connected)
  {
    #ifdef _DEBUG
      Serial.println("A problem has occured while joining the TTN app");
    #endif
    rtc.setDate(0,0,0);
    rtc.setTime(0,0,0);
    rtc.setAlarmTime(0,2,0);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.attachInterrupt(alarm);
    LowPower.standby();
    RESET();
  }
  
  pinMode(WAKE_UP_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(WAKE_UP_PIN, HIGH); // Wake up the black box
  
  //delay(40000);                   // Data won't be ready and stable until 40s

  #ifdef _DEBUG
    sensor.DispSensorInfos(Serial); // Debug test
  #endif

  SensorMeasurements *sm = new SensorMeasurements; // Get measurments
  sensor.RequestMeasurementsAndWaitUntilReady();
  sensor.GetSensorMeasurements(sm, 16, false);


  float tmp = sm->temperature;
  float oxy = sm->dissolvedOxygenSaturation;
  float sal = sm->salinity;
  float tur = sm->turbidity;
  float ph = sm->ph;
  delete sm;

  uint16_t tTmp = tmp * 100; // Data formatting for sending through LoRa
  uint16_t tOxy = oxy * 100;
  uint16_t tSal = sal * 100;
  uint16_t tTur = tur * 100;
  uint16_t tPh = ph * 100;

  #ifdef _DEBUG
    Serial.println("temperature: " + String(tmp) + "°C"); // Debug output
    Serial.println("oxygene: " + String(oxy) + "%");
    Serial.println("salinite: " + String(sal) + "g/L");
    Serial.println("turbitité: " + String(tur) + "FNU");
    Serial.println("ph: " + String(ph));
    Serial.println("\n___________________________________\n");
  #endif

  byte payload[10]; // Payload construction
  payload[0] = highByte(tTmp);
  payload[1] = lowByte(tTmp);
  payload[2] = highByte(tOxy);
  payload[3] = lowByte(tOxy);
  payload[4] = highByte(tSal);
  payload[5] = lowByte(tSal);
  payload[6] = highByte(tTur);
  payload[7] = lowByte(tTur);
  payload[8] = highByte(tPh);
  payload[9] = lowByte(tPh);

  // modem.beginPacket(); // Sending data via LoRa
  // modem.write(payload, sizeof(payload));
  // int err = modem.endPacket(true);
  #ifdef _DEBUG
    if (err > 0)
    {
      Serial.println("Message has been sent successfully");
      Serial.println();
    }
    else
    {
      Serial.println("Error while sending the message");
    }
  #endif

  digitalWrite(WAKE_UP_PIN, LOW); // Put the blackbox back to sleep
  // Serial.println("Going to sleep");      // Power consumtion test
  // digitalWrite(10, HIGH);
  // delay(10000);
  // digitalWrite(10, LOW);

  // rtc.setDate(0,0,0);
  // rtc.setTime(0,0,0);
  // rtc.setAlarmTime(0,0,20);
  // rtc.enableAlarm(rtc.MATCH_HHMMSS);
  // rtc.attachInterrupt(alarm);
  // LowPower.standby();
  // delay(10);

  // Serial.println("woke up");
  // digitalWrite(10, HIGH);
  // delay(3000);
  // digitalWrite(10, LOW);
  // delay(3000);
}
