#include <VirtualWire.h>
#include <Wire.h>
#include <RCSwitch.h>
#include "functionpointer.h"

char buffer[40] = {0};

volatile unsigned short val1 = 15;
volatile unsigned short val2 = 30;

#define RECVBUFLENGTH 15



RCSwitch mySwitch = RCSwitch();
uint8_t signatureStart[3] = { 'a', 'b', 'c' };
int matchStart = 0;
uint8_t signatureStop[3] = { 'd', 'e', 'f' };
int matchStop = 0;
uint8_t signaturePlant[3] = { 'g', 'h', 'i' };
int matchPlant = 0;



void setup() {
  Serial.begin(9600);
  Serial.println("booting");
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(3);
  mySwitch.setProtocol(4);
  mySwitch.setRepeatTransmit(1);
  mySwitch.setPulseLength(300);

  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(7);
  vw_setup(1000);  // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
}

void sendStartA(){
  mySwitch.send("0010011001100101111001010110");
  mySwitch.send("0010011001110010010010011110");
  mySwitch.send("0010011000100110111001110110");
  mySwitch.send("0010010001100100101001100010");
}
void sendStartB(){
  mySwitch.send("0010011010011011000011000101");
  mySwitch.send("0010010001110010110010010001");
  mySwitch.send("0010010010000111011110011001");
  mySwitch.send("0010010011100101011011111001");
}
void sendStartC(){
  mySwitch.send("1110011100011000101101011001");
  mySwitch.send("1110010100010111010101110001");
  mySwitch.send("1110010111111001100111111001");
  mySwitch.send("1110010011110001110100010001");
}
void sendStartD(){
  mySwitch.send("0110010000111011000111101010");
  mySwitch.send("0110011011110010100000011010");
  mySwitch.send("0110010010011011010011000010");
  mySwitch.send("0110010000001001111011000110");
}

void sendStopA(){
  mySwitch.send("0010011011011010110111000110");
  mySwitch.send("0010011100010000011001111010");
  mySwitch.send("0010011111101101101001011010");
  mySwitch.send("0010011011000110001110001110");
}
void sendStopB(){
  mySwitch.send("0010010011001100010111001101");
  mySwitch.send("0010011101001000001111101001");
  mySwitch.send("0010011011101010111000011101");
  mySwitch.send("0010011001111010011110010101");
}
void sendStopC(){
  mySwitch.send("1110011001000010011100010101");
  mySwitch.send("1110010110101000011100011101");
  mySwitch.send("1110010101100111011011110101");
  mySwitch.send("1110011000010010010001001101");
}
void sendStopD(){
  mySwitch.send("0110010001000110101110111010");
  mySwitch.send("0110010000000100110100110010");
  mySwitch.send("0110011100100100001011001010");
  mySwitch.send("0110010110000001101010110110");
}

void startSocket(int number){
  Serial.print("Socket turned on: ");
  Serial.println(number);
    
  switch(number){
    case 0:
      sendStartA();
      break;
    case 1:
      sendStartB();
      break;
    case 2:
      sendStartC();
      break;
    case 3:
      sendStartD();
      break;
  }
}

void stopSocket(int number){
  Serial.print("Socket turned off: ");
  Serial.println(number);
  
  switch(number){
    case 0:
      sendStopA();
      break;
    case 1:
      sendStopB();
      break;
    case 2:
      sendStopC();
      break;
    case 3:
      sendStopD();
      break;
  }
}

void sendPlantData(int plant){
  char buffer[10] = {0};
  String s;
  switch(plant){
    case 0:
      s = String(val1,DEC);
      s.toCharArray(buffer,10);
      Serial.println(buffer);
      break;

    case 1:
      s = String(val2,DEC);
      s.toCharArray(buffer,10);
      Serial.println(buffer);
      break;
  }
}


void matchSerialInput(unsigned int incomingByte, uint8_t * signature, int * match, p_func function){
  if( signature[*match] == incomingByte ){
    (*match)++;
    return;
  }if( *match == 3 ){
    incomingByte = incomingByte - 48; // Substract ASCII encoding
    function(incomingByte);
    *match = 0;
  }else{
    *match = 0;
    if( signature[*match] == incomingByte ){
      (*match)++;
    }
  }  
}

void loop() {
    uint8_t recvbuf[RECVBUFLENGTH] = { 0 };
    uint8_t buflen = RECVBUFLENGTH;
    if (vw_get_message(recvbuf, &buflen)) // Non-blocking
    {
      if( recvbuf[0] = 0x34 && recvbuf[1] == 0x24 ){
        val1 = *((unsigned short*)(recvbuf+2));
        val2 = *((unsigned short*)(recvbuf+4));
        Serial.print("Pflanze 1: ");
        Serial.print(val1);
        Serial.print(" Pflanze 2: ");
        Serial.print(val2);
        Serial.print("\n");
      }
    }
    
    if (Serial.available()){
      unsigned int incomingByte = Serial.read();
      matchSerialInput(incomingByte, signatureStart, &matchStart, startSocket);
      matchSerialInput(incomingByte, signatureStop, &matchStop, stopSocket);
      matchSerialInput(incomingByte, signaturePlant, &matchPlant, sendPlantData);
    }
}

