#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>

#define pin_thermometre 32
OneWire oneWire(pin_thermometre);
DallasTemperature sensors(&oneWire);

int pin_relai[8]={19,18,5,17,16,4,0,2};


#define pin_lumiere pin_relai[0]

#define pin_mangeoire pin_relai[1]

#define PH_PIN 33
float voltage,PH;
DFRobot_PH ph;


const char *ssid = "";
const char *password = "";

int etatLumiere;
float temperature;
int repas;

String Heure;
String Minute;
bool repasOk=false;
int lumiereManuelle=0;
int lumiereTempo=0;
AsyncWebServer server(80);

IPAddress local_IP(192,168,1,28);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(192,168,1,1);
IPAddress secondaryDNS(192,168,1,1);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void RecupererTempsServeur(){
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  char HeureServeur[3];
  strftime(HeureServeur,3, "%H", &timeinfo);
  Heure=HeureServeur;

  char MinuteServeur[3];
  strftime(MinuteServeur,3, "%M", &timeinfo);
  Minute=MinuteServeur;
}

void ProgrammateurLumiere(){
  if(Heure.toInt()>8&&Heure.toInt()<17){
    lumiereTempo=true;
    }
    else{
      lumiereTempo=false;
      }
}

void ProgrammateurMangeoire(){
  if(Heure.toInt()==9&&Minute.toInt()==15){
    if(repasOk==false){
        repas=0;
        digitalWrite(pin_mangeoire,LOW);
        delay(1000);
        digitalWrite(pin_mangeoire,HIGH);
        repasOk=true;
        repas++; 
      }
  }  
  
  else if(Heure.toInt()==15){
    if(repasOk==false){
        digitalWrite(pin_mangeoire,LOW);
        delay(1000);
        digitalWrite(pin_mangeoire,HIGH);
        repasOk=true; 
        repas++;
      }
  }
  else{
    repasOk=false;
  }
}

void gererLumiere(){
  
  if(lumiereManuelle==1||lumiereTempo==1){
    digitalWrite(pin_lumiere,LOW);
    etatLumiere=0;
  }
  else{
    digitalWrite(pin_lumiere,HIGH);
    etatLumiere=1;
  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void RecupererPH(){
      static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        voltage=(analogRead(PH_PIN));
        
        PH = mapfloat(voltage, 0, 4043, 0, 14);  // convert voltage to pH with temperature compensation
        
        Serial.print("Voltage:");
        Serial.print(voltage);
        Serial.print(" temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(PH);
    }
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
}

void setup()
{
  ph.begin();
  pinMode(pin_lumiere,OUTPUT);
  digitalWrite(pin_lumiere,HIGH);
   
  pinMode(pin_mangeoire,OUTPUT);
  digitalWrite(pin_mangeoire,HIGH);
  
  
  //----------------------------------------------------Serial
  Serial.begin(115200);
  Serial.println("\n");
  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
    if(!WiFi.config(local_IP,gateway,subnet,primaryDNS,secondaryDNS))
  {
    Serial.println("Wifi failed");
  }
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/fish-solid.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/fish-solid.png", "image/png");
  });
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/jquery.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/jquery.js", "text/javascript");
  });

  server.on("/lumiere", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      lumiereManuelle= !lumiereManuelle; 

     if(lumiereTempo==1){
     request->send(200,"text/plain","0");
     }
     else{
      request->send(200,"text/plain",String(!etatLumiere));
     }
      
   });

   server.on("/etats", HTTP_GET, [](AsyncWebServerRequest *request)
  {    
    etatLumiere=digitalRead(pin_lumiere);
    
    sensors.requestTemperatures();
    temperature=sensors.getTempCByIndex(0);
   
    request->send(200,"text/json","{\"lumiere\":"+String(etatLumiere)+",\"ph\" : "+String(PH)+" ,\"temperature\" : "+String(temperature,0)+", \"repas\" :"+String(repas)+"}");
   });
   
   server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
  {    
    sensors.requestTemperatures();
    temperature=sensors.getTempCByIndex(0);
    request->send(200, "text/plain",String(temperature,0));
   });


  server.on("/mangeoire", HTTP_GET, [](AsyncWebServerRequest *request)
  {
   digitalWrite(pin_mangeoire,LOW);
   delay(1000);
   digitalWrite(pin_mangeoire,HIGH);
   repas++;
   request->send(200, "text/plain",String(repas));
  });
  
  server.begin();
  Serial.println("Serveur actif!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
RecupererTempsServeur();
ProgrammateurLumiere();
ProgrammateurMangeoire();
gererLumiere();
RecupererPH();
delay(1000);
}
