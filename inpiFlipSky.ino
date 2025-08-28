//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Firmware para controle de servomotor com base no controle
//remoto via LORAWAN FlipSky VX3 via controle PPM e não 
//UART
//Autor: Boris Vargas 2025
// Library: Flipsky Remote Reader Library
//https://github.com/Chipney/FlipskyPPM
//Créditos para: Aaron Beckmann 2021
//License: GNU GPL v3 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "FlipskyPPM.h" // Flipsky library
#include <Servo.h> // Inclui Servo library
//Instance class
FlipskyPPM reciever(2);//Receiver PPM ligado no PINO 2 do Arduino pro-mini
Servo servoMotor; 
//Seleção do curso em graus com base nas entradas digitais
#define in45g 12 
#define in90g 10
#define in120g 11
//Saídas digitais
#define chave 4 // Chave que liga geral
#define partida 5 // Motor de partida
#define statusLed 13 //Led da propria placa arduino para indicar fora de range o controle.
//Variaveis
int servoPos = 0; // Posição do servo
int controlValue = 0;
int posSelected = 180;
unsigned long previousMillis = 0;
const long interval = 2000;
bool fpPulseLigaDesliga = false;
bool modoPartidaAtivo = false;
bool jetLigado = false;
bool transicaoLigaDesliga = false;
//Init Setup
void setup()
{
  Serial.begin(115200);
  servoMotor.attach(9); //Atribui pino 9 para PWM do servo
  pinMode(statusLed,OUTPUT);
  pinMode(chave,OUTPUT);
  digitalWrite(chave,LOW);
  pinMode(partida,OUTPUT);
  digitalWrite(partida,LOW);
  pinMode(in45g,INPUT_PULLUP);
  pinMode(in90g,INPUT_PULLUP);
  pinMode(in120g,INPUT_PULLUP);
}
//Main Loop
void loop(){
  unsigned long currentMillis = millis();
  controlValue = reciever.getNewestValue(); //Flipsky envia novo valor a cada ~20ms
  //Apertando o freio do controle os valores ficam entre 0 e -20
  //Neste caso este modo é usado para ligar/desliga e acionar motor de partida
  if(controlValue<0 and controlValue>-20){
    modoPartidaAtivo = true;
  }else{
    modoPartidaAtivo = false;
  }
  
  //Verifica modo de liga/desliga
  if(modoPartidaAtivo and !fpPulseLigaDesliga){
    previousMillis = currentMillis;
    fpPulseLigaDesliga = true;
  }
  if (!(modoPartidaAtivo)){
    fpPulseLigaDesliga = false;
  }

  if(modoPartidaAtivo and (currentMillis - previousMillis >= interval) and !jetLigado and !transicaoLigaDesliga){
    transicaoLigaDesliga = true;
    digitalWrite(chave, HIGH);
    jetLigado = true;
    delay(500);
    digitalWrite(partida, HIGH);
  }

  if(!modoPartidaAtivo and jetLigado){
    digitalWrite(partida, LOW);
  }

  if(modoPartidaAtivo and (currentMillis - previousMillis >= interval) and jetLigado and !transicaoLigaDesliga){
    transicaoLigaDesliga = true;
    jetLigado = false;
    digitalWrite(chave, LOW);
  }

  if(!modoPartidaAtivo){
    transicaoLigaDesliga = false;
  }

  if(!digitalRead(in45g)){
    posSelected = 45;
  }
  else if(!digitalRead(in90g)){
    posSelected = 90;
  }
  else if(!digitalRead(in120g)){
    posSelected = 120;
  }else{
    posSelected = 180;
  }
  if (controlValue < -100 || controlValue > 1000){
    digitalWrite(statusLed, !digitalRead(statusLed));
  }else{
    digitalWrite(statusLed, LOW);
  }

  servoPos = map(controlValue,480,956,0,posSelected);
  servoMotor.write(servoPos);
  delay(1); 
  
}