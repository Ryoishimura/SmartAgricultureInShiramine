#include <M5Unified.h>  // M5Stack用ライブラリ
#include <Wire.h>           // I2C用ライブラリ
#include <Adafruit_BMP280.h> // BMP280用ライブラリ
#include <SensirionI2CSht4x.h>   // SHT4x用ライブラリ
#include <WiFi.h>         // WiFi用ライブラリ
#include <WebServer.h>  // Webサーバー用ライブラリ

// WiFi設定 
const char* ssid = "Wtf"; 
const char* password = "fb4e9c7d"; 

WebServer server(80); 

Adafruit_BMP280 bmp;          // BMP280 @ 0x76
SensirionI2cSht4x sht4x;      // SHT40 @ 0x44

// センサー値を格納するグローバル変数
float temperature = 0.0f; 
float humidity = 0.0f;
float press_hPa = 0.0f;

// Webページを生成する関数
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>ENV IV Sensor</title>"; 
  html += "<meta http-equiv='refresh' content='5'>"; // 5秒ごとに自動更新
  html += "<style>body{font-family:sans-serif;text-align:center;}h1{color:#007BFF;}</style>";
  html += "</head><body style=\"font-size: 200%;\">";
  html += "<h1>ENV IV Sensor Data</h1>";
  html += "<p>Temperature: " + String(temperature, 2) + " C</p>";
  html += "<p>Humidity: " + String(humidity, 2) + " %</p>";
  html += "<p>Pressure: " + String(press_hPa, 2) + " hPa</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

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

  // WiFi接続
  M5.Display.setCursor(10, 40);
  M5.Display.printf("Connecting to %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }
  M5.Display.fillScreen(TFT_BLACK); // 画面を一度クリア
  M5.Display.setCursor(10, 10);
  M5.Display.println("ENV IV demo (SHT40 + BMP280)");
  M5.Display.println("WiFi connected.");
  M5.Display.print("IP: ");
  M5.Display.println(WiFi.localIP());

  // Webサーバー設定
  server.on("/", handleRoot);
  server.begin();
  M5.Display.println("Web server started.");
  delay(30000); // IPアドレス確認のため30秒待機

  // SHT40 初期化
  sht4x.begin(Wire, 0x44);

  // BMP280 初期化
  if (!bmp.begin(0x76)) {
    M5.Display.setCursor(10, 130); // 表示位置を下に変更
    M5.Display.println("BMP280 not found!");
    Serial.println("ERROR: BMP280 not found (0x76).");
  }
}

void loop() {
  M5.update();
  server.handleClient(); // Webサーバーの処理

  static uint32_t last_update = 0;
  if (millis() - last_update < 1000) {
    return;
  }
  last_update = millis();

  uint16_t error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    Serial.println("SHT40 read failed");
    temperature = 0.0f; // 読み取り失敗時は値をリセット
    humidity = 0.0f;
  }

  press_hPa = bmp.readPressure() / 100.0f;

  M5.Display.fillRect(0, 60, 320, 80, TFT_BLACK);
  M5.Display.setCursor(10, 60);
  M5.Display.printf("Temp: %.2f C", temperature);
  M5.Display.setCursor(10, 85);
  M5.Display.printf("Humi: %.2f %%", humidity);
  M5.Display.setCursor(10, 110);
  M5.Display.printf("Pres: %.2f hPa", press_hPa);

  Serial.printf("T=%.2fC, RH=%.2f%%, P=%.2f hPa\n", temperature, humidity, press_hPa);
}
