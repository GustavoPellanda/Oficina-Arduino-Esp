#include <Arduino.h>
#include <Servo.h>

/*
Código que controla o abrimento e fechamento de uma cancela, 
utilizando um potenciômetro para definir a posição da cancela 
e um servo motor para movimentá-la.
*/

const int potPin = A0; // Pino do potenciômetro
const int servoPin = 9; // Pino do servo motor
Servo servo;

// Lê o potenciômetro e retorna a tensão (0 a 5V):
float lerTensao(int pin) {
    int leitura = analogRead(pin);
    return leitura * (5.0 / 1023.0);
}

// Mapeia a tensão para o ângulo do servo (0 a 180 graus):
int mapearTensaoParaAngulo(float tensao) {
    int angulo = (int)(tensao * (180.0 / 5.0));
    return constrain(angulo, 0, 180); // Garante que o ângulo não ultrapasse 0 ou 180
}

// Move o servo para o ângulo especificado:
void moverServo(int angulo) {
    servo.write(angulo);
}

void setup() {
    Serial.begin(9600);
    servo.attach(servoPin);
    pinMode(potPin, INPUT);
}

void loop() {
    float tensao = lerTensao(potPin);
    int angulo = mapearTensaoParaAngulo(tensao);
    moverServo(angulo);

    if (angulo < 90) {
        Serial.println("Cancela Fechada");
    } else {
        Serial.println("Cancela Aberta");
    }
}