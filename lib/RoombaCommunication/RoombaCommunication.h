/*
  RoombaCommunication.h - Library for interfacing Roomba 500 series
  Created by: Adam Ottvar, 2020
*/

// ensure this library description is only included once
#ifndef RoombaCommunication_h
#define RoombaCommunication_h

#include "SoftwareSerial.h"

class RoombaCommunication
{
  // user-accessible "public" interface
  public:
    RoombaCommunication(int, int, int, long);
    void wakeUp(void);
    void start(void);
    void baud(int);
    void safeMode(void);
    void fullMode(void);
    void powerDown(void);
    void seekDock(void);
    void drive(int, int);
    void driveDirect(int, int);
    void drivePWM(int, int);
    void driveStop(void);

  // library-accessible "private" interface
  private:
    int ddPin;
    SoftwareSerial roombaSerial;
    void limitValue(int&, int, int);
};

#endif
