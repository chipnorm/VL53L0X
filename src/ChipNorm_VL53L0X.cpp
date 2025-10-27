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

#include "ChipNorm_VL53L0X.h"

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID             0x00c0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID          0x00c2
#define VL53L0X_REG_RESULT_RANGE_STATUS                 0x0014
#define VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS            0x008a
#define VL53L0X_REG_SYSTEM_RANGE_CONFIG                 0x0009
#define VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV   0x0089
#define VL53L0X_REG_SYSRANGE_START                      0x0000
#define VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK            0x0002

#define VL53L0X_DEFAULT_MAX_LOOP  200

#define ESD_2V8

struct ChipNorm_VL53L0X::sVL53L0X_DetailedData_t{
    unsigned char i2cDevAddr;
    unsigned char originalData[16];
    uint16_t ambientCount;
    uint16_t signalCount;
    uint16_t distance;
    uint8_t status;
};


ChipNorm_VL53L0X::ChipNorm_VL53L0X(uint8_t scl, uint8_t sda, uint8_t addr)
: _sclPin(scl), _sdaPin(sda), _i2cAddr(addr)
{
    _detailedData = new sVL53L0X_DetailedData_t;
}

ChipNorm_VL53L0X::~ChipNorm_VL53L0X()
{
    delete _detailedData;
}

void ChipNorm_VL53L0X::writeData(unsigned char Reg ,unsigned char *buf,
    unsigned char Num){
   for(unsigned char i=0;i<Num;i++)
   {
       writeByteData(Reg++, buf[i]);
   }
}

void ChipNorm_VL53L0X::writeByteData(unsigned char Reg, unsigned char byte){
    Wire.beginTransmission(_detailedData->i2cDevAddr);
    Wire.write(Reg);
    Wire.write((uint8_t)byte);
    Wire.endTransmission();
}

void ChipNorm_VL53L0X::readData(unsigned char Reg, unsigned char Num){
    Wire.beginTransmission(_detailedData->i2cDevAddr);
    Wire.write((uint8_t)Reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_detailedData->i2cDevAddr, (uint8_t)Num);

    for(int i=0;i<Num;i++)
    {
        _detailedData->originalData[i] = Wire.read();
        delay(1);
    }
}

uint8_t ChipNorm_VL53L0X::readByteData(unsigned char Reg){
    uint8_t data;
    Wire.beginTransmission(_detailedData->i2cDevAddr);
    Wire.write((uint8_t)Reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_detailedData->i2cDevAddr, (uint8_t)1);
    data = Wire.read();
    return data;
}

void ChipNorm_VL53L0X::dataInit(){
    uint8_t data;
#ifdef ESD_2V8
    data = readByteData(VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV);
    data = (data & 0xFE) | 0x01;
    writeByteData(VL53L0X_REG_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, data);
#endif
    writeByteData(0x88, 0x00);
    writeByteData(0x80, 0x01);
    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    readByteData(0x91);
    writeByteData(0x91, 0x3c);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
    writeByteData(0x80, 0x00);
}

void ChipNorm_VL53L0X::setDeviceAddress(uint8_t newAddr){
    newAddr &= 0x7F;
    writeByteData(VL53L0X_REG_I2C_SLAVE_DEVICE_ADDRESS, newAddr);
    _detailedData->i2cDevAddr = newAddr;
}

void ChipNorm_VL53L0X::highPrecisionEnable(eFunctionalState NewState){
    writeByteData(VL53L0X_REG_SYSTEM_RANGE_CONFIG, (uint8_t)NewState);
}

void ChipNorm_VL53L0X::startContinuousHighPrecision(){
    writeByteData(0x80, 0x01);
    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    writeByteData(0x91, 0x3c);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
    writeByteData(0x80, 0x00);
    
    highPrecisionEnable(eENABLE); 

    writeByteData(VL53L0X_REG_SYSRANGE_START, VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK);
}

void ChipNorm_VL53L0X::stop(){
    writeByteData(VL53L0X_REG_SYSRANGE_START, 0x00);

    writeByteData(0xFF, 0x01);
    writeByteData(0x00, 0x00);
    writeByteData(0x91, 0x00);
    writeByteData(0x00, 0x01);
    writeByteData(0xFF, 0x00);
}

bool ChipNorm_VL53L0X::begin(){
    uint8_t revisionId;
    uint8_t modelId;
    
    Wire.begin(_sdaPin, _sclPin);
    
    _detailedData->i2cDevAddr = VL53L0X_DEF_I2C_ADDR;
    
    delay(1500);

    dataInit();
    setDeviceAddress(_i2cAddr);

    revisionId = readByteData(VL53L0X_REG_IDENTIFICATION_REVISION_ID);
    modelId = readByteData(VL53L0X_REG_IDENTIFICATION_MODEL_ID);
    
    if (modelId != 0xEE) {
        Serial.print("Fehler: Unerwartete Device ID: 0x"); 
        Serial.println(modelId, HEX);
        return false;
    }

    startContinuousHighPrecision();
    
    return true;
}


void ChipNorm_VL53L0X::readVL53L0X(){
    readData(VL53L0X_REG_RESULT_RANGE_STATUS, 12); 
    
    _detailedData->ambientCount = ((_detailedData->originalData[6] & 0xFF) << 8) |
                                    (_detailedData->originalData[7] & 0xFF);
    _detailedData->signalCount = ((_detailedData->originalData[8] & 0xFF) << 8) |
                                    (_detailedData->originalData[9] & 0xFF);
    _detailedData->distance = ((_detailedData->originalData[10] & 0xFF) << 8) |
                                (_detailedData->originalData[11] & 0xFF);
    _detailedData->status = ((_detailedData->originalData[0] & 0x78) >> 3);
}

float ChipNorm_VL53L0X::readDistance(){
    readVL53L0X();
    
    if (_detailedData->status >= 7 && _detailedData->status <= 11)
    {
        float distance_mm = (float)_detailedData->distance / 4.0; 
        
	distance_mm -= 50.0;
        
        if (distance_mm < 0) {
            distance_mm = 0;
        }

        _distance = _detailedData->distance;
        return distance_mm;
    } 
    else 
    {
        return NAN;
    }
}