/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/home/aaron/Repos/bikeThing/src/bikeThing.ino"
/*
 * Project: bikeThing
 * Description: Smart bike tracker and theft detector
 * Author: Aaron Ky-Riesenbach
 * Date: 7-08-19
 */

#include <blynk.h>
#include <TinyGPS++.h>
#include "init.h"
#include "regman.h"
#include "blynkkey.h"

void initBlynk();
void setup();
bool parseGPS();
void resetInterrupt();
void loop();
#line 14 "/home/aaron/Repos/bikeThing/src/bikeThing.ino"
SYSTEM_MODE(MANUAL);

#define UART_TX_BUF_SIZE 20
#define GPSSerial Serial1
#define GPSEnable D6
#define AccelInterrupt D8
#define MAP_PIN V0
#define DISPLAY_PIN V1

  void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);

  const char* serviceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
  const char* rxUuid = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
  const char* txUuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

  BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
  BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, &rxCharacteristic);

  uint8_t txBuf[UART_TX_BUF_SIZE];
  size_t txLen = 0;

TinyGPSPlus GPS;

volatile uint8_t mode = 0; //0 is disarmed, 1 is armed, 2 is theft

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
  initCell();
  initBlynk();
  Blynk.run();
  for (uint8_t i = 0; i < len; i++) {
    if ((char)data[i] == '0')
    {
      mode = 0;
      Blynk.virtualWrite(DISPLAY_PIN, "DISARMED");
    }
    if ((char)data[i] == '1')
    {
      mode = 1;
      Blynk.virtualWrite(DISPLAY_PIN, "ARMED");
    }
    if ((char)data[i] == '2')
    {
      mode = 2;
      Blynk.virtualWrite(DISPLAY_PIN, "THEFT IN PROGRESS");
    }
  }
}

void initBlynk() {
  Blynk.begin(BLYNK_API_KEY, IPAddress(167, 99, 150, 124), 8080);
  while (!Blynk.connected()) {}
}

void setup() {
  Serial.begin(115200);
  initCell();
  initBlynk();
  Blynk.virtualWrite(DISPLAY_PIN, "DISARMED");
  GPSSerial.begin(9600);
  initAccel();
  Serial.println("Initialized");
  pinMode(GPSEnable, OUTPUT);
  pinMode(AccelInterrupt, INPUT);
  RGB.control(true);

    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    BleAdvertisingData data;
    data.appendServiceUUID(serviceUuid);
    data.appendLocalName("bikeThing");
    BLE.advertise(&data);
}

bool parseGPS() {
  while (GPSSerial.available() > 0)
  {
    GPS.encode(GPSSerial.read());
  }
  return (GPS.location.isValid() ? true : false);
}

void resetInterrupt() {
  writeRegister(CTRL_REG1, 0x18);
  setBit(CTRL_REG1, 0);
}

void loop() {
  if (BLE.connected()) { //BLE connection
    while (Serial.available() && txLen < UART_TX_BUF_SIZE)
    {
      txBuf[txLen++] = Serial.read();
      Serial.write(txBuf[txLen - 1]);
    }
    if (txLen > 0)
    {
      txCharacteristic.setValue(txBuf, txLen);
      txLen = 0;
    }
  }
  if (mode == 2) //theft
  {
    RGB.color(255, 0, 0);
    if(!Blynk.connected())
    {
      initBlynk();
    }
    Blynk.run();
    if (parseGPS())
    {
      Blynk.virtualWrite(MAP_PIN, 0, GPS.location.lat(), GPS.location.lng(), "Bike");
    }
  }
  else
  {
    digitalWrite(GPSEnable, HIGH); //turn off GPS module
    System.sleep(AccelInterrupt, FALLING);
    resetInterrupt();
    if (mode == 0) //disarmed
    {
      RGB.color(0, 255, 0);
      delay(15000);
    }
    if (mode == 1) //armed
    {
      RGB.color(0, 0, 255);
      delay(15000);
      if (mode != 0)//switch to theft mode if user hasn't disarmed
      {
        mode = 2;
        digitalWrite(GPSEnable, LOW); //enable GPS module
        initCell(); //initialize all required modules
        initBlynk();
        Blynk.run();
        Blynk.notify("Your bike is being stolen!"); //notify user of theft
        Blynk.virtualWrite(DISPLAY_PIN, "THEFT IN PROGRESS");
      }
    }
  }
}