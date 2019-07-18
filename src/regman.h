#ifndef REGMAN_H
#define REGMAN_H

void writeRegister(byte reg, byte data);
byte readRegister(byte reg);
void setBit(byte reg, uint8_t bitNumber);
void clearBit(uint8_t reg, uint8_t bitNumber);

#endif