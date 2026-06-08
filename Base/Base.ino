#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_R 7 // LED Vermelho
#define LED_G 5 // LED Verde
#define LED_Y 6 // LED Amarelo 

char st[20];
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Variáveis para o controle do LED amarelo piscante
unsigned long tempoAnterior = 0;
const long intervaloPisca = 500; 
bool estadoLedAmarelo = LOW;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println("Aproxime o seu cartao/TAG do leitor");
  Serial.println();
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);

  // Garante que todos os LEDs comecem apagados
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
}

void loop()
{
  // No estado de espera, garante que vermelho e verde estejam apagados
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);

  // Lógica para piscar o LED Amarelo aguardando leitura
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervaloPisca) 
  {
    tempoAnterior = tempoAtual;
    estadoLedAmarelo = !estadoLedAmarelo;
    digitalWrite(LED_Y, estadoLedAmarelo);
  }

  // Busca novos cartões 
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return; 
  }
  
  // Seleciona um cartão a ser lido
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  // Cartão detectado! Apaga o LED amarelo para processar a leitura
  digitalWrite(LED_Y, LOW);

  // Mostra ID na serial
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
  
  if (conteudo.substring(1) == "D6 35 19 7E") // ID CARTÃO AUTORIZADO
  {
    Serial.println("Acesso autorizado !");
    Serial.println();
    
    digitalWrite(LED_G, HIGH); // Acende o Verde
    digitalWrite(LED_R, LOW);  // Garante que o Vermelho está apagado
    
    delay(5000); // Mantém o LED Verde aceso por 5 segundos
    
    digitalWrite(LED_G, LOW); // Apaga o Verde para voltar ao início
  }
  else // CARTÃO RECUSADO
  {
    Serial.println("Acesso negado !");
    Serial.println();
    
    digitalWrite(LED_R, HIGH); // Acende o Vermelho
    digitalWrite(LED_G, LOW);  // Garante que o Verde está apagado
    
    delay(3000); // Mantém o LED Vermelho aceso por 3 segundos indicando o erro
    
    digitalWrite(LED_R, LOW); // Apaga o Vermelho para voltar ao início
  }
}