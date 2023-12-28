#include <Arduino.h>
#include <stdint.h>
#include "SCMD.h"
#include "SCMD_config.h"
#include "Wire.h"
#include <BluetoothSerial.h>

#define LED_PIN 13
#define LIM_F_M 14
#define LIM_F_I 15

int limf;
int limm;

SCMD myMotorDriver;
BluetoothSerial SerialBT;

void toggleLed() {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
}

void driveMotors(int drive1, int drive2, int speed, int duration) {
    myMotorDriver.setDrive(0, drive1, speed);
    myMotorDriver.setDrive(1, drive2, speed);
    delay(duration);
    myMotorDriver.setDrive(0, 0, 0);
    myMotorDriver.setDrive(1, 0, 0);
}

void setup() {
    Serial.begin(9600);
    SerialBT.begin("ESP32_BT");
    Serial.println("Bluetooth activated");
    pinMode(LED_PIN, OUTPUT);

    Serial.println("Starting sketch.");

    myMotorDriver.settings.commInterface = I2C_MODE;
    myMotorDriver.settings.I2CAddress = 0x5D;
    myMotorDriver.settings.chipSelectPin = 10;

    while (myMotorDriver.begin() != 0xA9) {
        Serial.println("ID mismatch, trying again");
        delay(500);
    }
    Serial.println("ID matches 0xA9");

    Serial.print("Waiting for enumeration...");
    while (!myMotorDriver.ready());
    Serial.println("Done.");
    Serial.println();

    while (myMotorDriver.busy());
    myMotorDriver.enable();
}

void loop() {
    if (SerialBT.available()) {
        String command = SerialBT.readString();
        int CMD = command.toInt();
        Serial.println(CMD);

        limf = digitalRead(LIM_F_I);
        limm = digitalRead(LIM_F_M);

        Serial.println(limf);
        Serial.println(limm);

        if (CMD == 1) {
            if (limf == 0) {
                myMotorDriver.reset();
                toggleLed();
                delay(500);
                myMotorDriver.setDrive(0, 1, 150);
                delay(500);
            } else {
                myMotorDriver.reset();
                myMotorDriver.setDrive(0, 0, 160);
                Serial.println("Stop!");
                delay(500);
                myMotorDriver.setDrive(0, 0, 0);
                myMotorDriver.reset();
            }

            if (limm == 0) {
                myMotorDriver.reset();
                toggleLed();
                delay(1200);
                myMotorDriver.setDrive(1, 1, 150);
            } else {
                myMotorDriver.reset();
                myMotorDriver.setDrive(1, 0, 160);
                Serial.println("Stop!");
                delay(500);
                myMotorDriver.setDrive(0, 0, 0);
                myMotorDriver.setDrive(1, 0, 0);
                delay(1000);
                myMotorDriver.reset();
            }
        } else if (CMD == 2) {
            if (limf == 0 && limm == 0) {
                myMotorDriver.reset();
                driveMotors(1, 1, 0, 500);
                // ... other drive sequences ...
            } else {
                myMotorDriver.reset();
                toggleLed();
                // ... alternative behavior ...
            }
        } else if (CMD == 3) {
            myMotorDriver.setDrive(0, 0, 150);
            delay(400);
            myMotorDriver.setDrive(0, 0, 0);
            myMotorDriver.reset();
        } else if (CMD == 4) {
            myMotorDriver.setDrive(0, 1, 150);
            delay(400);
            myMotorDriver.setDrive(0, 0, 0);
            myMotorDriver.reset();
        } else if (CMD == 5) {
            myMotorDriver.setDrive(1, 0, 150);
            delay(400);
            myMotorDriver.setDrive(1, 0, 0);
            myMotorDriver.reset();
        } else if (CMD == 6) {
            myMotorDriver.setDrive(1, 1, 150);
            delay(400);
            myMotorDriver.setDrive(1, 0, 0);
            myMotorDriver.reset();
        }
    }
}
