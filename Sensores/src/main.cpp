#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <FastLED.h>

//! DHT22
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//! Sensor de Umidade do Solo
#define pinAnalogico 34

//! LDR
#define LDRpin 4

//! LEDs 5x5
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

// Variável para efeito arco-íris no LED 4
uint8_t rainbow_hue = 0;
void fastShowLed();
void fastShowLedRainbow4();

float temperatura, umidade, umidadeSolo, luminosidade;
int tempoEnvio = 2000;

void setup() {
  //DHT22
  dht.begin();

  // Sensor de Umidade do Solo
  pinMode(pinAnalogico, INPUT);

  //LDR
  pinMode(LDRpin, INPUT);

  //LEDs 5x5
  FastLED.addLeds<WS2812, pinLed, GRB>(leds, NUM_LEDS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[4] = CHSV(rainbow_hue, 255, 64);
  FastLED.show();

  Serial.begin(9600);
}

void loop() {
  fastShowLedRainbow4(); // Atualiza apenas o LED 4 com arco-íris

  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if(tempoAtual - tempoAnterior > tempoEnvio){

    //DHT22
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
    
    //Sensor de Umidade do Solo
    umidadeSolo = analogRead(pinAnalogico);

    //LDR
    luminosidade = analogRead(LDRpin);

    // LEDs
    fastShowLed();

    tempoAnterior = tempoAtual;
  }
}

void fastShowLedRainbow4() {
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior > 500){
    rainbow_hue += 8; // Ajuste a velocidade do arco-íris conforme desejado
    leds[4] = CHSV(rainbow_hue, 255, 64);
    FastLED.show();
    tempoAnterior = tempoAtual;
  }
}

void fastShowLed() {
  // * Temperatura
  if (temperatura < 10) {
    leds[20] = CRGB(0, 0, 0);
    leds[21] = CRGB(0, 0, 0);
    leds[22] = CRGB(0, 0, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if(temperatura >= 10 && temperatura < 15) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(0, 0, 0);;
    leds[22] = CRGB(0, 0, 0);;
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 15 && temperatura < 20) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(72, 24, 0);
    leds[22] = CRGB(0, 0, 0);;
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 20 && temperatura < 25) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(72, 24, 0);
    leds[22] = CRGB(80, 16, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 25 && temperatura < 30) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(72, 24, 0);
    leds[22] = CRGB(80, 16, 0);
    leds[23] = CRGB(88, 8, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 30) {
    leds[20] = CRGB(64, 32, 0);
    leds[21] = CRGB(72, 24, 0);
    leds[22] = CRGB(80, 16, 0);
    leds[23] = CRGB(88, 8, 0);
    leds[24] = CRGB(96, 0, 0);
  }

  // * Umidade do Ar
  if (umidade == 0) {
    leds[19] = CRGB(0, 0, 0);
    leds[18] = CRGB(0, 0, 0);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade > 0 && umidade < 20) {
    leds[19] = CRGB(0, 48, 48);
    leds[18] = CRGB(0, 0, 0);;
    leds[17] = CRGB(0, 0, 0);;
    leds[16] = CRGB(0, 0, 0);;
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 20 && umidade < 40) {
    leds[19] = CRGB(0, 48, 48);
    leds[18] = CRGB(0, 36, 60);
    leds[17] = CRGB(0, 0, 0);;
    leds[16] = CRGB(0, 0, 0);;
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 40 && umidade < 60) {
    leds[19] = CRGB(0, 48, 48);
    leds[18] = CRGB(0, 36, 60);
    leds[17] = CRGB(0, 24, 72);
    leds[16] = CRGB(0, 0, 0);;
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 60 && umidade < 80) {
    leds[19] = CRGB(0, 48, 48);
    leds[18] = CRGB(0, 36, 60);
    leds[17] = CRGB(0, 24, 72);
    leds[16] = CRGB(0, 12, 84);
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 80) {
    leds[19] = CRGB(0, 48, 48);
    leds[18] = CRGB(0, 36, 60);
    leds[17] = CRGB(0, 24, 72);
    leds[16] = CRGB(0, 12, 84);
    leds[15] = CRGB(0, 0, 96);
  }

  // * Umidade do Solo
  if(umidadeSolo > 4000) {
    leds[10] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 4000 && umidadeSolo > 3000) {
    leds[10] = CRGB(0, 96, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 3000 && umidadeSolo > 2500) {
    leds[10] = CRGB(0, 96, 0);
    leds[11] = CRGB(12, 74, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 2500 && umidadeSolo > 700) {
    leds[10] = CRGB(0, 96, 0);
    leds[11] = CRGB(12, 74, 0);
    leds[12] = CRGB(24, 72, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 700 && umidadeSolo > 400) {
    leds[10] = CRGB(0, 96, 0);
    leds[11] = CRGB(12, 74, 0);
    leds[12] = CRGB(24, 72, 0);
    leds[13] = CRGB(36, 60, 0);
    leds[14] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 400) {
    leds[10] = CRGB(0, 96, 0);
    leds[11] = CRGB(12, 74, 0);
    leds[12] = CRGB(24, 72, 0);
    leds[13] = CRGB(36, 60, 0);
    leds[14] = CRGB(48, 48, 0);
    
  }

  // * Luminosidade
  if(luminosidade < 200){
    leds[9] = CRGB(0, 0, 0);
    leds[8] = CRGB(0, 0, 0);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);

  } else if(luminosidade >= 200 && luminosidade < 1000){
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