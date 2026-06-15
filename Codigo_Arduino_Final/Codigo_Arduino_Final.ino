#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h> // Sem isso não conversa com o ESP

#define SS_PIN 10
#define RST_PIN 9
#define LED_R 7 // LED Vermelho
#define LED_G 5 // LED Verde
#define LED_Y 6 // LED Amarelo 

char st[20];
MFRC522 mfrc522(SS_PIN, RST_PIN);


// Pino 2 do arduino = RX
// Pino 3 do arduino = TX 
// ou (RX,TX)
SoftwareSerial comunicacaoESP(2, 3);

// Para piscar
unsigned long tempoAnterior = 0;
const long intervaloPisca = 500; 
bool estadoLedAmarelo = LOW;

void setup()
{
  Serial.begin(9600);
  comunicacaoESP.begin(9600); // Frequencia do ESP

  SPI.begin();
  mfrc522.PCD_Init();

  delay(10);
  mfrc522.PCD_DumpVersionToSerial(); // para ver ser realmente esta funcionando o leitor, caso não leia tem algo errado.
  
  Serial.println("Aproxime o seu cartao/TAG do leitor");
  Serial.println();
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);

  
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
}

void loop()
{
  
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);

  
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervaloPisca) 
  {
    tempoAnterior = tempoAtual;
    estadoLedAmarelo = !estadoLedAmarelo;
    digitalWrite(LED_Y, estadoLedAmarelo);
  }

  
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return; 
  }
  
  
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  digitalWrite(LED_Y, LOW);

  Serial.print("ID da tag:");
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();
  
  if (conteudo.substring(1) == "5A D4 5B 32") // CARTÃO AUTORIZADO
  {
    Serial.println("Acesso autorizado !");
    Serial.println();
    
    
    comunicacaoESP.println("🟢 Acesso AUTORIZADO! ID: " + conteudo);
    
    digitalWrite(LED_G, HIGH); 
    digitalWrite(LED_R, LOW);  
    
    delay(5000); 
    
    digitalWrite(LED_G, LOW); 
  }
  else // CARTÃO RECUSADO
  {
    Serial.println("Acesso negado !");
    Serial.println();
    
    comunicacaoESP.println("🔴 Acesso NEGADO! ID: " + conteudo);
    
    digitalWrite(LED_R, HIGH); 
    digitalWrite(LED_G, LOW);  
    
    delay(3000); 
    
    digitalWrite(LED_R, LOW); 
  }
}