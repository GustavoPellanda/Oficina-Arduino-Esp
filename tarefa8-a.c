```cpp
#include <Arduino.h>

/*
Código para uma simulação de um AGV (Automated Guided Vehicle) que se move para frente com sensor de colisão e procedimento de segurança.
*/

// Botão
const int PIN_BOTAO = 2;

// Sensor Ultrassônico
const int PIN_TRIG = 4;
const int PIN_ECHO = 7;

// LEDs
const int LED_VERDE = 8;
const int LED_AMARELO = 12;
const int LED_VERMELHO = 13;

// Motor Esquerdo
const int MOTOR_E_EN = 5; // PWM
const int MOTOR_E_IN1 = 9;
const int MOTOR_E_IN2 = 10;

// Motor Direito
const int MOTOR_D_EN = 6; // PWM
const int MOTOR_D_IN1 = 11;
const int MOTOR_D_IN2 = 3;

bool segurancaAtivo = false;
bool agvLigado = false;
bool velocidadeReduzida = false;
bool estadoAnteriorBotao = HIGH;
int velocidadeAtual = 0;
const int VELOCIDADE_MAXIMA = 255; // Velocidade máxima do AGV (0-255 para PWM)

// Movimenta o AGV para frente:
void avancar() {
    // Motor Esquerdo para frente (L293D: IN1=HIGH, IN2=LOW):
    digitalWrite(MOTOR_E_IN1, HIGH);
    digitalWrite(MOTOR_E_IN2, LOW);

    // Motor Direito para frente (L293D: IN1=HIGH, IN2=LOW):
    digitalWrite(MOTOR_D_IN1, HIGH);
    digitalWrite(MOTOR_D_IN2, LOW);
}

// Para o AGV:
void parar() {
    // L293D: EN=0 desabilita os canais, IN1=IN2=LOW para frear:
    analogWrite(MOTOR_E_EN, 0);
    digitalWrite(MOTOR_E_IN1, LOW);
    digitalWrite(MOTOR_E_IN2, LOW);

    analogWrite(MOTOR_D_EN, 0);
    digitalWrite(MOTOR_D_IN1, LOW);
    digitalWrite(MOTOR_D_IN2, LOW);
}

// Movimenta o AGV para trás:
void retroceder() {
    // Restaura EN para 20% da velocidade máxima antes de mudar direção:
    analogWrite(MOTOR_E_EN, VELOCIDADE_MAXIMA * 0.2);
    analogWrite(MOTOR_D_EN, VELOCIDADE_MAXIMA * 0.2);

    // Motor Esquerdo para trás (L293D: IN1=LOW, IN2=HIGH):
    digitalWrite(MOTOR_E_IN1, LOW);
    digitalWrite(MOTOR_E_IN2, HIGH);

    // Motor Direito para trás (L293D: IN1=LOW, IN2=HIGH):
    digitalWrite(MOTOR_D_IN1, LOW);
    digitalWrite(MOTOR_D_IN2, HIGH);
}

// Utiliza o sensor ultrassônico para medir a distância até o obstáculo mais próximo:
float detectarObstaculo() {
    // Envia pulso ultrassônico:
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    long duracao = pulseIn(PIN_ECHO, HIGH); // Lê o tempo do pulso de retorno
    float distancia = duracao * 0.034 / 2; // Distância em cm

    return distancia;
}

// Acelera gradualmente o AGV até a velocidade máxima:
void acelerar() {
    for(int velocidade = velocidadeAtual; velocidade <= VELOCIDADE_MAXIMA; velocidade += 5) {
        analogWrite(MOTOR_E_EN, velocidade);
        analogWrite(MOTOR_D_EN, velocidade);
        velocidadeAtual = velocidade;
        delay(20);
    }
}

// Diminui gradualmente a velocidade até 20% da velocidade máxima:
void desacelerar() {
    int velocidadeMinima = VELOCIDADE_MAXIMA * 0.2;
    for(int velocidade = velocidadeAtual; velocidade >= velocidadeMinima; velocidade -= 5) {
        analogWrite(MOTOR_E_EN, velocidade);
        analogWrite(MOTOR_D_EN, velocidade);
        velocidadeAtual = velocidade;
        delay(20);
    }
}

void procedimentoSeguranca() {
    parar();
    velocidadeAtual = 0;
    atualizarLEDs(0);
    delay(500);
    retroceder();
    delay(2000);
    parar();
    atualizarLEDs(3);
}

// Pisca os três LEDs simultaneamente:
void piscarLEDs() {
    for(int i = 0; i < 10; i++) {
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, HIGH);
        delay(200);
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        delay(200);
    }
}

// Atualiza os LEDs de acordo com o estado atual do AGV:
void atualizarLEDs(int estado) {
    switch(estado) {
        case 0: // Estado 0: Veículo parado (vermelho)
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_AMARELO, LOW);
            digitalWrite(LED_VERMELHO, HIGH);
        break;
        case 1: // Estado 1: Veículo em movimento normal (verde)
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_AMARELO, LOW);
            digitalWrite(LED_VERMELHO, LOW);
        break;
        case 2: // Estado 2: Obstáculo detectado (amarelo)
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_AMARELO, HIGH);
            digitalWrite(LED_VERMELHO, LOW);
        break;
        case 3: // Estado 3: Procedimento de segurança ativo (pisca os três LEDs)
            piscarLEDs();
        break;
        default:
            digitalWrite(LED_VERDE, LOW);
            digitalWrite(LED_AMARELO, LOW);
            digitalWrite(LED_VERMELHO, LOW);
        break;
    }
}

bool botaoPressionado() {
    return digitalRead(PIN_BOTAO) == LOW; // Botão ativo em LOW
}

void setup() {
    Serial.begin(9600);

    pinMode(PIN_BOTAO, INPUT_PULLUP);

    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);

    // Configura pinos dos motores:
    pinMode(MOTOR_E_EN, OUTPUT);
    pinMode(MOTOR_E_IN1, OUTPUT);
    pinMode(MOTOR_E_IN2, OUTPUT);

    pinMode(MOTOR_D_EN, OUTPUT);
    pinMode(MOTOR_D_IN1, OUTPUT);
    pinMode(MOTOR_D_IN2, OUTPUT);

    // Configura pinos do sensor ultrassônico:
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
}

void loop() {

    // Realiza a alternância liga/desliga do AGV:
    bool estadoAtualBotao = digitalRead(PIN_BOTAO);
    if(estadoAnteriorBotao == HIGH && estadoAtualBotao == LOW) {
        agvLigado = !agvLigado;
        delay(50);
    }
    estadoAnteriorBotao = estadoAtualBotao;

    // AGV desligado:
    if(!agvLigado) {
        parar();
        atualizarLEDs(0);

        segurancaAtivo = false;
        velocidadeReduzida = false;
        velocidadeAtual = 0;
        return;
    }

    float distancia = detectarObstaculo();
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    // Não há obstáculo à frente:
    if(distancia > 100) {
        avancar();

        // Caso esteja saindo do procedimento de segurança, acelera novamente até a velocidade máxima:
        if(velocidadeAtual == 0)  acelerar();

        segurancaAtivo = false;
        velocidadeReduzida = false;
        atualizarLEDs(1);
    }

    // Obstáculo entre 100 cm e 20 cm:
    else if(distancia > 20) {
        avancar();

        // Reduz a velocidade apenas uma vez:
        if(!velocidadeReduzida) {
            desacelerar();
            velocidadeReduzida = true;
        }

        atualizarLEDs(2);
    }

    // Obstáculo abaixo de 20 cm:
    else {
        segurancaAtivo = true;
        procedimentoSeguranca();

        // Permanece em modo de segurança até o obstáculo ser removido:
        while(detectarObstaculo() <= 100) delay(100);

        segurancaAtivo = false;
        velocidadeReduzida = false;
        velocidadeAtual = 0;
    }
}