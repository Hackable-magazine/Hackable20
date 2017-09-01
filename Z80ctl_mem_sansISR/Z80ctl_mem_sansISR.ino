#define B_CLOCK  12  // horloge
#define B_RESET  13  // réinitialisation
#define B_MREQ   3   // requête mémoire
#define B_RD     2   // lecture

#define BITS_ADDR  6  // nombre de bits d'adresse
#define MEM_SIZE  64  // taille 2^6
#define CLKDELAY  60  // delai horloge

// Bus d'adresse
//                   bit       0  1  2  3  4  5
int pinsAddress[BITS_ADDR] = {A0,A1,A2,A3,A4,A5};

// Bus de données
//               bit   0 1 2 3 4 5  6  7
int pinsData[8]     = {4,5,6,7,8,9,10,11};

// Fonction horloge
void doClock(unsigned int n) {
  for(int i=0; i<n; i++) {
    digitalWrite(B_CLOCK, HIGH);
    delay(CLKDELAY);
    digitalWrite(B_CLOCK, LOW);
    delay(CLKDELAY);
  }
}

// Réinitialisation
void doReset() {
    digitalWrite(B_RESET, LOW);
    doClock(10);
    digitalWrite(B_RESET, HIGH);
}

// Lecture de l'adresse
unsigned int getaddr() {
  unsigned int addr = 0;

  for(int pin=0; pin<BITS_ADDR; pin++) {
      if(digitalRead( pinsAddress[pin] ) == HIGH )
        addr |= (1 << pin);
  }
  
  return addr;
}

// Définition des bits de données
void setData(unsigned char data) {
    for(int pin=0; pin<8; pin++) {
      if(data & 1)
        digitalWrite(pinsData[pin], HIGH);
      else
        digitalWrite(pinsData[pin], LOW);
      data >>= 1;
    }
}

// Configuration
void setup() {
  pinMode(B_CLOCK, OUTPUT);
  pinMode(B_RESET, OUTPUT);
  pinMode(B_MREQ, INPUT);
  pinMode(B_RD, INPUT);

  // Tout le bus de données en sortie
  for( int pin = 0; pin < 8; pin++ ) {
    pinMode(pinsData[pin], OUTPUT);
  }

  // Tout le bus d'adresse en entrée
  for( int pin = 0; pin<BITS_ADDR; pin++ ) {
    pinMode(pinsAddress[pin], INPUT);
  }

  // Activation moniteur série
  Serial.begin(115200);
  delay(5);

  // Reset du Z80
  Serial.println("Reset Z80");
  doReset();
  delay(5);
}

/*
00 : JP 04 00
03 : 00 nop
04 : 00 nop
05 : 00 nop
06 : JP 00 00
09 : 00 nop
10 : 00 nop
11 : 00 nop
12 : 00 nop
...

0xc3 0x04 0x00
0x00
0x00
0x00
0xc3 0x00 0x00

 JP    L     H     nop   nop   JP    L     H     nop   nop   nop   nop   nop...
0xc3, 0x04, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00... 
*/

// Mémoire pour notre Z80
unsigned char mem[MEM_SIZE] =
  //  00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F
  {
    0xc3, 0x04, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }; 

// Ancienne adresse traitée
unsigned int oldaddr = 0x42;

// Boucle principale
void loop() {
  // Récupération de l'adresse
  unsigned int addr = getaddr();
  
  if(!digitalRead(B_RD) && addr != oldaddr) {
    // Affichage
    Serial.print("Adresse: 0x");
    if(addr < 0x10)   Serial.print("0");
    if(addr < 0x100)  Serial.print("0");
    if(addr < 0x1000) Serial.print("0");
    Serial.print(addr, HEX);

    // Si l'adresse est valide (dans les 64 octets de mémoire)
    if(addr >= 0 && addr <sizeof(mem)) {
      // On "pousse" les données correspondante sur le bus
      setData(mem[addr]);
      // Et on affiche le tout
      Serial.print(" [0x");
      if(mem[addr] < 0x10)   Serial.print("0");
      Serial.print(mem[addr], HEX);
      Serial.println("]");
    } else {
      // Nous sommes hors de l'espace mémoire émulé
      // On renvoi 0x00, l'instruction NOP
      setData(0x00);
    }
    // On met à jour la précédent adresse traitée
    oldaddr = addr;
  }

  // Impulsion horloge
  doClock(1);
}
