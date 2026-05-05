#include <Arduino.h>
#include <LiquidCrystal.h>

/*
Código para controlar a cor de um LED RGB com base na tensão lida de um potenciômetro, exibindo a tensão e a cor correspondente em um display LCD 16x2.
*/

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
const int potPin = A0; // Pino do potenciômetro
const int leds[] = {11, 12, 13}; // Pinos do LED RGB: vermelho, verde, azul

// Representação das cores em formato RGB (1 = aceso, 0 = apagado):
const int VERMELHO[3] = {1, 0, 0};
const int VERDE[3]    = {0, 1, 0};
const int AZUL[3]     = {0, 0, 1};
const int BRANCO[3]   = {1, 1, 1};
const int MAGENTA[3]  = {1, 0, 1};
const int AMARELO[3]  = {1, 1, 0};
const int CIANO[3]    = {0, 1, 1};

// Lê o potenciômetro e retorna a tensão (0 a 5V):
float lerTensao(int pin) {
    int leitura = analogRead(pin);
    return leitura * (5.0 / 1023.0);
}

// Acende LEDs de acordo com o array de estados:
void acendeLed(const int* pinos, const int* estados, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (estados[i] == 1) {
            digitalWrite(pinos[i], HIGH);
        } else {
            digitalWrite(pinos[i], LOW);
        }
    }
}

// Mostra tensão e cor no LCD:
void mostraLCD(float tensao, String cor) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tensao: ");
    lcd.print(tensao, 2);
    lcd.print("V");

    lcd.setCursor(0, 1);
    lcd.print("Cor: ");
    lcd.print(cor);
}

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);

    for (int i = 0; i < 3; i++) {
        pinMode(leds[i], OUTPUT);
    }
}

void loop() {
    float tensao = lerTensao(potPin);
    Serial.print("Tensão: ");
    Serial.println(tensao, 2);

    String cor = "";

    // Decide qual cor acender com base na tensão lida:
    if (tensao < 0.75) {
        cor = "VERMELHO";
        acendeLed(leds, VERMELHO, 3);
    }
    else if (tensao < 2.0) {
        cor = "VERDE";
        acendeLed(leds, VERDE, 3);
    }
    else if (tensao < 3.0) {
        cor = "AZUL";
        acendeLed(leds, AZUL, 3);
    }
    else if (tensao < 3.25) {
        cor = "BRANCO";
        acendeLed(leds, BRANCO, 3);
    }
    else if (tensao < 4.0) {
        cor = "MAGENTA";
        acendeLed(leds, MAGENTA, 3);
    }
    else if (tensao < 4.8) {
        cor = "AMARELO";
        acendeLed(leds, AMARELO, 3);
    }
    else {
        cor = "CIANO";
        acendeLed(leds, CIANO, 3);
    }

    mostraLCD(tensao, cor);
    delay(200);
}