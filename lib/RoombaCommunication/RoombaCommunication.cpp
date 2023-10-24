/*
  RoombaCommunication.h - Library for interfacing Roomba 500 series
  Created by: Adam Ottvar, 2020
*/
#include <Arduino.h>
#include "RoombaCommunication.h"
#include <SoftwareSerial.h>

// Constructor /////////////////////////////////////////////////////////////////
RoombaCommunication::RoombaCommunication(int rxPin, int txPin, int devDetectPin, long baudRate)
{
  // initialize this instance's variables
  int ddPin = devDetectPin;
  SoftwareSerial roombaSerial(rxPin,txPin);
  roombaSerial.begin(baudRate);
  pinMode(devDetectPin, OUTPUT);
}

// Private Methods /////////////////////////////////////////////////////////////
void RoombaCommunication::limitValue(int &value, int min, int max)
{
  value = (value < min ? min : value > max ? max : value);
}

// Public Methods //////////////////////////////////////////////////////////////
void RoombaCommunication::wakeUp(void)
{
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
}

void RoombaCommunication::start(void)
{
  roombaSerial.write(128);  //Start
}

void RoombaCommunication::baud(int baudCode)
{
  /*
   * Baud Code | Baud Rate
   * 0            300
   * 1            600
   * 2            1200
   * 3            2400
   * 4            4800
   * 5            9600
   * 6            14400
   * 7            19200
   * 8            28800
   * 9            38400
   * 10           57600
   * 11           115200
   */
  roombaSerial.write(129);
  roombaSerial.write(baudCode);
  delay(100);
}

void RoombaCommunication::safeMode(void)
{
  roombaSerial.write(131);
}

void RoombaCommunication::fullMode(void)
{
  roombaSerial.write(132);
}

void RoombaCommunication::powerDown(void)
{
  roombaSerial.write(133);
}

void RoombaCommunication::seekDock(void)
{
  roombaSerial.write(143);
}

void RoombaCommunication::drive(int velocity, int radius)
{
  roombaSerial.write(137);
  roombaSerial.write(velocity >> 8);
  roombaSerial.write(velocity);
  roombaSerial.write(radius >> 8);
  roombaSerial.write(radius);
}

void RoombaCommunication::driveDirect(int rightVel, int leftVel)
{
  limitValue(rightVel, -500, 500);
  limitValue(leftVel, -500, 500);
  roombaSerial.write(145);
  roombaSerial.write(rightVel >> 8);
  roombaSerial.write(rightVel);
  roombaSerial.write(leftVel >> 8);
  roombaSerial.write(leftVel);
}

void RoombaCommunication::drivePWM(int rightPWM, int leftPWM)
{
  limitValue(rightPWM, -255, 255);
  limitValue(leftPWM, -255, 255);
  roombaSerial.write(146);
  roombaSerial.write(rightPWM >> 8);
  roombaSerial.write(rightPWM);
  roombaSerial.write(leftPWM >> 8);
  roombaSerial.write(leftPWM);
}

void RoombaCommunication::driveStop(void)
{
  driveDirect(0,0);
}
