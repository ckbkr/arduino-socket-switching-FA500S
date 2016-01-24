/* 
 Created by Karl-Heinz Wind - karl-heinz.wind@web.de
 Copyright 2015 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
 
 Flamingo switch sample.
 
 required hardware:
 - 1x Arduino
 - 1x 433MHz Receiver
 - 1x 433Mhz Sender
 connect PIN3 to receiver-unit Data-PIN
 connect PIN4 to sender-unit Data-PIN
 
 The sketch listenes for codes from the remote control and prints em via serial.
 Every button sends 4 different codes, so don't be confused when you read different codes
 when pressing the same button several times. indeed you need only one of the four to control
 the socket.
 */

#include "FlamingoSwitch.h"

FlamingoSwitch Switch;

int RX_PIN = 0; // PIN3 rx needs to be in IRQ pin!
int TX_PIN = 4; // PIN4

int counter = 0;
int deviceCounter = 0;

uint32_t codes[4] = {
  0,0,0,0};
String bins[4] = {
  "","","",""};

void setup() 
{
  Serial.begin(9600);

  Switch.enableReceive(RX_PIN);  // Receiver on interrupt 0 => that is pin #2
}

void loop() 
{
  // detect code from remote control
  if (Switch.available())
  {
    output(Switch.getReceivedValue(), Switch.getReceivedBitlength(), Switch.getReceivedDelay(), Switch.getReceivedRawdata());
    Switch.resetAvailable();
  }
}

void output(unsigned long code, unsigned int length, unsigned int delay, unsigned int* raw) 
{
  counter++;
  if (code != 0)
  {
    char* b = code2bin(code, length);
    String bin(b);

    if (deviceCounter == 0)
    {
      codes[0] = code;
      bins[0] = bin;
      deviceCounter ++;
    }
    boolean found = false;
    for (int i = 0; i < deviceCounter; i++)
    {
      if (code == codes[i])
      {
        found = true;
      }
    }
    if (!found)
    {
      codes[deviceCounter] = code;
      bins[deviceCounter] = bin;      
      deviceCounter ++;
    }
    Serial.println();    
    Serial.print(" Bin: ");
    Serial.println(b);
    Serial.println();
    for (int i = 0; i < deviceCounter; i++)
    {
      Serial.print("Counter: ");
      Serial.print(i);
      Serial.print(" Code: ");
      Serial.print(codes[i]);
      Serial.print(" Code: 0x");
      Serial.print(codes[i], HEX);
      Serial.print(" Bin: ");
      Serial.println(bins[i]);
    }
  }
  else
  {
    Serial.print("Unknown encoding.");
  }
  Serial.println();
  Serial.println();
}

static char * code2bin(unsigned long code, unsigned int bitLength)
{
  static char bin[64];
  unsigned int i = 0;

  while (code > 0)
  {
    bin[32 + i++] = (code & 1 > 0) ? '1' : '0';
    code = code >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++)
  {
    if (j >= bitLength - i)
    {
      bin[j] = bin[31 + i - (j - (bitLength - i))];
    }
    else
    {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';

  return bin;
}


