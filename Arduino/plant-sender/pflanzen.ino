
#include <VirtualWire.h>
#define READING_SIZE 128
short readings1[READING_SIZE] = { 0 };
short readings2[READING_SIZE] = { 0 };

short readingpos = 0;

short median(short n, short x[]) {
    short temp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<n-1; i++) {
        for(j=i+1; j<n; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }

    if(n%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[n/2] + x[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[n/2];
    }
}

void setup() {
  Serial.begin(9600);
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(12);
  vw_setup(1000);// speed of data transfer Kbps
}

void loop() {
  if( readingpos != READING_SIZE ){
    readings1[readingpos] = analogRead(A1);
    readings2[readingpos] = analogRead(A0);
    readingpos++;
    delay(70);
  }else{
    // Sends every 12 minuts
    readingpos = 0;
    char buffer[6] = { 0 };
    buffer[0] = 0x34; // Sender signature
    buffer[1] = 0x24;
    short * val1 = (short*)(buffer+2);
    short * val2 = (short*)(buffer+4);
    *val1 = median(READING_SIZE,readings1);
    *val2 = median(READING_SIZE,readings2);
    *val1 = millis() % 1000;
    *val2 = 1024;
    if( *val1 >= 0 && *val1 <= 1024 && *val2 >= 0 && *val2 <= 1024 ){
    Serial.print("Pflanze 1: ");
    Serial.print(*val1);
    Serial.print(" Pflanze 2: ");
    Serial.print(*val2);
    Serial.print("\n");
      // Sends every 12 minuts
      vw_send((uint8_t *)buffer, 6);
      vw_wait_tx(); // Wait until the whole message is gone
    }
  }

}
