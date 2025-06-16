#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "internet.h"
#include <ezTime.h>

const int botaoSetinhaBaixo = 32;
const int botaoSetinhaCima = 27;
const int botaoSelecionar = 13;
int posicoesBotoes = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);


//* Borda Menu
byte cantoEsquerdo[] = {B10100, B10100, B10100, B10100, B10100, B10100, B10100, B10100};
byte cantoSuperiorEsquerdo[] = {B11111, B10000, B10111, B10100, B10100, B10100, B10100, B10100};
byte cantoSuperior[] = {B11111, B00000, B11111, B00000, B00000, B00000, B00000, B00000};
byte cantoSuperiorDireito[] = {B11111, B00001, B11101, B00101, B00101, B00101, B00101, B00101};
byte cantoDireito[] = {B00101, B00101, B00101, B00101, B00101, B00101, B00101, B00101};
byte cantoInferiorDireito[] = {B00101, B00101, B00101, B00101, B00101, B11101, B00001, B11111};
byte cantoInferior[] = {B00000, B00000, B00000, B00000, B00000, B11111, B00000, B11111};
byte cantoInferiorEsquerdo[] = {B10100, B10100, B10100, B10100, B10100, B10111, B10000, B11111};

#define esq 0
#define supEsq 1
#define sup 2
#define supDir 3
#define dir 4
#define infDir 5
#define inf 6
#define infEsq 7

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-FloralTech";
const char *mqtt_topic_sub = "senai134/mesa07/esp_infoSensores";
const char *mqtt_topic_pub = "senai134/mesa07/esp_pub";

int luminosidade, umidadeSolo;
float umidadeAr, temperatura;

Timezone tempoLocal;
unsigned long timestamp = 0;

void mqttConnect(void);
void callBack(char *topic, byte *payLoad, unsigned int length);
void bordaMenu();

void menuInicial();

void setup()
{
  Serial.begin(9600);

  conectaWiFi();

  lcd.init();
  lcd.backlight();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callBack);

  //* Borda Menu
  lcd.createChar(esq, cantoEsquerdo);
  lcd.createChar(supEsq, cantoSuperiorEsquerdo);
  lcd.createChar(sup, cantoSuperior);
  lcd.createChar(supDir, cantoSuperiorDireito);
  lcd.createChar(dir, cantoDireito);
  lcd.createChar(infDir, cantoInferiorDireito);
  lcd.createChar(inf, cantoInferior);
  lcd.createChar(infEsq, cantoInferiorEsquerdo);

  //*LCD
  menuInicial();
}

void loop()
{
  checkWiFi();

  if (!client.connected())
    mqttConnect();

  client.loop();

  //*Tratamento botoes + tela inicial
  static int valorAnteriorBaixo = 0;
  static int valorAnteriorCima = 0;
  static bool menu = true;
  static bool opcao = false;

  int valorSetinhaBaixo = touchRead(botaoSetinhaBaixo);
  int valorSetinhaCima = touchRead(botaoSetinhaCima);
  int valorSelecionar = touchRead(botaoSelecionar);

  if (valorSetinhaBaixo < 25 && valorAnteriorBaixo >= 25 && posicoesBotoes < 3 && menu == true)
  {
    lcd.setCursor(0, posicoesBotoes);
    lcd.print(" ");
    posicoesBotoes++;
    lcd.setCursor(0, posicoesBotoes);
    lcd.print(">");
  }
  valorAnteriorBaixo = valorSetinhaBaixo;

  if (valorSetinhaCima < 25 && valorAnteriorCima >= 25 && posicoesBotoes > 0 && menu == true)
  {
    lcd.setCursor(0, posicoesBotoes);
    lcd.print(" ");
    posicoesBotoes--;
    lcd.setCursor(0, posicoesBotoes);
    lcd.print(">");
  }
  valorAnteriorCima = valorSetinhaCima;

  static int valorSelecionarAnterior = 0;
  if (valorSelecionar < 25 && valorSelecionarAnterior >= 25)
  {
    menu = !menu;
    opcao = true;
    lcd.clear();
    if (menu == true)
      menuInicial();
  }
  valorSelecionarAnterior = valorSelecionar;

  //*paineis variaveis

  if (!menu)
  {
    unsigned long tempoAtual = millis();

     lcd.setCursor (2,2);
     tempoLocal.setTime (timestamp);
     lcd.print (tempoLocal.dateTime ("d/m/Y H:i"));

    switch (posicoesBotoes)
    {
    case 0:
    { // Luminosidade
      static unsigned long tempoAnterior0 = 0;
      static const int tempoEnvio0 = 1000;

      if (opcao)
      {
        bordaMenu();
        lcd.setCursor(4, 0);
        lcd.print("Luminosidade");
        lcd.setCursor(7, 1);
        lcd.print(">");
        opcao = false;
      }

      if (tempoAtual - tempoAnterior0 > tempoEnvio0)
      {
        lcd.setCursor(2, 1);
        lcd.print("    ");
        if (luminosidade < 1000 && luminosidade >= 100) lcd.setCursor (3,1);
        else if (luminosidade < 100 && luminosidade >= 10) lcd.setCursor (4,1);
        else if (luminosidade < 10) lcd.setCursor (5,1);
        else lcd.setCursor(2,1);
        lcd.print (luminosidade);

        lcd.setCursor(9, 1);
        if (luminosidade < 200)  
          lcd.print("Sem luz  ");
        else if (luminosidade >= 200 && luminosidade < 1000) 
          lcd.print("Breu     ");
        else if (luminosidade >= 1000 && luminosidade < 2000)
          lcd.print("Escurinho");
        else if (luminosidade >= 2000 && luminosidade < 3400)
          lcd.print("Sombra   ");
        else if (luminosidade >= 3400 && luminosidade < 3800)
          lcd.print("Adequada ");
        else
          lcd.print("Muita luz");

        tempoAnterior0 = tempoAtual;
      }
      break;
    }

    case 1:
    { // Umidade do ar
      static unsigned long tempoAnterior1 = 0;
      static const int tempoEnvio1 = 1000;

      if (opcao)
      {
        bordaMenu();
        lcd.setCursor(3, 0);
        lcd.print("Umidade do ar");

        lcd.setCursor(7, 1);
        lcd.print(">");
        opcao = false;
      }

      if (tempoAtual - tempoAnterior1 > tempoEnvio1)
      {
        lcd.setCursor(2, 1);
        lcd.print("    ");
        if (umidadeAr < 10)
        {
          lcd.setCursor (3,1);
          lcd.print (umidadeAr, 1);
        }
        if (umidadeAr < 100 && umidadeAr >= 10)
        {
          lcd.setCursor (2,1);
          lcd.print (umidadeAr, 1);
        }
        if (umidadeAr >= 100) 
        {
          lcd.setCursor (3,1);
          lcd.print (umidadeAr, 0);
        }
        

        lcd.setCursor(9, 1);
        if (umidadeAr < 20)      lcd.print("Ar seco ");
        else if (umidadeAr >= 20 && umidadeAr < 40) lcd.print("Baixa   ");
        else if (umidadeAr >= 40 && umidadeAr <= 70)lcd.print("Adequada");
        else if (umidadeAr > 70 && umidadeAr < 80) lcd.print("Alta    ");
        else                     lcd.print("Abafado ");

        tempoAnterior1 = tempoAtual;
      }
      break;
    }

    case 2:
    { // Umidade do solo
      static unsigned long tempoAnterior2 = 0;
      static const int tempoEnvio2 = 1000;

      if (opcao)
      {
        bordaMenu();
        lcd.setCursor(2, 0);
        lcd.print("Umidade do solo");

        lcd.setCursor(7, 1);
        lcd.print(">");
        opcao = false;
      }

      if (tempoAtual - tempoAnterior2 > tempoEnvio2)
      {
        lcd.setCursor(2, 1);
        lcd.print("    ");
        if (umidadeSolo < 1000 && umidadeSolo >= 100) lcd.setCursor (3,1);
        else if (umidadeSolo < 100 && umidadeSolo >= 10) lcd.setCursor (4,1);
        else if (umidadeSolo < 10) lcd.setCursor (5,1);
        else lcd.setCursor(2,1);
        lcd.print (umidadeSolo);

        lcd.setCursor(9, 1);
        if (umidadeSolo <= 400)
          lcd.print("Lama      ");
        else if (umidadeSolo <= 700 && umidadeSolo > 400 )
          lcd.print("Molhado   ");
        else if (umidadeSolo <= 2500 && umidadeSolo > 700)
          lcd.print("Adequada  ");
        else if (umidadeSolo <= 3000 && umidadeSolo > 2500)
          lcd.print("Terra seca");
        else
          lcd.print("Muito seco");

        tempoAnterior2 = tempoAtual;
      }
      break;
    }

    case 3:
    { // Temperatura
      static unsigned long tempoAnterior3 = 0;
      static const int tempoEnvio3 = 1000;

      if (opcao)
      {
        bordaMenu();
        lcd.setCursor(4, 0);
        lcd.print("Temperatura");

        lcd.setCursor(7, 1);
        lcd.print(">");
        opcao = false;
      }

      if (tempoAtual - tempoAnterior3 > tempoEnvio3)
      {
        lcd.setCursor(2, 1);
        lcd.print("    ");
        if (temperatura < 10)
        {
          lcd.setCursor (3,1);
          lcd.print (temperatura, 1);
        }
        else if (temperatura < 100 && temperatura >= 10)
        {
          lcd.setCursor (2,1);
          lcd.print (temperatura, 1);
        }
        else if (temperatura >= 100) 
        {
          lcd.setCursor (3,1);
          lcd.print (temperatura, 0);
        }

        lcd.setCursor(9, 1);
        if (temperatura < 10)
          lcd.print("Frio     ");
        else if (temperatura >= 10 && temperatura < 15)
          lcd.print("Fresco   ");
        else if (temperatura >= 15 && temperatura < 20)
          lcd.print("Agradável");
        else if (temperatura >= 20 && temperatura <= 30)
          lcd.print("Adequada ");
        else if (temperatura > 30 && temperatura < 35)
          lcd.print("Quente   ");
        else
          lcd.print("Forno    ");

        tempoAnterior3 = tempoAtual;
      }
      break;
    }
    }
  }
}

void callBack(char *topic, byte *payLoad, unsigned int length)
{
  Serial.printf("Mensagem recebida em %s: ", topic);

  String mensagem = "";
  for (unsigned int i = 0; i < length; i++)
  {
    mensagem += (char)payLoad[i];
  }
  Serial.println(mensagem);

  JsonDocument doc;
  deserializeJson(doc, mensagem);

  if (!doc["temperatura"].isNull())
  {
    temperatura = doc["temperatura"];
    Serial.print(temperatura);
  }

  if (!doc["umidadeSolo"].isNull())
  {
    umidadeSolo = doc["umidadeSolo"];
  }

  if (!doc["umidade"].isNull())
  {
    umidadeAr = doc["umidade"];
  }

  if (!doc["luminosidade"].isNull())
  {
    luminosidade = doc["luminosidade"];
  }
  if (!doc["timestamp"].isNull())
  {
    timestamp = doc["timestamp"];
  }
}

void mqttConnect()
{
  while (!client.connected())
  {
    Serial.println("Conectando ao MQTT...");

    if (client.connect(mqtt_id))
    {
      Serial.println("Conectado com sucesso!");
      client.subscribe(mqtt_topic_sub);
    }

    else
    {
      Serial.print("Falha, rc=");
      Serial.println(client.state());
      Serial.println("tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void menuInicial()
{

  //* Setinha inicial
  lcd.setCursor(0, posicoesBotoes);
  lcd.print(">");

  //* Tela inicial
  lcd.setCursor(2, 0);
  lcd.print("Luminosidade");

  lcd.setCursor(2, 1);
  lcd.print("Umidade do ar");

  lcd.setCursor(2, 2);
  lcd.print("Umidade do solo");

  lcd.setCursor(2, 3);
  lcd.print("Temperatura");
}

void bordaMenu()
{
  // Linha superior
  lcd.setCursor(0, 0);
  lcd.write(supEsq);
  for (int i = 0; i < 18; i++)
    lcd.write(sup);
  lcd.write(supDir);

  // Linhas do meio (laterais)
  for (int i = 1; i <= 2; i++)
  {
    lcd.setCursor(0, i);
    lcd.write(esq);
    lcd.setCursor(19, i);
    lcd.write(dir);
  }

  // Linha inferior
  lcd.setCursor(0, 3);
  lcd.write(infEsq);
  for (int i = 0; i < 18; i++)
    lcd.write(inf);
  lcd.write(infDir);
}