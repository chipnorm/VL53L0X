// FILE: ChipNorm_VL53L0X.h
// VERSION: 1.0.0
//
// ChipNorm invests time and resources providing this open code,
// please support ChipNorm by purchasing products from ChipNorm.
//
// Written by Enrique Fernandez for ChipNorm by FMH.
//
// Copyright   Copyright (c) 2025 Enrique Fernandez - ChipNorm by FMH
// See the LICENSE file for details.

#ifndef __CHIPNORM_VL53L0X_H
#define __CHIPNORM_VL53L0X_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#define VL53L0X_DEF_I2C_ADDR 0x29

class ChipNorm_VL53L0X{
private:
    uint8_t _sclPin;
    uint8_t _sdaPin;
    uint8_t _i2cAddr;

    typedef enum {eDISABLE = 0, eENABLE = !eDISABLE} eFunctionalState;

public:
    ChipNorm_VL53L0X(uint8_t scl, uint8_t sda, uint8_t addr = VL53L0X_DEF_I2C_ADDR);
    ~ChipNorm_VL53L0X();

    bool begin();
    
    float readDistance();

private:
    uint16_t _distance;
    
    void writeByteData(unsigned char Reg, unsigned char byte);
    uint8_t readByteData(unsigned char Reg);
    void writeData(unsigned char Reg ,unsigned char *buf, unsigned char Num);
    void readData(unsigned char Reg, unsigned char Num);
    void setDeviceAddress(uint8_t newAddr);
    void highPrecisionEnable(eFunctionalState NewState);
    void dataInit();
    void readVL53L0X();
    void startContinuousHighPrecision(); 
    void stop();

    struct sVL53L0X_DetailedData_t;
    sVL53L0X_DetailedData_t* _detailedData;
};

#endif