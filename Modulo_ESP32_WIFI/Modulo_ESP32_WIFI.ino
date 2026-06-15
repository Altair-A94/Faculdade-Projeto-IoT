#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h" 

// Wi-Fi Aqui
const char* ssid = "aqui";
const char* password = "Senha aqui";

#define RX2_PIN 16
#define TX2_PIN 17

// Fuso horario
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800; 
const int   daylightOffset_sec = 0; 

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  
  Serial.println();
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Conectado!");
  
  // Sincronia
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Relógio sincronizado com a internet!");
  
  Serial.println("[ESP32] Aguardando o cartão...");
}

void loop() {
  if (Serial2.available() > 0) {
    
    String mensagemArduino = Serial2.readStringUntil('\n');
    mensagemArduino.trim(); 
    
    if (mensagemArduino.length() > 0 && WiFi.status() == WL_CONNECTED) {
      
      struct tm timeinfo;
      String dataHora = "";
      
      if(!getLocalTime(&timeinfo)){
        dataHora = "[Sem Hora]";
      } else {
        char bufferTempo[50];
        strftime(bufferTempo, sizeof(bufferTempo), "%d/%m/%Y %H:%M:%S", &timeinfo);
        dataHora = String(bufferTempo);
      }

      String mensagemFinal = mensagemArduino + " - Tempo: " + dataHora;
      
      Serial.print("Enviando via Wi-Fi: ");
      Serial.println(mensagemFinal);
      
      HTTPClient http;
      

      String urlDestino = "https://webhook.site/8777638f-cd98-4072-af07-17d8d9d4600c";
      
      http.begin(urlDestino);      
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
      

      http.addHeader("Content-Type", "text/plain"); 
      

      int codigoResposta = http.POST(mensagemFinal); 
      
      if (codigoResposta > 0) {
        Serial.print("Sucesso! Codigo HTTP: ");
        Serial.println(codigoResposta); // se for 200 perfeito, qualquer outra coisa é erro
      } else {
        Serial.print("Erro HTTP: ");
        Serial.println(codigoResposta);
      }
      
      http.end(); 
    } 
  }
}