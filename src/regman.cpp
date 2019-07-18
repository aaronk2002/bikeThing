#include "application.h"
#include "regman.h"

#define AccelAddress 0x1D

void writeRegister(byte reg, byte data);
byte readRegister(byte reg);
void setBit(byte reg, uint8_t bitNumber);
void clearBit(uint8_t reg, uint8_t bitNumber);

void writeRegister(byte reg, byte data)
{
  Wire.beginTransmission(AccelAddress);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

byte readRegister(byte reg)
{
  Wire.beginTransmission(AccelAddress);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(AccelAddress, 1);
  return(Wire.read());
}

void setBit(byte reg, uint8_t bitNumber)
{
  uint8_t value = readRegister(reg);
  value |= 1 << bitNumber;
  writeRegister(reg, value);
}

void clearBit(uint8_t reg, uint8_t bitNumber)
{
  uint8_t value = readRegister(reg);
  value &= ~(1 << bitNumber);
  Wire.beginTransmission(AccelAddress);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}