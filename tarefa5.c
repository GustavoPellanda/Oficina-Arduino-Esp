#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> 

/*
Esse código movimenta uma plataforma de painel solar utilizando dois servomotores (um para o eixo horizontal e outro para o eixo vertical),
baseado na leitura de quatro sensores de luminosidade (LDRs) posicionados em cada canto da plataforma.
*/

const int ldrPins[] = {A0, A1, A2, A3}; // Pinos dos LDRs
// A0 = superior esquerdo | A1 = superior direito
// A2 = inferior esquerdo | A3 = inferior direito

const int servoHorizontalPin = 9; // Pino do servo horizontal
const int servoVerticalPin = 10; // Pino do servo vertical

const int LIMIAR_DIFERENCA = 50; // Sensibilidade mínima para movimentação

// Limites de ângulo dos servos:
const int ANGULO_MIN = 0;
const int ANGULO_MAX = 180;

// Posições iniciais dos servos:
int posHorizontal = 90;
int posVertical = 90;

Servo servoHorizontal;
Servo servoVertical;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Calcula a diferença de luminosidade entre os LDRs:
void calcularDiferencas(int ldrValues[], float &difH, float &difV) {

    int esquerda = ldrValues[0] + ldrValues[2]; // A0 + A2
    int direita  = ldrValues[1] + ldrValues[3]; // A1 + A3

    int superior = ldrValues[0] + ldrValues[1]; // A0 + A1
    int inferior = ldrValues[2] + ldrValues[3]; // A2 + A3

    difH = esquerda - direita;
    difV = superior - inferior;
}

// Controla os servos com base nas diferenças de luminosidade:
void controlarServos(float diferencaHorizontal, float diferencaVertical) {

    // Ajusta o servo horizontal:
    if(abs(diferencaHorizontal) > LIMIAR_DIFERENCA) {
        if(diferencaHorizontal > 0) { // Mais luz na esquerda:
            posHorizontal++;
        }
        else { // Mais luz na direita:
            posHorizontal--;
        }
    }

    // Ajusta o servo vertical:
    if(abs(diferencaVertical) > LIMIAR_DIFERENCA) {
        if(diferencaVertical > 0) { // Mais luz na parte superior:
            posVertical++;
        }
        else { // Mais luz na parte inferior:
            posVertical--;
        }
    }

    // Garante que os ângulos permaneçam dentro dos limites:
    posHorizontal = constrain(posHorizontal, ANGULO_MIN, ANGULO_MAX);
    posVertical = constrain(posVertical, ANGULO_MIN, ANGULO_MAX);

    servoHorizontal.write(posHorizontal);
    servoVertical.write(posVertical);
}

// Posiciona os servos na posição de descanso:
void posicaoDescanso() {
    posHorizontal = 90;
    posVertical = 90;
    servoHorizontal.write(posHorizontal);
    servoVertical.write(posVertical);
}

// Exibe as leituras dos LDRs no display LCD:
void atualizarDisplay(int ldrValues[]) {
    // Primeira linha:
    lcd.setCursor(0, 0);
    lcd.print("SE:");
    lcd.print(ldrValues[0]);
    lcd.print("   ");

    lcd.setCursor(8, 0);
    lcd.print("SD:");
    lcd.print(ldrValues[1]);
    lcd.print("   ");

    // Segunda linha:
    lcd.setCursor(0, 1);
    lcd.print("IE:");
    lcd.print(ldrValues[2]);
    lcd.print("   ");

    lcd.setCursor(8, 1);
    lcd.print("ID:");
    lcd.print(ldrValues[3]);
    lcd.print("   ");
}

void setup() {
    Serial.begin(9600);
    for(int i = 0; i < 4; i++) pinMode(ldrPins[i], INPUT);
    servoHorizontal.attach(servoHorizontalPin);
    servoVertical.attach(servoVerticalPin);
    lcd.init();
    lcd.backlight();
    posicaoDescanso();
}

void loop() {

    // Leitura dos sensores de luminosidade:
    int ldrValues[4];
    for(int i = 0; i < 4; i++) {
        ldrValues[i] = analogRead(ldrPins[i]);
    }

    atualizarDisplay(ldrValues);

    // Se for noite (baixa luminosidade em todos os LDRs), posiciona os servos na posição de descanso:
    if(ldrValues[0] < 100 && ldrValues[1] < 100 && ldrValues[2] < 100 && ldrValues[3] < 100) {
        posicaoDescanso();
        delay(1000);
        return; // Pula o restante do loop para evitar movimentação desnecessária
    }

    float diferencaHorizontal;
    float diferencaVertical;
    calcularDiferencas(ldrValues, diferencaHorizontal,diferencaVertical);
    controlarServos(diferencaHorizontal,diferencaVertical);

    delay(20);
}