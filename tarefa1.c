#include <Arduino.h>

const int LEDS[] = {13, 12, 11, 10, 9, 8, 7};

void setup() {
  for (int i = 0; i < 7; i++) pinMode(LEDS[i], OUTPUT);
}

void loop() {
  // Acende os leds sequencialmente:
  for (int i = 0; i < 7; i++) {
    digitalWrite(LEDS[i], HIGH);
    delay(300);
  }

  // Pisca todos os leds juntos três vezes:
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) digitalWrite(LEDS[j], LOW);
    delay(300);
    for (int j = 0; j < 7; j++) digitalWrite(LEDS[j], HIGH);
    delay(300);
  }

  // Apaga os leds na ordem contrária:
  for (int i = 6; i >= 0; i--) {
    digitalWrite(LEDS[i], LOW);
    delay(300);
  }
  
  delay(5000);
}