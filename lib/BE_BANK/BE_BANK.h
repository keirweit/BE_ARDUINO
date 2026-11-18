#ifndef BE_BANK_H
#define BE_BANK_H


#include <Arduino.h>
#include "rgb_lcd.h"

extern volatile unsigned long duree;
extern volatile int mesure_prete;
extern rgb_lcd lcd;

void affichage(int distance);
void trigger_mesure();
void clignotant(int distance);
ISR(INT0_vect);

#endif