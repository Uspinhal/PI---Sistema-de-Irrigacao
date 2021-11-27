/*
PROJETO INTEGRADOR - UNIVESP

PI VII - A Internet das Coisas na resolução de problemas da sociedade brasileira


SISTEMA DE IRRIGAÇÃO AUTOMÁTICO COM ATUALIZAÇÃO PELA PREVISÃO DO TEMPO

*/

// Inclusão de bibliotecas
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <ArduinoJson.h>

// Definição de constantes
#define SUN  0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4


// Definição de portas

int port = 5; // Selecionando a porta de saída

// Inicia o cliete de WiFi
WiFiClient client;

const char* ssid = "********";          // Nome da rede
const char* password = "*********";     // Senha da rede

unsigned long lastCallTime = 0;                     // last time you called the updateWeather function, in milliseconds
const unsigned long postingInterval = 60L * 1000L;  // delay between updates, in milliseconds

String APIKEY = "API_KEY";
String NameOfCity = "Nome da cidade";

void printWifiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void updateWeather(){
  // Verifica se há uma conexão
  if (client.connect("api.openweathermap.org", 80))
  {
    Serial.println("Conectando com Open Weather...");
    // enviando a requisição
    client.println("GET /data/2.5/weather?q=" + NameOfCity + "&units=metric&appid=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();


    DynamicJsonDocument doc(1024);
    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }


    String weatherId = doc["weather"][0]["main"];
    float weatherTemperature = doc["main"]["temp"].as<float>();
    int weatherHumidity = doc["main"]["humidity"].as<int>();

    client.stop();    


    // Imprime as respostas na porta serial
    Serial.println(F("Response:"));
    Serial.print("Weather: ");
    Serial.println(weatherId);
    Serial.print("Temperature: ");
    Serial.println(weatherTemperature);
    Serial.print("Humidity: ");
    Serial.println(weatherHumidity);
    Serial.println();
    
    // Aciona a irrigação se não estiver chovendo
    if (weatherId != "Rain")
    {
      // liga a irrigação por 10 minutos
      digitalWrite(port, HIGH);
      delay(600000); 
      digitalWrite(port, LOW);
    }
    
    
  }
  else
  {
    Serial.println("connection failed");
  }
  lastCallTime = millis();
}

void setup() {
  // put your setup code here, to run once:

  pinMode(port, OUTPUT);
  Serial.begin(9600);
  Serial.println("\n\nOnline Weather Display\n");

  Serial.println("Connecting to network");
  WiFi.begin(ssid, password);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);    
    if (++counter > 100) 
      ESP.restart();
    Serial.print( "." );
  }
  Serial.println("\nWiFi connected");
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - lastCallTime > postingInterval)
  {
    updateWeather();
  }
  
}

