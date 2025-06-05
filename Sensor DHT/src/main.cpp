#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temperatura, umidade;
int tempoEnvio = 2000;

void setup() {
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if(tempoAtual - tempoAnterior > tempoEnvio){
    
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
    Serial.printf("Temperatura: %.2f°C, Umidade: %.2f% \n", temperatura, umidade);

    tempoAnterior = tempoAtual;
  }
}