#include <Arduino.h>
#include <Wire.h>     // banque I2C
#include "rgb_lcd.h"  // banque LCD
#include "BE_BANK.h"

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
  static int distanceavant = 0;
   mesure_prete = 0;

  trigger_mesure();                // On envoie l'ordre au capteur de débuter la mesure 
  delay(60);                       // délai préconisé par le constructeur entre 2 mesures
  cli();                           // Désactivation des interruptions le temps de stocker la donnée
  int distance = duree * 0.034/2; // stockage de la distance en fonction de la durée du signal PWM
  sei();                          // réactivation des interruption
  if(distanceavant != distance){  // Si la distance à changée
    affichage(distance);          // On affiche la distance sur le LCD
    distanceavant = distance;     // Condition pour vérifier si la distance a changée
  }
  clignotant(distance);           // Fonction qui clignote l'affichage selon la distance 
}

