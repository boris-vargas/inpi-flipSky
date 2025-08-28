/*

*/

#include "FlipskyPPM.h"
#include <Servo.h> // Include the Servo library

FlipskyPPM reciever(2);
Servo myservo; // Create a Servo object named myservo

#define in45g 12
#define in90g 10
#define in120g 11
#define chave 4
#define partida 5
int servoPos = 0; // Variable to store the servo position

void setup()
{
  Serial.begin(115200);
  myservo.attach(9); // Attaches the servo on pin 9 to the Servo object
  pinMode(13,OUTPUT);
  pinMode(chave,OUTPUT);
  digitalWrite(chave,LOW);
  pinMode(partida,OUTPUT);
  digitalWrite(partida,LOW);
  pinMode(in45g,INPUT_PULLUP);
  pinMode(in90g,INPUT_PULLUP);
  pinMode(in120g,INPUT_PULLUP);
  Serial.println("Init FlyControl siroB@");
}

int controlValue = 0;
int posSelected = 180;

unsigned long previousMillis = 0;
const long interval = 2000;

bool fpPulseLigaDesliga = false;
bool modoPartidaAtivo = false;
bool jetLigado = false;
bool transicaoLigaDesliga = false;

void loop(){
   unsigned long currentMillis = millis();

  controlValue = reciever.getNewestValue(); //Flipsky Remotes send new value every ~20ms

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
      Serial.println("Modo de liga");
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
    Serial.println("Desliguei JET");
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
    digitalWrite(13, !digitalRead(13));
  }else{
    digitalWrite(13, LOW);
  }

  servoPos = map(controlValue,480,956,0,posSelected);
  myservo.write(servoPos);
  delay(1); 
  
}