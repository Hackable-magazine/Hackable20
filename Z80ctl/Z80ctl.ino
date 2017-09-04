#define CLKDELAY 40

#define B_CLOCK 6
#define B_RESET 7

// donne une impulsion sur la broche phi
void doClock(unsigned int n) {
  for(int i=0; i<n; i++) {
    digitalWrite(B_CLOCK, HIGH);
    delay(CLKDELAY);
    digitalWrite(B_CLOCK, LOW);
    delay(CLKDELAY);
  }
}

// provoque un reset en accord avec la doc
void doReset() {
    digitalWrite(B_RESET, LOW);
    doClock(5);
    digitalWrite(B_RESET, HIGH);
}

// configuration
void setup() {
  pinMode(B_CLOCK, OUTPUT);
  pinMode(B_RESET, OUTPUT);
  doReset();
}

// boucle principale
void loop() {
  doClock(1);
}
