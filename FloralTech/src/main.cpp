// Pinos

#include <Arduino.h>
#include <ESP32Servo.h>

int tempoEnvio = 500;

// LDR
const int LDRpin = 4;
int valorLDR = 0;

// 1 <1000 = breu maximo escuri
// 2 1000 - 2000 = Escurinho
// 3 2000 - 3400 = Sombra, sombrinha
// 4 3400 - 3800 = valor ideal
// 5 >3800 = muito sol

// Servo

Servo servo;

void setup()
{

 
  pinMode(LDRpin, INPUT);
  Serial.begin(9600); //
  servo.attach(18);
}

void loop()
{

  //! fazer leitura do ldr
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnterior > tempoEnvio)
  {
    valorLDR = analogRead(LDRpin);
    Serial.println(valorLDR);
    tempoAnterior = tempoAtual;
  }

  //! lógica do micro servo

  if (valorLDR <= 1000) //*Breu maximo
  {
    servo.write(180);
  }
  else if (valorLDR >= 1001 && valorLDR <= 2000) //*escurinho
  {
    servo.write(135);
  }
  else if (valorLDR > 2000 && valorLDR <= 3400) //*sombra
  {
    servo.write(90);
  }
  else if (valorLDR > 3400 && valorLDR <= 3800) //*ideal
  {
    servo.write(45);
  }
  else if (valorLDR > 3800 && valorLDR <= 4095)//* muita luz
  {
    servo.write(5);
  }
}