#include <Stepper.h>
#include <LiquidCrystal_I2C.h>

/*
Código que controla uma esteira transportadora utilizando um motor de passo e um sensor ultrassônico para contar peças.
O sistema possui um botão de emergência que, quando acionado, para a contagem e reseta o número de peças para zero.
*/

// Motor de passo (28BYJ-48):
const int passosPorVolta = 2048;
Stepper motor(passosPorVolta, 8, 10, 9, 11); // IN1 IN3 IN2 IN4

// Sensor ultrassônico:
const int TRIG = 5;
const int ECHO = 6;

// Variáveis de controle da emergência:
const int pinBotaoEmergencia = 2; // Botão de emergência
volatile bool alertaEmergencia = false; // Variável da interrupção
bool estadoAnteriorEmergencia = false; // Controle de transição

// Variáveis para contagem de peças:
const float DISTANCIA_SENSOR = 25.0; // Distância fixa do sensor até a esteira
int quantidadePecas = 0; // Contagem de peças
bool pecaDetectada = false; // Controle de detecção

LiquidCrystal_I2C lcd(0x27, 16, 2); // DIsplay LCD 16x2

void displayInfo(float altura, int quantidadePecas){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Altura: ");
    lcd.print(altura);
    lcd.setCursor(0, 1);
    lcd.print("Pecas: ");
    lcd.print(quantidadePecas);

    Serial.print("Altura: ");
    Serial.print(altura);
    Serial.print(" cm | Pecas: ");
    Serial.println(quantidadePecas);
}

// Função de interrupção:
void emergenciaISR(){
    alertaEmergencia = !alertaEmergencia;
}

// Rotina de emergência:
void rotinaEmergencia(){
    quantidadePecas = 0;
    Serial.println("EMERGENCIA");
}

// Leitura da distância pelo sensor ultrassônico:
float medirDistancia(){
    long duracao;
    float distancia;

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG, LOW);

    duracao = pulseIn(ECHO, HIGH);
    distancia = duracao * 0.0343 / 2.0;
    return distancia;
}

// Conta as peças:
void contarPecas(float altura){
    if (altura > 2.0 && !pecaDetectada){
        quantidadePecas++;
        pecaDetectada = true;
    }

    if (altura <= 2.0) pecaDetectada = false;
}

// Movimenta a esteira:
void movimentarEsteira(){
    motor.step(64);
}

void setup(){
    Serial.begin(9600);

    lcd.begin();
    lcd.backlight();

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(pinBotaoEmergencia, INPUT_PULLUP);
    motor.setSpeed(10);

    // Configura a interrupção para o botão de emergência:
    attachInterrupt(
        digitalPinToInterrupt(pinBotaoEmergencia),
        emergenciaISR,
        FALLING
    );
}

void loop(){

    // Estado normal -> emergência
    if (alertaEmergencia && !estadoAnteriorEmergencia){
        rotinaEmergencia();
    }

    estadoAnteriorEmergencia = alertaEmergencia;

   // Funcionamento normal:
    if (!alertaEmergencia){
        movimentarEsteira();

        float distancia = medirDistancia();
        float altura = DISTANCIA_SENSOR - distancia;

        contarPecas(altura);

        displayInfo(altura, quantidadePecas);
    }
}