#include <Arduino.h>
#include <Servo.h>

/*
Código que controla o abrimento e fechamento de uma cancela, 
utilizando um push button para definir o estado da cancela 
e um servo motor para movimentá-la.
*/

const int buttonPin = 2; // Pino do botão
const int servoPin = 9; // Pino do servo motor
const int ledPin = 13; // Pino do LED verde
const int ledPin2 = 12; // Pino do LED vermelho
Servo servo;

// Lê o estado do botão:
bool lerBotao(int pin) {
    if (digitalRead(pin) == LOW) {
        delay(50); // Aguarda estabilizar (debounce)
        return digitalRead(pin) == LOW; // Confirma o estado
    }
    return false;
}

// Move o servo para a posição de abertura ou fechamento:
void moverServo(bool abrir) {
    if (abrir) {
        servo.write(90); // Abre a cancela
    } else {
        servo.write(0); // Fecha a cancela
    }
}

// Controla os LEDs de acordo com o estado da cancela:
void controlarLEDs(bool abrir) {
    if (abrir) {
        digitalWrite(ledPin, HIGH); // Acende LED verde
        digitalWrite(ledPin2, LOW); // Apaga LED vermelho
        Serial.println("Cancela aberta");
    } else {
        digitalWrite(ledPin, LOW); // Apaga LED verde
        digitalWrite(ledPin2, HIGH); // Acende LED vermelho
        Serial.println("Cancela fechada");
    }
}

void setup() {
    Serial.begin(9600);
    servo.attach(servoPin);
    pinMode(buttonPin, INPUT_PULLUP); // Resistor pull-up interno do Arduino
    pinMode(ledPin, OUTPUT);
    pinMode(ledPin2, OUTPUT);
}

void loop() {
    bool botaoPressionado = lerBotao(buttonPin);
    moverServo(botaoPressionado);
    controlarLEDs(botaoPressionado);
}