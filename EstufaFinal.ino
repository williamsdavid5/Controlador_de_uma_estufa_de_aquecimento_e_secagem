#include <DHT.h>;
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Btn.h>

//configurações do dht
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temp;  //Armazena valores de temperatura
float temperaturaIdeal = 30.0;

//condfigurações do display
#define endereco 0x27  // Endereços comuns: 0x27, 0x3F. endereço de memoria dos display no arduino, 0x27 é o endereço comum para UNO
#define colunas 16
#define linhas 2
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

#define potenciometro A1
bool potenciometroLigado = false;

#define RELE 3

Btn btn = new Btn(5);
#define ledBotao 4

//função que é chamada quando o botão é clicado
void botaoClicado() {
  potenciometroLigado = !potenciometroLigado; //o valor alterna entre true e false a cada clique
}

void mostrarTemp() {
  lcd.setCursor(12, 0);
  lcd.print("    ");
  lcd.setCursor(12, 0);
  lcd.print(temp);

  lcd.setCursor(12, 1);
  lcd.print("    ");
  lcd.setCursor(12, 1);
  lcd.print(temperaturaIdeal);

  delay(200);

}

void setup() {
  Serial.begin(9600);
  pinMode(ledBotao, OUTPUT);
  pinMode(RELE, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  lcd.init();       // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight();  // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear();      // LIMPA O DISPLAY

  //essas strings serão fixas na tela
  //apenas os valores de temperatura serão atualizados no loop
  lcd.print("Temp. atual:");
  lcd.setCursor(0, 1);
  lcd.print("Temp. ideal:");
}

unsigned long ultimoTempoAcao = 0;
unsigned long intervaloAcao = 30000;  // Intervalo de 30 segundos

bool estadoRelay = false;
bool aguardandoAcao = true;

void loop() {
  temp = dht.readTemperature();
  btn.clique(botaoClicado);
  Serial.print(temp);

  if (potenciometroLigado == true) {
    digitalWrite(ledBotao, HIGH);
    temperaturaIdeal = map(analogRead(potenciometro), 0, 1024, 0, 60);
  } else {
    digitalWrite(ledBotao, LOW);
  }

  // Verifica se a temperatura está dentro do intervalo aceitável
  if (temp > 15 && temp < 60) {
    mostrarTemp(); // Atualiza a tela com a temperatura em tempo real

    unsigned long tempoAtual = millis();

    if (temp < temperaturaIdeal) {
      if (aguardandoAcao && tempoAtual - ultimoTempoAcao >= intervaloAcao) {
        digitalWrite(RELE, HIGH);
        digitalWrite(13, HIGH);
        estadoRelay = true;
        aguardandoAcao = false;
        ultimoTempoAcao = tempoAtual;
      } else if (!aguardandoAcao && tempoAtual - ultimoTempoAcao >= intervaloAcao) {
        digitalWrite(RELE, LOW);
        digitalWrite(13, LOW);
        estadoRelay = false;
        aguardandoAcao = true;
        ultimoTempoAcao = tempoAtual;
      }
    } else {
      digitalWrite(RELE, LOW);
      digitalWrite(13, LOW);
      estadoRelay = false;
      aguardandoAcao = true;
    }
  } else {
    digitalWrite(RELE, LOW);
    digitalWrite(13, LOW);
    lcd.clear();
    lcd.print("Algo está errado!");
    delay(200);
  }
}
