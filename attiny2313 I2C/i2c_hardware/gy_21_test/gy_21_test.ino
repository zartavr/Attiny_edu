#include <GyverHTU21D.h>
GyverHTU21D htu;
void setup() {
  Serial.begin(9600);
  htu.begin();   // запустить датчик
}
void loop() {
  // функция опрашивает датчик по своему таймеру
  if (htu.readTick()) {
    // можно забирать значения здесь или в другом месте программы
    Serial.println(htu.getTemperature());
    Serial.println(htu.getHumidity());
    Serial.println();
  }
}
