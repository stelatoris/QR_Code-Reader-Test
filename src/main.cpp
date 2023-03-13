#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 16 // Green
#define TX_PIN 17 // Blue

SoftwareSerial mySerial(RX_PIN, TX_PIN);
//-----------------------------

enum ReceiveState
{
  IDLE,
  RECEIVING
};
ReceiveState rec_state = IDLE;

#define CMD_BAUD_RATE_9600 "7E 00 08 02 00 2A 39 01 A7 EA"
#define CMD_BAUD_RATE_1200 "7E 00 08 02 00 2A C4 09 A7 EA"

#define CMD_SAVE_SETTLEMENTS "7E 00 09 01 00 00 00 DE C8"
#define CMD_FIND_BAUD_RATE "7E 00 07 01 00 2A 02 D8 0F"
#define CMD_LED_ON "7E 00 08 01 00 00 D6 AB CD"
#define CMD_TRIGGER_MODE "7E 00 08 01 00 02 01 AB CD"
#define CMD_FULL_AREA "7E 00 08 01 00 2C 02 AB CD"
#define CMD_ALLOW_CODE39 "7E 00 08 01 00 36 01 AB CD"
#define CMD_CODE39_MIN_LENGTH "7E 00 08 01 00 37 00 AB CD"
#define CMD_CODE39_MAX_LENGTH "7E 00 08 01 00 38 FF AB CD"
#define CMD_CUT_OUT_START_PART "7E 00 08 01 00 B0 01 AB CD"
#define CMD_CUT_OUT_M_BYTES_START "7E 00 08 01 00 B1 FF AB CD"
#define CMD_OUTPUT_END_PART "7E 00 08 01 00 B0 02 AB CD"
#define CMD_CUT_OUT_N_BYTES_END "7E 00 08 01 00 B2 FF AB CD"
#define CMD_OUTPUT_CENTER_PART "7E 00 08 01 00 B0 03 AB CD"
#define CMD_CUT_OUT_M_BYTES_START_3 "7E 00 08 01 00 B1 03 AB CD"
#define CMD_CUT_OUT_N_BYTES_END_2 "7E 00 08 01 00 B2 02 AB CD"
#define CMD_OUT1_LOW_LEVEL "7E 00 08 01 00 E7 00 AB CD"
#define CMD_OUT1_HIGH_LEVEL "7E 00 08 01 00 E7 01 AB CD"

// Define the possible baud rates that the module can return
#define GM811_BAUD_RATE_1200 0x05
#define GM811_BAUD_RATE_4800 0x06
#define GM811_BAUD_RATE_9600 0x07
#define GM811_BAUD_RATE_14400 0x08
#define GM811_BAUD_RATE_19200 0x09
#define GM811_BAUD_RATE_38400 0x0A
#define GM811_BAUD_RATE_57600 0x0B

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

void findGM811BaudRate(SoftwareSerial &mySerial)
{
  // Send the find baud rate command
  // mySerial.write(CMD_FIND_BAUD_RATE);
  // Send the find baud rate command
  byte cmdBaudRate9600[] = {0x7E, 0x00, 0x08, 0x02, 0x00, 0x2A, 0x39, 0x01, 0xA7, 0xEA};
  mySerial.write(cmdBaudRate9600, sizeof(cmdBaudRate9600));
  delay(1000);
  // mySerial.write((byte)0x7E);
  // mySerial.write((byte)0x00);
  // mySerial.write((byte)0x07);
  // mySerial.write((byte)0x01);
  // mySerial.write((byte)0x00);
  // mySerial.write((byte)0x2A);
  // mySerial.write((byte)0x02);
  // mySerial.write((byte)0xD8);
  // mySerial.write((byte)0x0F);

  // Wait for the response from the module
  delay(1000);

  // Wait for the response from the module
  delay(50);

  // Check if there is any data available on the scanner interface
  if (mySerial.available())
  {
    // Read the data from the scanner interface
    String response = mySerial.readStringUntil('\n');
    Serial.println("Response: " + response);

    // Extract the baud rate code from the response
    String baudRateCode = response.substring(10, 14);

    // Convert the baud rate code to an integer
    int baudRate = 0;
    for (int i = 0; i < 4; i++)
    {
      baudRate = baudRate * 256 + (int)baudRateCode.charAt(i);
    }

    // Print the baud rate to the serial monitor
    Serial.print("Baud rate: ");
    Serial.println(baudRate);
  }
  else
    Serial.println("No scanner.available..");
}

void sendCommandPacket(String cmdPacket)
{
  mySerial.write(cmdPacket.c_str());
  delay(100);

  String response = "";
  while (mySerial.available())
  {
    char c = mySerial.read();
    response += c;
    delay(10);
  }

  Serial.println(response);
}

//----------------------------------

bool setGM811BaudRate(SoftwareSerial &mySerial, int baudRate)
{
  byte cmd[] = {0x7E, 0x00, 0x08, 0x02, 0x00, 0x2A, 0x39, 0x01, 0xA7, 0xEA};

  switch (baudRate)
  {
  case 1200:
    cmd[4] = 0x00;
    cmd[5] = 0x02;
    cmd[6] = 0xC4;
    cmd[7] = 0x09;
    break;
  case 4800:
    cmd[4] = 0x00;
    cmd[5] = 0x02;
    cmd[6] = 0x71;
    cmd[7] = 0x02;
    break;
  case 9600:
    cmd[4] = 0x00;
    cmd[5] = 0x2A;
    cmd[6] = 0x39;
    cmd[7] = 0x01;
    break;
  case 14400:
    cmd[4] = 0x00;
    cmd[5] = 0x02;
    cmd[6] = 0xD0;
    cmd[7] = 0x00;
    break;
  case 19200:
    cmd[4] = 0x00;
    cmd[5] = 0x2A;
    cmd[6] = 0x9C;
    cmd[7] = 0x00;
    break;
  case 38400:
    cmd[4] = 0x00;
    cmd[5] = 0x2C;
    cmd[6] = 0x4E;
    cmd[7] = 0x00;
    break;
  case 57600:
    cmd[4] = 0x00;
    cmd[5] = 0x34;
    cmd[6] = 0x00;
    cmd[7] = 0x00;
    break;
  default:
    return false;
  }

  mySerial.write(cmd, sizeof(cmd));

  // Wait for the response from the GM811 QR code reader
  delay(1000);

  if (mySerial.available())
  {
    // Read the response from the GM811 QR code reader

    Serial.print("Response: ");
    while (mySerial.available())
    {
      Serial.print("0x");
      Serial.print(mySerial.read(), HEX);
      Serial.print(" ");
    }
    Serial.println();
    // String response = mySerial.readStringUntil('\n');
    // Serial.println("Response: " + response);

    // Parse the response and check for errors or success
    // ...

    return true;
  }
  else
  {
    Serial.println("No response received.");
    return false;
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  mySerial.begin(9600);
  while (!mySerial)
  {
    Serial.println("No mySerial.");
  }
  Serial.println("Setting Baud rate...");
  Serial.print("readParity: ");
  Serial.println(mySerial.readParity());
  setGM811BaudRate(mySerial, 9600);
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
