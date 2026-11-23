// 引入库
#include "DHT.h"

#define DHTPIN 2     // ⚠️ 确保 DHT11 的数据线接在 D2，千万不要接 D13
#define DHTTYPE DHT11   // 传感器型号

// ================= [改动1 位置：使用板载灯] =================
#define ONBOARD_LED LED_BUILTIN // 使用 Arduino 自带的 L 灯（即 D13 引脚）
unsigned long previousMillis = 0; 
int blinkInterval = 1000;         
bool ledState = LOW;              
unsigned long lastReadTime = 0;   
// =========================================================

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  // 设置板载灯引脚为输出模式
  pinMode(ONBOARD_LED, OUTPUT); 

  dht.begin();
}

void loop() {
  unsigned long currentMillis = millis(); 

  // 每2秒读取一次传感器
  if (currentMillis - lastReadTime >= 2000) {
    lastReadTime = currentMillis; 

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    } else {
      // ================= [改动1 位置：温度映射为闪烁间隔] =================
      // 温度 15度~35度，映射为闪烁间隔 1000ms(慢) ~ 200ms(快)
      blinkInterval = map(t, 15, 35, 1000, 200);
      blinkInterval = constrain(blinkInterval, 100, 2000); 
      // =================================================================
      
      float hif = dht.computeHeatIndex(f, h);
      float hic = dht.computeHeatIndex(t, h, false);

      // ================= [改动2 位置：增加组号] =================
      Serial.print(F("Group: 侯洁 潘采薇 "));  
      // ========================================================

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print(f);
      Serial.print(F("°F  Heat index: "));
      Serial.print(hic);
      Serial.print(F("°C "));
      Serial.print(hif);
      Serial.println(F("°F"));
    }
  }

  // ================= [改动1 位置：LED非阻塞闪烁逻辑] =================
  // 根据计算出的 blinkInterval 控制板载 L 灯闪烁
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    ledState = !ledState; 
    digitalWrite(ONBOARD_LED, ledState); // 这里替换成了 ONBOARD_LED
  }
  // =================================================================
}