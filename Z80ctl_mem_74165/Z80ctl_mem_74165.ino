#include <SPI.h>

#define B_CLOCK  A1  // horloge
#define B_RESET  A0  // reinitialisation
#define B_RD     2   // lecture
#define PL       A2  // stockage dans 74HCT126

// MOSI : n/a
// MISO : 12
// CLK  : 13
// SS   : 10 ne peut être passé en input

#define CLKDELAY  40

//               bit   0 1 2 3 4 5  6  7
int pinsData[8]     = {A3,4,5,6,7,8,9,A4};

volatile int doread=0;

void readISR() {
  doread=1;
}

void doClock(unsigned int n) {
  for(int i=0; i<n; i++) {
    digitalWrite(B_CLOCK, HIGH);
    delay(CLKDELAY);
    digitalWrite(B_CLOCK, LOW);
    delay(CLKDELAY);
  }
}

void doReset() {
    digitalWrite(B_RESET, LOW);
    doClock(5);
    digitalWrite(B_RESET, HIGH);
}

unsigned int getaddr() {
  unsigned int addr = 0;

  // MREQ Z80 -> PL ne marche pas car PL doit être HIGH pour lecture série du 74hc165
  // pas comme une EEPROM avec MREQ -> CE et RD -> OE
  digitalWrite(PL, LOW);
  delay(1);
  digitalWrite(PL, HIGH);
  
  byte shift1 = SPI.transfer(0x00);
  byte shift2 = SPI.transfer(0x00);
  
  addr = shift2;
  addr = (addr << 8);
  addr |= shift1;

/*
  for(int pin=0; pin<BITS_ADDR; pin++) {
      if(digitalRead( pinsAddress[pin] ) == HIGH )
        addr |= (1 << pin);
  }
*/
  return addr;
}

void setData(unsigned char data) {
  for( int pin = 0; pin < 8; pin++ ) pinMode(pinsData[pin], OUTPUT);  // output
  for(int pin=0; pin<8; pin++) {
    if(data & 1)
      digitalWrite(pinsData[pin], HIGH);
    else
      digitalWrite(pinsData[pin], LOW);
    data >>= 1;
  }
}

void setup() {
  digitalWrite(PL, HIGH);
  pinMode(PL, OUTPUT);
  pinMode(B_CLOCK, OUTPUT);
  pinMode(B_RESET, OUTPUT);
  pinMode(B_RD, INPUT);

  for( int pin = 0; pin < 8; pin++ ) {
    pinMode(pinsData[pin], OUTPUT);
  }

  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();

  Serial.begin(115200);
  delay(25);

  Serial.println("Reset Z80");
  doReset();

  delay(10);

  attachInterrupt(digitalPinToInterrupt(B_RD), readISR, FALLING);

}

unsigned char mem[] =
  //  00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F
  {
    0xc3, 0x04, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }; 

void loop() {
  unsigned int addr;
  
  if(doread) {
    delay(10);
    addr = getaddr();
    Serial.print("Adresse: 0x");
    if(addr < 0x10)   Serial.print("0");
    if(addr < 0x100)  Serial.print("0");
    if(addr < 0x1000) Serial.print("0");
    Serial.print(addr, HEX);
    
    if(addr >= 0 && addr <sizeof(mem)) {
      setData(mem[addr]);
      Serial.print(" [0x");
      if(mem[addr] < 0x10)   Serial.print("0");
      Serial.print(mem[addr], HEX);
      Serial.println("]");
    } else {
      setData(0x00);
      Serial.println("");
    }
    doread=0;
  }

  doClock(1);
  for( int pin = 0; pin < 8; pin++ ) pinMode(pinsData[pin], INPUT);  // input
}
