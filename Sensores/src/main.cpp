#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <FastLED.h>
#include <ESP32Servo.h>
#include <ezTime.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "internet.h"

//! DHT22
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//! Sensor de Umidade do Solo
#define pinAnalogico 34

//! LDR
#define LDRpin 35

//! LEDs 5x5
#define pinLed 13 // Saída
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
uint8_t rainbow_hue[5] = {0, 10, 20, 30, 40}; // Cores do arco-íris
void fastShowLed();
void fastShowLedRainbow();

//! Microservo
Servo servo;
void microservoControl();


//! ezTime
Timezone tempoLocal;
unsigned long timestamp = 0;

// * Variáveis para armazenar os dados dos sensores
float temperatura, umidade, umidadeSolo, luminosidade;
int tempoEnvio = 1000;

// * MQTT
WiFiClient espClient;
PubSubClient client(espClient);
bool envioMqtt = false;
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-projeto07";
const char *mqtt_topic_sub = "senai134/mesa07/esp_sub";
const char *mqtt_topic_pub = "senai134/mesa07/esp_infoSensores";

void callback(char *, byte *, unsigned int);
void mqttConnect();

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
  FastLED.show();

  // Microservo
  servo.attach(18);

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  conectaWiFi();

  // ezTime
  Serial.begin(9600);
  Serial.println("Sincronizando com o servidor de horário...");
  waitForSync();
  tempoLocal.setLocation("America/Sao_Paulo");
}

void loop() {
  checkWiFi();
  if (!client.connected()) mqttConnect();
  client.loop();

  fastShowLedRainbow(); // Atualiza apenas o LED 4 com arco-íris
  
  JsonDocument doc;
  String mensagem = "";

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

    // Microservo
    microservoControl();

    // ezTime
    timestamp = now();

    // MQTT
    if (!isnan(temperatura)
     && !isnan(umidade)
     && !isnan(umidadeSolo)
     && !isnan(luminosidade)) {
      doc["umidade"] = umidade;
      doc["temperatura"] = temperatura;
      doc["umidadeSolo"] = umidadeSolo;
      doc["luminosidade"] = luminosidade;
      doc["timestamp"] = timestamp;
      doc["tempoEnvio"] = tempoEnvio;
      envioMqtt = true;
    } else {
      Serial.println("Leitura inválida do DHT no envio periódico");
    }

    if (envioMqtt) {
      mensagem = "";
      serializeJson(doc, mensagem);
      Serial.println(mensagem);
      client.publish(mqtt_topic_pub, mensagem.c_str());
      envioMqtt = false;
    }

    // LEDs
    fastShowLed();

    tempoAnterior = tempoAtual;
  }
}

void fastShowLedRainbow() {
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior > 12){
    for (int i = 0; i < 5; i++) {
      rainbow_hue[i] += 1; // Incrementa o tom de cor
      if (rainbow_hue[i] >= 255) rainbow_hue[i] = 0; // Reseta o tom se passar de 255
      leds[i] = CHSV(rainbow_hue[i], 255, 64); // Define a cor do LED
    }
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
    leds[20] = CRGB(8, 4, 0);
    leds[21] = CRGB(0, 0, 0);
    leds[22] = CRGB(0, 0, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 15 && temperatura < 20) {
    leds[20] = CRGB(8, 4, 0);
    leds[21] = CRGB(9, 3, 0);
    leds[22] = CRGB(0, 0, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 20 && temperatura < 25) {
    leds[20] = CRGB(8, 4, 0);
    leds[21] = CRGB(9, 3, 0);
    leds[22] = CRGB(10, 2, 0);
    leds[23] = CRGB(0, 0, 0);
    leds[24] = CRGB(0, 0, 0);

  } else if (temperatura >= 25 && temperatura < 30) {
    leds[20] = CRGB(8, 4, 0);
    leds[21] = CRGB(9, 3, 0);
    leds[22] = CRGB(10, 2, 0);
    leds[23] = CRGB(11, 1, 0);
    leds[24] = CRGB(0, 0, 0);
    
  } else if (temperatura >= 30) {
    leds[20] = CRGB(8, 4, 0);
    leds[21] = CRGB(9, 3, 0);
    leds[22] = CRGB(10, 2, 0);
    leds[23] = CRGB(11, 1, 0);
    leds[24] = CRGB(12, 0, 0);
  }

  // * Umidade do Ar
  if (umidade == 0) {
    leds[19] = CRGB(0, 0, 0);
    leds[18] = CRGB(0, 0, 0);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade > 0 && umidade < 20) {
    leds[19] = CRGB(0, 6, 6);
    leds[18] = CRGB(0, 0, 0);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 20 && umidade < 40) {
    leds[19] = CRGB(0, 6, 6);
    leds[18] = CRGB(0, 4, 7);
    leds[17] = CRGB(0, 0, 0);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 40 && umidade < 60) {
    leds[19] = CRGB(0, 6, 6);
    leds[18] = CRGB(0, 4, 7);
    leds[17] = CRGB(0, 3, 9);
    leds[16] = CRGB(0, 0, 0);
    leds[15] = CRGB(0, 0, 0);

  } else if (umidade >= 60 && umidade < 80) {
    leds[19] = CRGB(0, 6, 6);
    leds[18] = CRGB(0, 4, 7);
    leds[17] = CRGB(0, 3, 9);
    leds[16] = CRGB(0, 1, 10);
    leds[15] = CRGB(0, 0, 0);
    
  } else if (umidade >= 80) {
    leds[19] = CRGB(0, 6, 6);
    leds[18] = CRGB(0, 4, 7);
    leds[17] = CRGB(0, 3, 9);
    leds[16] = CRGB(0, 1, 10);
    leds[15] = CRGB(0, 0, 12);
  }

  // * Umidade do Solo
  if(umidadeSolo > 4000) {
    leds[10] = CRGB(0, 0, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 4000 && umidadeSolo > 3000) {
    leds[10] = CRGB(0, 12, 0);
    leds[11] = CRGB(0, 0, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 3000 && umidadeSolo > 2500) {
    leds[10] = CRGB(0, 12, 0);
    leds[11] = CRGB(1, 9, 0);
    leds[12] = CRGB(0, 0, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 2500 && umidadeSolo > 700) {
    leds[10] = CRGB(0, 12, 0);
    leds[11] = CRGB(1, 9, 0);
    leds[12] = CRGB(3, 9, 0);
    leds[13] = CRGB(0, 0, 0);
    leds[14] = CRGB(0, 0, 0);

  } else if (umidadeSolo <= 700 && umidadeSolo > 400) {
    leds[10] = CRGB(0, 12, 0);
    leds[11] = CRGB(1, 9, 0);
    leds[12] = CRGB(3, 9, 0);
    leds[13] = CRGB(4, 7, 0);
    leds[14] = CRGB(0, 0, 0);
    
  } else if (umidadeSolo <= 400) {
    leds[10] = CRGB(0, 12, 0);
    leds[11] = CRGB(1, 9, 0);
    leds[12] = CRGB(3, 9, 0);
    leds[13] = CRGB(4, 7, 0);
    leds[14] = CRGB(6, 6, 0);
    
  }

  // * Luminosidade
  if(luminosidade < 200){
    leds[9] = CRGB(0, 0, 0);
    leds[8] = CRGB(0, 0, 0);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);

  } else if(luminosidade >= 200 && luminosidade < 1000){
    leds[9] = CRGB(4, 0, 8);
    leds[8] = CRGB(0, 0, 0);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);

  } else if (luminosidade >= 1000 && luminosidade < 2000) {
    leds[9] = CRGB(4, 0, 8);
    leds[8] = CRGB(4, 0, 7);
    leds[7] = CRGB(0, 0, 0);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 2000 && luminosidade < 3400) {
    leds[9] = CRGB(4, 0, 8);
    leds[8] = CRGB(4, 0, 7);
    leds[7] = CRGB(5, 0, 7);
    leds[6] = CRGB(0, 0, 0);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 3400 && luminosidade < 3800) {
    leds[9] = CRGB(4, 0, 8);
    leds[8] = CRGB(4, 0, 7);
    leds[7] = CRGB(5, 0, 7);
    leds[6] = CRGB(5, 0, 6);
    leds[5] = CRGB(0, 0, 0);
    
  } else if (luminosidade >= 3800) {
    leds[9] = CRGB(4, 0, 8);
    leds[8] = CRGB(4, 0, 7);
    leds[7] = CRGB(5, 0, 7);
    leds[6] = CRGB(5, 0, 6);
    leds[5] = CRGB(6, 0, 6);
    
  }

  FastLED.show();
}

void microservoControl() {
  static unsigned long tempoAnterior = 0;
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior > 5000) {
    if (luminosidade < 1000) {
    // * Breu maximo
    servo.write(180);

    } else if (luminosidade >= 1000 && luminosidade < 2000) {
      // * Escurinho
      servo.write(135);

    } else if (luminosidade >= 2000 && luminosidade < 3400) {
      // * Sombra
      servo.write(90);

    } else if (luminosidade >= 3400 && luminosidade < 3800) {
      // * Ideal
      servo.write(45);

    } else if (luminosidade >= 3800) {
      // * Muita luz
      servo.write(5);

    }

    tempoAnterior = tempoAtual;
  }
}

void callback(char *topic, byte *payLoad, unsigned int length) {
  Serial.printf("Mensagem recebida em %s: ", topic);

  String mensagem = "";
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payLoad[i];
  }

  JsonDocument doc;
  deserializeJson(doc, mensagem.c_str());

  if (!doc["umidade"].isNull()) {
    umidade = doc["umidade"];
  }

  if (!doc["temperatura"].isNull()) {
    temperatura = doc["temperatura"];
  }

  if (!doc["umidadeSolo"].isNull()) {
    umidadeSolo = doc["umidadeSolo"];
  }

  if (!doc["luminosidade"].isNull()) {
    luminosidade = doc["luminosidade"];
  }

  if (!doc["timestamp"].isNull()) {
    timestamp = doc["timestamp"];
  }

  if (!doc["tempoEnvio"].isNull()) {
    tempoEnvio = doc["tempoEnvio"];
  }
}

void mqttConnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");

    if (client.connect(mqtt_id)) {
      Serial.println("Conectado com sucesso");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("Falha, rc=");
      Serial.println(client.state());
      Serial.println("Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}