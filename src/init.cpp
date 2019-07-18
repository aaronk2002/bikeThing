#include "application.h"
#include "init.h"
#include "regman.h"

#define AccelAddress 0x1D

#define XYZ_DATA_CFG 0x0E
#define TRANSIENT_CFG 0x1D
#define TRANSIENT_SRC 0x1E
#define TRANSIENT_THS 0x1F
#define TRANSIENT_COUNT 0x20
#define CTRL_REG1 0x2A
#define CTRL_REG2 0x2B
#define CTRL_REG3 0x2C
#define CTRL_REG4 0x2D
#define CTRL_REG5 0x2E

void initCell();
void initAccel();

void initCell()
{
  Cellular.on();
  Cellular.connect();
  while(!Cellular.ready()) {}
}

void initAccel()
{
  Wire.begin();
  writeRegister(CTRL_REG1, 0x18);
  writeRegister(TRANSIENT_CFG, 0x16);
  writeRegister(TRANSIENT_THS, 0x0F);
  writeRegister(TRANSIENT_COUNT, 0x14);
  writeRegister(CTRL_REG4, 0x20);
  writeRegister(CTRL_REG5, 0x20);
  setBit(CTRL_REG1, 0);
}