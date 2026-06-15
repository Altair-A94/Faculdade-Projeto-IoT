#include "BluetoothSerial.h"

// Verifica se o Bluetooth está ativado no chip
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#define RX2_PIN 16
#define TX2_PIN 17

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  
  // O nome que vai aparecer no seu telemóvel quando for parear
  SerialBT.begin("ESP32"); 
  
  Serial.println("Bluetooth Iniciado!");
  Serial.println("Vá ao seu celular e pareie com o dispositivo 'ESP32'");
}

void loop() {
  // Verifica se o Arduino enviou alguma coisa
  if (Serial2.available() > 0) {
    
    String mensagemArduino = Serial2.readStringUntil('\n');
    mensagemArduino.trim(); 
    
    // Se a mensagem não estiver vazia, envia para o Bluetooth!
    if (mensagemArduino.length() > 0) {
      Serial.print("Repassando para o Bluetooth: ");
      Serial.println(mensagemArduino);
      
      // Comando mágico que manda a notificação para o celular conectado:
      SerialBT.println(mensagemArduino); 
    }
  }
}