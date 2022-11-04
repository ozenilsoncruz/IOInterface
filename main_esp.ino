#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Nome do ESP na rede
const char* host = "ESP-10.0.0.109";

// Definições de rede
IPAddress local_IP(10, 0, 0, 109);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

void config_connect(){
  // inicializacao do nodemcu com o modulo wifi
  Serial.begin(115200);
  Serial.println("Booting");

  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // realiza a configuracao inicial para conexao via wifi com nodemcu
  config_connect();
  
  Serial.begin(9600);

  // definicao dos pinos
  pinMode(LED_BUILTIN, OUTPUT);  
}

void loop() {
  ArduinoOTA.handle();

  //int n_bytes = Serial.availableForWrite(); //Retorna o número de bytes (caracteres) livres no buffer de transmissão serial que podem ser ocupados sem bloquear a operação de trasnmissão.
  
  if(Serial.available() > 0){ // Retorna o número de bytes (caracteres) disponíveis para leitura da porta serial.
    String msg = Serial.readString(); // Le uma String
    if(msg == "3"){
      Serial.write("1F");
    }else if(msg == "4"){
      Serial.write(analogRead(A0) * (3.3/1023.0));
    }else if(msg == "5"){
      Serial.write(digitalRead(D0));
    }else if(msg == "6"){
      //Serial.write(10);
      if(digitalRead(LED_BUILTIN) == HIGH){
        digitalWrite(LED_BUILTIN, LOW);
      }else{
        digitalWrite(LED_BUILTIN, HIGH);
      }
      int bytesSent = Serial.write("TESTE");
    }else {
      // pisco o led caso a mensagem nao seja reconhecida
      for(int i=0; i<100; i++){
        digitalWrite(LED_BUILTIN,LOW);
        delay(200);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(200);
      }
    }
  } 
}