#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "IFMG-Bambui-Aberta";
const char* password = "";    

const int switch_pin_1 = 13; 
const int switch_pin_2 = 14;  
const int switch_pin_3 = 15; 
const int led_pin = 16;

void setup() { //Configurações iniciais
  pinMode(switch_pin_1, INPUT_PULLUP);
  pinMode(switch_pin_2, INPUT_PULLUP);
  pinMode(switch_pin_3, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led_pin, HIGH);
    delay(300);
    digitalWrite(led_pin, LOW);
    delay(300);
  }
}

void loop() {
  //Faz a leitura das chaves de nível
  int switch_std_1 = digitalRead(switch_pin_1);
  int switch_std_2 = digitalRead(switch_pin_2);
  int switch_std_3 = digitalRead(switch_pin_3);

  //Essas variáveis são usadas para que não seja enviado mais de um alerta por vez.
  static bool level_1 = true;
  static bool level_2 = true;
  static bool level_3 = true;
  static int count = 0; //Está variável funciona como um contador de tentivas de reconexão.
 
  if (switch_std_1 == LOW && switch_std_2 == LOW && switch_std_3 == LOW){ //Caso: O reservatório está cheio

    level_1 = true;
    level_2 = true;
    level_3 = true;
    
  }else if (switch_std_1 == HIGH && switch_std_2 == LOW && switch_std_3 == LOW && level_1 == true){ //Caso: A chave mais alta está aberta

    HTTPClient http;
    http.begin("Adress"); //No caso Adress é a url contendo o caminho para a aplicaçao web
    http.GET();

    delay(2000);

    http.end();

    level_1 = false;

  }else if (switch_std_1 == HIGH && switch_std_2 == HIGH && switch_std_3 == LOW && level_2 == true){ // Caso: A chave alta e intermediária estão abertas

    HTTPClient http;
    http.begin("Adress"); //No caso Adress é a url contendo o caminho para a aplicaçao web
    http.GET();

    delay(2000);

    http.end();

    level_2 = false;

  }else if (switch_std_1 == HIGH && switch_std_2 == HIGH && switch_std_3 == HIGH && level_3 == true){ //Caso: As 3 chaves estão abertas
    HTTPClient http;
    http.begin("Adress"); //No caso Adress é a url contendo o caminho para a aplicaçao web
    http.GET();

    delay(2000);

    http.end();

    level_3 = false;
    
  }else{  //Caso: Don't care
    
    digitalWrite(led_pin, HIGH);
    delay(2000);
    digitalWrite(led_pin, LOW);
  }

  if (WiFi.status() != WL_CONNECTED) { //Se há alguns problema com a conexão wifi
    WiFi.disconnect();
    for (int i = 0; i < 32; i++) { //Entre desconectar e reconectar, é necessário colocar um intervalo de tempo, nesse intervalo de tempo o LED vermelho irá piscar a cada 300 milissegundos
      digitalWrite(led_pin, HIGH);
      delay(300);
      digitalWrite(led_pin, LOW);
      delay(300);
    }
    WiFi.reconnect();
    for (int i = 0; i < 32; i++) {
      digitalWrite(led_pin, HIGH);
      delay(300);
      digitalWrite(led_pin, LOW);
      delay(300);
    }
    count++;
  }

  if (count > 9) { //Quando o contador de tentativas de reconexão atingir 10, o ESP32 é reiniciado como outra forma de reestabelecer a conexão
    ESP.restart();
  }

  delay(2000); 
}
