#include <Arduino.h>
#include <driver/uart.h>
#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "GROWGM811.h"
#include "QRCode.h"

QueueHandle_t StatusMessageQueue;
//-----------------------------

enum ReceiveState
{
  IDLE,
  RECEIVING
};
ReceiveState rec_state = IDLE;

void stringToBytes(String str, byte bytes[])
{
  int strLength = str.length();
  for (int i = 0; i < strLength; i += 2)
  {
    char highNibble = str.charAt(i);
    char lowNibble = str.charAt(i + 1);
    byte highByte = highNibble > '9' ? highNibble - 'A' + 10 : highNibble - '0';
    byte lowByte = lowNibble > '9' ? lowNibble - 'A' + 10 : lowNibble - '0';
    bytes[i / 2] = (highByte << 4) | lowByte;
  }
}

//----------------------------------

#include <driver/uart.h>

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  // mySerial.begin(9600);
  // while (!mySerial)
  // {
  //   Serial.println("No mySerial.");
  // }

  // Serial.println("Setting Baud rate...");
  // Serial.print("readParity: ");
  // Serial.println(mySerial.readParity());

  // writeCommand(mySerial, setBaudRate9600, sizeof(setBaudRate9600));
  delay(1);
  UART_Setup();
  // writeCommand(mySerial, findBaudRate, sizeof(findBaudRate));
  writeCommand(UART_NUM_1, findBaudRate, sizeof(findBaudRate));
  // delay(100);
  writeCommand(UART_NUM_1, ledOn, sizeof(ledOn));
  writeCommand(UART_NUM_1, findBaudRate, sizeof(ledOn));

  // setGM811BaudRate(mySerial, 9600);
  delay(100);
  // mySerial.write(CMD_LED_ON);
  delay(100);
  // mySerial.write(CMD_TRIGGER_MODE);
  delay(100);
  // mySerial.write(CMD_FULL_AREA);
  delay(100);
  // findGM811BaudRate(mySerial);
  Serial.println("Setup Done.");
}

void loop()
{
  // if (mySerial.available()) // Check if there is Incoming Data in the Serial Buffer.
  // {
  //   while (mySerial.available()) // Keep reading Byte by Byte from the Buffer till the Buffer is empty
  //   {
  //     char input = mySerial.read(); // Read 1 Byte of data and store it in a character variable
  //     Serial.print(input);          // Print the Byte
  //     delay(10);                    // A small delay
  //   }
  //   Serial.println();
  // }
}
