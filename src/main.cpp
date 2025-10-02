#include <M5Unified.h>  // M5Stack用ライブラリ
#include <Wire.h>           // I2C用ライブラリ
#include <Adafruit_BMP280.h> // BMP280用ライブラリ
#include <SensirionI2CSht4x.h>   // SHT4x用ライブラリ

Adafruit_BMP280 bmp;          // BMP280 @ 0x76
SensirionI2cSht4x sht4x;      // SHT40 @ 0x44

void setup() {
  auto cfg = M5.config(); 
  M5.begin(cfg);
  Serial.begin(115200);

  Wire.begin(32, 33);   // Core2のPortA(Grove)のI2Cピン

  M5.Display.setBrightness(200);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 10);
  M5.Display.println("ENV IV demo (SHT40 + BMP280)");

  // SHT40 初期化
  sht4x.begin(Wire, 0x44);

  // BMP280 初期化
  if (!bmp.begin(0x76)) {
    M5.Display.setCursor(10, 40);
    M5.Display.println("BMP280 not found!");
    Serial.println("ERROR: BMP280 not found (0x76).");
  }
}

void loop() {
  M5.update();

  float temperature, humidity;
  uint16_t error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    Serial.println("SHT40 read failed");
  }

  float press_hPa = bmp.readPressure() / 100.0f;

  M5.Display.fillRect(0, 60, 320, 80, TFT_BLACK);
  M5.Display.setCursor(10, 60);
  M5.Display.printf("Temp: %.2f C", temperature);
  M5.Display.setCursor(10, 85);
  M5.Display.printf("Humi: %.2f %%", humidity);
  M5.Display.setCursor(10, 110);
  M5.Display.printf("Pres: %.2f hPa", press_hPa);

  Serial.printf("T=%.2fC, RH=%.2f%%, P=%.2f hPa\n", temperature, humidity, press_hPa);

  delay(1000);
}
