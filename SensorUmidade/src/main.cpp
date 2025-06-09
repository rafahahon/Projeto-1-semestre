#include <Arduino.h>

#define pinAnalogico 34 // saída A0 do sensor
#define pinLed 2

void setup()
{

    Serial.begin(9600);
    pinMode(pinLed, OUTPUT);
}

void loop()
{
    // Leitura analógica
    int valorAnalogico = analogRead(pinAnalogico);

    Serial.print("Nível da água: ");
    Serial.println(valorAnalogico);

    if (valorAnalogico > 4000)
    {
        Serial.println("Terra extremamente seca");
    }

    else if (valorAnalogico > 3000)
    {
        Serial.println("Terra seca");
    }

    else if (valorAnalogico > 2500)
    {
        Serial.println("Planta necessita de água");
    }

    else if(valorAnalogico > 700)
    {
        Serial.println("Níveis de água estáveis");
    }

    else if(valorAnalogico < 700)
    {
        Serial.println("Terra extremamente úmida");
    }

    delay(5000); 
}
