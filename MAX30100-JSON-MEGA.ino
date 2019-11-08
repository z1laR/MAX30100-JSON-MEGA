#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"

#include <Arduino.h>
#include <math.h>
#include <Wire.h>

#include "MAX30100.h"

MAX30100* pulseOxymeter;

#define IP "192.168.0.102"
#define PORT 3000

String postSaturacion = "";

RestClient client = RestClient(IP, PORT);

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Conectado a la red:");
  client.dhcp();

  Serial.print("IP de Placa Ethernet Shield:");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Listo :D!");

  pulseOxymeter = new MAX30100( DEFAULT_OPERATING_MODE, DEFAULT_SAMPLING_RATE, DEFAULT_LED_PULSE_WIDTH, DEFAULT_IR_LED_CURRENT, true, true );
  pulseOxymeter = new MAX30100();
  pinMode(3, OUTPUT);

  pulseOxymeter->printRegisters();
}

void loop()
{  
  pulseoxymeter_t result = pulseOxymeter->update();

  if( result.pulseDetected == true )
  {    
    Serial.println("");
    Serial.print("spO2: ");
    Serial.print(result.SaO2);
    Serial.print(" %");
    Serial.println("");
  }

  delay(10);

  digitalWrite( 3, !digitalRead(3) );

  client.setHeader("Content-Type: application/json");

  StaticJsonBuffer<200> jsonBuffer;
  char json[256];  
  JsonObject& root = jsonBuffer.createObject();
  root["spO2"] = result.SaO2;
  root.printTo(json, sizeof(json));
  Serial.println(json);

  int statusCode = client.post("http://192.168.0.102:3000/api/oximetria", json, &postSaturacion);

  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("postSaturacion body from server: ");
  Serial.println(postSaturacion);

  postSaturacion = "";
}
