#include <Arduino.h>

#define PINO_ANALOGICO 34 // Conectado na saída A0 do sensor
#define pinLed 2

void setup()
{

    Serial.begin(9600);
    pinMode(pinLed, OUTPUT);
}

void loop()
{
    // Leitura analógica
    int valorAnalogico = analogRead(PINO_ANALOGICO);

    Serial.print("Nível da água: ");
    Serial.println(valorAnalogico);

    if (valorAnalogico > 4000)
    {
        Serial.println("Terra extremamente seca");
        digitalWrite(pinLed, HIGH);
    }

    else if (valorAnalogico > 3000)
    {
        Serial.println("Terra seca");
        digitalWrite(pinLed, LOW);
    }

    else if (valorAnalogico > 2500)
    {
        Serial.println("Planta necessita de agua");
        digitalWrite(pinLed, LOW);
    }

    else if(valorAnalogico > 700)
    {
        Serial.println("Niveis de agua estaveis");
        digitalWrite(pinLed, LOW);
    }

    else if(valorAnalogico < 700)
    {
        Serial.println("Terra extremamente umida");
        digitalWrite(pinLed, HIGH);
    }

    delay(5000); // Espera 1 segundo
}
