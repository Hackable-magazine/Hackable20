#include "moonfnct.h"

#define PIN_D    D5
#define PIN_STR  D7
#define PIN_CP   D6
#define PIN_OE   D8

#define TAILLELED 8

void printTmDate(tmElements_t tm) {
    if(tm.Day<10) Serial.print("0");
    Serial.print(tm.Day);
    Serial.print("/");
    if(tm.Month<10) Serial.print("0");
    Serial.print(tm.Month);
    Serial.print("/");
    Serial.print(tm.Year+1970);
    Serial.print(" ");
    if(tm.Hour<10) Serial.print("0");
    Serial.print(tm.Hour);
    Serial.print(":");
    if(tm.Minute<10) Serial.print("0");
    Serial.print(tm.Minute);
    Serial.print(":");
    if(tm.Second<10) Serial.print("0");
    Serial.println(tm.Second);
}

void printDate(int dd, int mm, int yy, int hh, int mn, int ss) {
    if(dd<10) Serial.print("0");
    Serial.print(dd);
    Serial.print("/");
    if(mm<10) Serial.print("0");
    Serial.print(mm);
    Serial.print("/");
    Serial.print(yy);
    Serial.print(" ");
    if(hh<10) Serial.print("0");
    Serial.print(hh);
    Serial.print(":");
    if(mn<10) Serial.print("0");
    Serial.print(mn);
    Serial.print(":");
    if(ss<10) Serial.print("0");
    Serial.println(ss);
}

void printmoon(tmElements_t *tm) {
    double pdatetime = jtime(tm);

    Serial.print("Jour julien: ");
    Serial.println(pdatetime);

    double pphase, mage, dist, sudist;
    double pangle = phase(pdatetime, &pphase, &mage, &dist, &sudist);

    if(pangle < 0.5 && (int)round(pphase*100)!=100)
        Serial.println("   Lune Croissante");
    else if (pangle > 0.5 && (int)round(pphase*100)!=100)
        Serial.println("   Lune decroissante");

    Serial.print("   angle: ");
    Serial.println(pangle);

    Serial.print("   illumination: ");
    Serial.print((int)round(pphase*100));
    Serial.print("%   ");
    Serial.print((int)round(pphase*10));
    Serial.println("/10");

    Serial.print("   age lunaire: ");
    Serial.print(mage);
    Serial.println("j");

    Serial.print("   distance: ");
    Serial.print(dist);
    Serial.println("km");

    Serial.print("   distance soleil: ");
    Serial.print(sudist);
    Serial.println("km");

    double phases[5];
    char *strphase[5] = { "Nouvelle lune", "Premier quartier", "Pleine lune", "Dernier quartier", "Nouvelle lune" };

    phasehunt(pdatetime, phases);
    int dd, mm, yy, hh, mn, ss;
    for(int i=0; i<5; i++) {
        jyear(phases[i], &yy, &mm, &dd);
        jhms(phases[i], &hh, &mn, &ss);
        Serial.print(strphase[i]);
        Serial.print(" = ");
        printDate(dd,mm,yy,hh,mn,ss);
    }
}

void printnextfull(tmElements_t *tm) {
  // jour, mois, année, heure, minute, seconde
  int dd, mm, yy, hh, mn, ss;

  // date en jours julien
  double pdatetime = jtime(tm);

  // calcul de la date pour la phase
  double next = nextmoon(pdatetime, 0.5);

  // décodage de la date à partir des jours julien
  jyear(next, &yy, &mm, &dd);
  jhms(next, &hh, &mn, &ss);

  // Affichage
  Serial.print("Prochaine pleine lune = ");
  printDate(dd,mm,yy,hh,mn,ss);
}

void printnextnew(tmElements_t *tm) {
  int dd, mm, yy, hh, mn, ss;
  double pdatetime = jtime(tm);
  double next = nextmoon(pdatetime, 0.0);
  jyear(next, &yy, &mm, &dd);
  jhms(next, &hh, &mn, &ss);
  Serial.print("Prochaine nouvelle lune = ");
  printDate(dd,mm,yy,hh,mn,ss);
}

void barmoon(tmElements_t *tm) {
  // date en jours julien
  double pdatetime;
  // variables pour phase()
  double pphase, mage, dist, sudist;

  // construction de la barre
  char moonslide[TAILLELED*3];
  memset(moonslide, '.', TAILLELED*3);
  memset(moonslide+TAILLELED, '*', TAILLELED);

  // conversion de la date
  pdatetime = jtime(tm);

  // calculs
  double pangle = phase(pdatetime, &pphase, &mage, &dist, &sudist);

  // index du premier caractère
  int pos = (int)(pangle*TAILLELED*2);
  // désactivation sorties leds
  digitalWrite(PIN_OE, LOW);
  Serial.print("[");
  // on boucle sur les positions à afficher
  for(int i=pos; i<pos+TAILLELED; i++) {
    Serial.write(moonslide[i]);
    // donnée : "*" ou led allumée
    if(moonslide[i]=='*') 
      digitalWrite(PIN_D, HIGH);
    else
      digitalWrite(PIN_D, LOW);
    // clock
    digitalWrite(PIN_CP, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_CP, LOW);
    delayMicroseconds(1);
  }
  Serial.println("]");
  // enregistrement dans le registre
  digitalWrite(PIN_STR, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_STR, LOW);
  // activation des sorties leds
  digitalWrite(PIN_OE, HIGH);
}

void animled() {
    char moonslide[TAILLELED*3];
    memset(moonslide, '.', TAILLELED*3);
    memset(moonslide+TAILLELED, '*', TAILLELED);

    for(int pos=0; pos<(TAILLELED*2)+1; pos++) {
      digitalWrite(PIN_OE, LOW);
      for(int i=pos; i<pos+TAILLELED; i++) {
         if(moonslide[i]=='*') 
           digitalWrite(PIN_D, HIGH);
         else
           digitalWrite(PIN_D, LOW);
         digitalWrite(PIN_CP, HIGH);
         delayMicroseconds(1);
         digitalWrite(PIN_CP, LOW);
         delayMicroseconds(1);
      }
      digitalWrite(PIN_STR, HIGH);
      delayMicroseconds(1);
      digitalWrite(PIN_STR, LOW);
      digitalWrite(PIN_OE, HIGH);
      delay(35);
    }
}

void setup() {
  // Structure pour stocker la date/heure
  tmElements_t tm;

  // configuration des broches
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_STR, OUTPUT);
  pinMode(PIN_CP, OUTPUT);
  pinMode(PIN_OE, OUTPUT);

  // configuration série
  Serial.begin(115200);
  Serial.println("Go go go");

  // lecture de l'horloge
  if(RTC.read(tm)) {
    // DS3232 error > 0
    // DS1307 error == 0
    Serial.println("RTC error");
    while(1) {;}
  }

  // petite animation
  animled();
  delay(100);

  // information
  Serial.print("Maintenant (GMT): ");
  printTmDate(tm);

  // affichage de toutes les données
//  Serial.println("");
//  printmoon(&tm);

  // affichage illumination et gestion des leds
  barmoon(&tm);

  // affichage prochaine plein lune
  printnextnew(&tm);

  // affichage prochaine nouvelle lune
  printnextfull(&tm);
}

void loop() {
  tmElements_t tm;
  
  // lecture de l'horloge
  if(RTC.read(tm)) {
    // DS3232 error > 0
    // DS1307 error == 0
    Serial.println("RTC error");
    while(1) {;}
  }
  
  // information
  Serial.print("Maintenant (GMT): ");
  printTmDate(tm);
  
  // affichage illumination et gestion des leds
  barmoon(&tm);

  delay(5000);
}
