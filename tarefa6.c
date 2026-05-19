#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/*
Código que realiza uma contagem crescente de 0 a 99 a cada 1 segundo,
com um botão de emergência que gera interrupção de hardware.
Quando acionada a emergência, a contagem para e a mensagem "EMERGENCIA ON" é mostrada no LCD.
A contagem volta ao normal após o botão ser pressionado novamente.
*/

const int pinBotaoEmergencia = 2; // Pino do botão
const int pinLedVermelho = 12; // Pino do LED vermelho
const int pinLedVerde = 4; // Pino do LED verde
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C padrão do módulo 16x2

volatile bool alertaEmergencia = false; // Variável para indicar estado de emergência (volatile: alterada em ISR)
bool estadoAnteriorEmergencia = false;  // Rastreia a transição de estado
unsigned long tempoAnterior = 0;
static int contador = 0;

// Função de interrupção para o botão de emergência:
void emergenciaISR() {
    alertaEmergencia = !alertaEmergencia; // Alterna o estado de emergência
}
// *Bool é atômico no AVR, mas é preciso ter cuidado com condições de corrida ao usar interrupções.

// Rotina de emergência:
void rotinaEmergencia() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCIA ON");
}

// Função para incrementar o contador e atualizar o LCD:
void atualizarContador() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Contador: ");
    lcd.print(contador);
    contador++;
    if (contador > 99) {
        contador = 0; // Reinicia a contagem após 99
    }
}

// Controla os LEDs de acordo com o estado de emergência:
void atualizarLEDs(bool emEmergencia) {
    if (emEmergencia) {
        digitalWrite(pinLedVerde, LOW);
        digitalWrite(pinLedVermelho, HIGH);
    } else {
        digitalWrite(pinLedVerde, HIGH);
        digitalWrite(pinLedVermelho, LOW);
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(pinBotaoEmergencia, INPUT_PULLUP);
    pinMode(pinLedVerde, OUTPUT);
    pinMode(pinLedVermelho, OUTPUT);

    lcd.init();
    lcd.backlight();

    attachInterrupt(
        digitalPinToInterrupt(pinBotaoEmergencia), // Configura pino do botão para interrupção
        emergenciaISR, // Função de execução da interrupção
        FALLING // Interrupção na borda de descida (pull-up: pino vai de HIGH para LOW ao pressionar)
    );
}

void loop() {

    atualizarLEDs(alertaEmergencia);

    // Transição: NORMAL -> EMERGÊNCIA
    if (alertaEmergencia && !estadoAnteriorEmergencia) {
        rotinaEmergencia();
    }

    // Transição: EMERGÊNCIA -> NORMAL
    if (!alertaEmergencia && estadoAnteriorEmergencia) {
        atualizarContador(); // Atualiza o display imediatamente ao sair da emergência
        tempoAnterior = millis(); // Evita atualização dupla imediata
    }

    estadoAnteriorEmergencia = alertaEmergencia; 

    // Execução normal do contador:
    if (!alertaEmergencia) {
        unsigned long tempoAtual = millis();
        if (tempoAtual - tempoAnterior >= 1000) {
            atualizarContador();
            tempoAnterior = tempoAtual;
        }
    }
}