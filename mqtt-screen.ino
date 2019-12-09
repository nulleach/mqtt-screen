#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());

int  delayMS = 30000;  // Задаержка в мс между публикацией сообщений
long lastMsg = 0;      // Время публикации предыдущего сообщения  (мс) 
int  value = 0;        // Переменная для формирования публикуемого сообщения


int pinCS = D8;
int numberOfHorizontalDisplays = 1; // у нас по горизонтали 1 матрица
int numberOfVerticalDisplays = 4;   // зато по вертикали 4, и чтобы получить строку - нужно будет повернуть изображение на 90 градусов

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String tape = "Internet of Thinks"; // текст, который будет плыть
int wait = 20;                      // время между крайними перемещениями букв

int spacer = 1;                     // расстояние между буквами
int width = 5 + spacer;             // размер шрифта




// Функция обработки входящих сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  // Печать информации о полученном сообщенийй
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  tape="";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    tape+=(char)payload[i];
  }
  Serial.println();
}


void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);


   matrix.setIntensity(7); // яркость
  matrix.setRotation(1);  // поворачиваем матрицу на 90 градусов
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();





 for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {
    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // центр строки по вертикали

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }

    matrix.write();
    delay(wait);
  }


  
}
