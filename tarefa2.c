#include <Arduino.h>

/*
Código para controlar dois displays de 7 segmentos utilizando CD4511, com um switch para ligar e desligar a exibição.
*/

const int pinSwitch = 2; // Pino do switch
const int pinsDisplay1[] = {10, 11, 12, 13}; // A, B, C, D
const int pinsDisplay2[] = {4, 5, 6, 7}; 
const int pinLimpa1 = 9; // Pino que liga e desliga o display
const int pinLimpa2 = 3; 

// Retorna verdadeiro se o switch estiver ligado (HIGH), caso contrário, retorna falso (LOW):
bool isSwitchOn() {
    return digitalRead(pinSwitch) == HIGH;
}

// Recebe um número de 0 a 9 e codifica para BCD, escrevendo os bits nos pinos do display:
void exibeNumero(const int* pinsDisplay, int numero) {
    if (numero < 0 || numero > 9) return; // O display de 7 segmentos só pode exibir números de 0 a 9

    // A cada iteração, desloca o número i bits para a direita e pega o bit menos significativo com máscara and 1:
    for (int i = 0; i < 4; i++) {
        digitalWrite(pinsDisplay[i], (numero >> i) & 1);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(pinSwitch, INPUT);
  	pinMode(pinLimpa1, OUTPUT);
    pinMode(pinLimpa2, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(pinsDisplay1[i], OUTPUT);
        pinMode(pinsDisplay2[i], OUTPUT);
    }
}

void loop()
{
    if (isSwitchOn()) {
        // Dia: 25
        digitalWrite(pinLimpa1, HIGH);
        digitalWrite(pinLimpa2, HIGH);
        exibeNumero(pinsDisplay1, 2);
        exibeNumero(pinsDisplay2, 4);
        delay(1000);
        digitalWrite(pinLimpa1, LOW);
        digitalWrite(pinLimpa2, LOW);
        delay(100);

        // Mês: 03
        digitalWrite(pinLimpa1, HIGH);
        digitalWrite(pinLimpa2, HIGH);
        exibeNumero(pinsDisplay1, 0);
        exibeNumero(pinsDisplay2, 3);
        delay(1000);
        digitalWrite(pinLimpa1, LOW);
        digitalWrite(pinLimpa2, LOW);
        delay(100);

        // Ano: 26
        digitalWrite(pinLimpa1, HIGH);
        digitalWrite(pinLimpa2, HIGH);
        exibeNumero(pinsDisplay1, 2);
        exibeNumero(pinsDisplay2, 6);
        delay(1000);
        digitalWrite(pinLimpa1, LOW);
        digitalWrite(pinLimpa2, LOW);
        delay(100);

        // Aguarda 1 segundo:
        delay(1000);
        digitalWrite(pinLimpa1, LOW);
        digitalWrite(pinLimpa2, LOW);
        delay(1000);

        // Pisca 8 três vezes:
        for (int i = 0; i < 3; i++) {
            digitalWrite(pinLimpa1, HIGH);
            digitalWrite(pinLimpa2, HIGH);
            exibeNumero(pinsDisplay1, 8);
            exibeNumero(pinsDisplay2, 8);
            delay(500);
            digitalWrite(pinLimpa1, LOW);
            digitalWrite(pinLimpa2, LOW);
            delay(500);
        }
    }
    else {
        digitalWrite(pinLimpa1, LOW);
        digitalWrite(pinLimpa2, LOW);
    }
}