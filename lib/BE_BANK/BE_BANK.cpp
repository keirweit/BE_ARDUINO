#include "BE_BANK.h"

#define PIN_SIG PD2

rgb_lcd lcd;
volatile unsigned long duree = 100;
volatile unsigned long t_debut = 0;
volatile unsigned long overtime = 50000UL;
volatile int mesure_prete = 0 ;




void affichage(int distance) {
  static int HDP = 0;

  if(mesure_prete == 1 && distance<350){    // Si la mesure est prete et que la distance est en dessous de dmax
    if(HDP == 1){           // On réafiche "Distance" uniquement s'il à été éffacé
      lcd.setCursor(0, 0);  
      lcd.print("Distance:");
    }

    char buf[6];                                   // On créé un espace reservé pour notre affichage de la distance
    snprintf(buf, sizeof(buf), "%-4d", distance); // On stocke la valeur de distance dans l'espace reservé
    lcd.setCursor(9,0);
    lcd.print(buf);         // On place le curseur après "distance"  // on affiche la distance 
    lcd.setCursor(12,0);
    lcd.print(" cm");
  } 
  else {
    lcd.setCursor(0,0);
    lcd.print("Hors de portee  "); // sinon c'est que la mesure n'a pas abouti/que la distance max est atteinte
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

void clignotant(int distance){        
  
  static int ecran = 1;               // Variable permettant de savoir si l'écran est allumé ou éteint
  int maintenant = millis();          // repère temporel pour la durée du clignotement
  static int dernierchangement = 0;   // second repère temporel
  int intervalle;                     // variable utilisée pour le clignotement

  if (distance > 50){intervalle = -1;}    // Valeur de l'intervalle selon 
  if (distance < 50){intervalle = 1000;}  //  la distance
  if (distance < 40){intervalle = 800;}
  if (distance < 30){intervalle = 600;}
  if (distance < 20){intervalle = 400;}
  if (distance < 10){intervalle = 200;}
  if (distance < 5) {intervalle = 100;}

  if(ecran == 0 && ((maintenant-dernierchangement) > intervalle)){ // Si l'écran est éteint et que l'intervalle est dépassé
  lcd.display();  // on réactive l'affichage
  ecran = 1;      // Information de l'état d'écran
  dernierchangement = maintenant; // On stocke l'info temporelle
}
  
if(((maintenant - dernierchangement) > intervalle) && intervalle > 0 && ecran == 1){ // Si l'intervalle est dépassé, que l'écran est allumé et que distance < 50cm
  lcd.noDisplay();                // on désactive l'affichage
  dernierchangement = maintenant; // On stocke l'info temporelle
  ecran = 0;                      // Information de l'état d'écran

}
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