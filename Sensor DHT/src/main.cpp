#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <FastLED.h>

#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define pinLed 15 // Saída
#define NUM_LEDS 25 // Número de LEDs
CRGB leds[NUM_LEDS]; // Array de LEDs
/*
  20 21 22 23 24
  19 18 17 16 15
  10 11 12 13 14
  9  8  7  6  5    
  0  1  2  3  4
*/
uint8_t r = 16;
uint8_t g = 16;
uint8_t b = 16;

void fastShowLed();

float temperatura, umidade;
float luminosidade = 0; // Placeholder for luminosity, not used in this example
float umidadeSolo = 0; // Placeholder for soil humidity, not used in this example
int tempoEnvio = 2000;

void setup() {
  //DHT22
  dht.begin();

  //LEDs 5x5
  FastLED.addLeds<WS2812, pinLed, GRB>(leds, NUM_LEDS);

  Serial.begin(9600);
}

void loop() {
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if(tempoAtual - tempoAnterior > tempoEnvio){
    
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
    Serial.printf("Temperatura: %.2f°C, Umidade: %.2f% \n", temperatura, umidade);
    fastShowLed();

    tempoAnterior = tempoAtual;
  }
}

void fastShowLed() {
  // * Temperatura
  if(temperatura < 15) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(0, 0, 0);
    leds[22] = CRGB(0, 0, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 15 && temperatura < 20) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(68, 28, 0);
    leds[22] = CRGB(0, 0, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 20 && temperatura < 25) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(68, 28, 0);
    leds[22] = CRGB(72, 24, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 25 && temperatura < 30) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(68, 28, 0);
    leds[22] = CRGB(72, 24, 0);
    leds[23] = CRGB(76, 20, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 30) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(68, 28, 0);
    leds[22] = CRGB(72, 24, 0);
    leds[23] = CRGB(76, 20, 0);
    leds[24] = CRGB(80, 16, 0);
  }

  // * Umidade do Ar
  if(umidade < 20) {
    leds[19] = CRGB(0, 32, 64);
    leds[18] = CRGB(0, 0, 0);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 20 && umidade < 40) {
    leds[19] = CRGB(0, 32, 64);
    leds[18] = CRGB(0, 24, 72);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 40 && umidade < 60) {
    leds[19] = CRGB(0, 32, 64);
    leds[18] = CRGB(0, 24, 72);
    leds[17] = CRGB(0, 16, 80);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 60 && umidade < 80) {
    leds[19] = CRGB(0, 32, 64);
    leds[18] = CRGB(0, 24, 72);
    leds[17] = CRGB(0, 16, 80);
    leds[16] = CRGB(0, 8, 88);
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 80) {
    leds[19] = CRGB(0, 32, 64);
    leds[18] = CRGB(0, 24, 72);
    leds[17] = CRGB(0, 16, 80);
    leds[16] = CRGB(0, 8, 88);
    leds[15] = CRGB(0, 0, 96);
  }

  // * Umidade do Solo
  if(umidadeSolo > 4000) {
    leds[14] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[10] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 4000 && umidadeSolo > 3000) {
    leds[14] = CRGB(0, 96, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[10] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 3000 && umidadeSolo > 2500) {
    leds[14] = CRGB(0, 96, 0);
    leds[13] = CRGB(8, 88, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[10] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 2500 && umidadeSolo > 700) {
    leds[14] = CRGB(0, 96, 0);
    leds[13] = CRGB(8, 88, 0);
    leds[12] = CRGB(16, 80, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[10] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 700 && umidadeSolo > 400) {
    leds[14] = CRGB(0, 96, 0);
    leds[13] = CRGB(8, 88, 0);
    leds[12] = CRGB(16, 80, 0);
    leds[11] = CRGB(24, 72, 0);
    leds[10] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 400) {
    leds[14] = CRGB(0, 96, 0);
    leds[13] = CRGB(8, 88, 0);
    leds[12] = CRGB(16, 80, 0);
    leds[11] = CRGB(24, 72, 0);
    leds[10] = CRGB(32, 64, 0);
    
  }

  // * Luminosidade
  if(luminosidade < 1000){
    leds[9] = CRGB(32, 0, 64);
    leds[8] = CRGB(0, 0, 0);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
  } else if (luminosidade >= 1000 && luminosidade < 2000) {
    leds[9] = CRGB(32, 0, 64);
    leds[8] = CRGB(36, 0, 60);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 2000 && luminosidade < 3400) {
    leds[9] = CRGB(32, 0, 64);
    leds[8] = CRGB(36, 0, 60);
    leds[7] = CRGB(40, 0, 56);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 3400 && luminosidade < 3800) {
    leds[9] = CRGB(32, 0, 64);
    leds[8] = CRGB(36, 0, 60);
    leds[7] = CRGB(40, 0, 56);
    leds[6] = CRGB(44, 0, 52);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 3800) {
    leds[9] = CRGB(32, 0, 64);
    leds[8] = CRGB(36, 0, 60);
    leds[7] = CRGB(40, 0, 56);
    leds[6] = CRGB(44, 0, 52);
    leds[5] = CRGB(48, 0, 48);
    
  }

  FastLED.show();
}