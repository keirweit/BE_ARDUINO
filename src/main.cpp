#include <Arduino.h>
#include <Wire.h>     // banque I2C
#include "rgb_lcd.h"  // banque LCD


#define PIN_SIG PD2    // Pin du capteur

rgb_lcd lcd;

volatile unsigned long duree = 100;
volatile unsigned long t_debut = 0;
volatile unsigned long overtime = 50000UL;
volatile int mesure_prete = 0 ;

void trigger_mesure(); 
int MesureEnCentimetre();
void affichage(int distance);

void setup() { 
  Serial.begin(9600);         // Démarre le port série
  delay(100);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  EIMSK |=  (1 << INT0); // Active l'interruption sur INT0 (DPIN_2)
  EICRA &= ~(1<<ISC01); //  sur tous les fronts
  EICRA |= (1<<ISC00); //   (montant et descendant))
}

void loop() {
   mesure_prete = 0;
  trigger_mesure();   // On envoie l'ordre au capteur de débuter la mesure 
  delay(60);          // délai préconisé par le constructeur entre 2 mesures
  cli();              // Désactivation des interruptions le temps de stocker la donnée
  int distance = duree * 0.034/2; // stockage de la distance en fonction de la durée du signal PWM
  sei();              // réactivation des interruption
    affichage(distance);
  }


void affichage(int distance) {
  static int HDP = 0;
  if(mesure_prete == 1){    // Si la mesure est prete
    if(HDP == 1){           // On réafiche "Distance" uniquement s'il à été éffacé
      lcd.setCursor(0, 0);  
      lcd.print("Distance:");
    }
    char buf[8];
    snprintf(buf, sizeof(buf), "%-4d", distance);
    lcd.setCursor(10,0);
    lcd.print(buf);         // On place le curseur après "distance"  // on affiche la distance 
    lcd.setCursor(13,0);
    lcd.print(" cm");
  
  } 
  else {
    lcd.setCursor(0,0);
    lcd.print("Hors de portee  "); // sinon c'est que la mesure n'a pas abouti
    HDP = 1;
  }
}
void trigger_mesure() {
    cli(); // pas d'interrpution

    DDRD |= (1<<PIN_SIG); // PIN_SIG en sortie
    PORTD &= ~(1<<PIN_SIG); // PIN_SIG = 0
    delayMicroseconds(2);
    PORTD |= (1<<PIN_SIG); // PIN_SIG = 1
    delayMicroseconds(5);
    PORTD &= ~(1<<PIN_SIG); // PIN_SIG = 0
    DDRD &= ~(1<<PIN_SIG); // PIN_SIG en entrée
    
    sei(); // reactivation des interruptions
}

ISR(INT0_vect){           // fonction déclenchée en cas d'interruption

  unsigned long maintenant = micros();

  if (PIND & (1 << PIN_SIG)) {  // si c'est un front montant : début de mesure
    t_debut = maintenant;       // on stocke le moment du début de la mesure
  } 
  else {                  // sinon, c'est un front descendant : fin de mesure
    unsigned long d = maintenant - t_debut; // on fait le delta entre le moment du début et de la fin du signal PWM
    if(d<overtime){       // Si le temps max est pas dépassé
      duree = d;          // on incrémente le delta dans durée
      mesure_prete = 1;  // info que la mesure est prête
    }
    else {duree = 0;}     // si l'overtime est dépassé on conscidère que la mesure est fausse
  }
}

