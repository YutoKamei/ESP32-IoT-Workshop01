// 講師用ひな形：このままでは送信できません。
// Wi-Fi情報・TLSルートCA・サーバURL・必要な認証方法を講師が設定してください。
// 長期利用する秘密情報を公開GitHubにコミットしないでください。

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const int SENSOR_PIN = 17;
const char* DEVICE_ID = "group01";
const unsigned long SEND_INTERVAL_MS = 5000;

// 以下は研修の設定に応じて講師が安全に用意すること。
const char* WIFI_SSID = "REPLACE_BY_INSTRUCTOR";
const char* WIFI_PASSWORD = "REPLACE_BY_INSTRUCTOR";
const char* API_URL = "https://example.invalid/measurements";
// 接続先の証明書を検証するためのルートCA（PEM形式）。
const char* ROOT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
REPLACE_WITH_VALID_CA_CERTIFICATE
-----END CERTIFICATE-----
)EOF";

OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
unsigned long lastSentAt = 0;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting Wi-Fi");
  unsigned long started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) Serial.println("Wi-Fi connected");
  else Serial.println("Wi-Fi not connected");
}

void loop() {
  if (millis() - lastSentAt < SEND_INTERVAL_MS) return;
  lastSentAt = millis();

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  if (temperature == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor error");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected - skip sending");
    WiFi.reconnect();
    return;
  }

  WiFiClientSecure client;
  client.setCACert(ROOT_CA); // TLS証明書の検証を必須にする
  HTTPClient http;
  if (!http.begin(client, API_URL)) {
    Serial.println("HTTPS setup failed");
    return;
  }
  http.addHeader("Content-Type", "application/json");
  // 認証・認可は研修専用APIで適切に実装すること（この雛形には未実装）。
  String payload = String("{\"deviceId\":\"") + DEVICE_ID +
                   "\",\"temperature\":" + String(temperature, 2) + "}";
  int status = http.POST(payload);
  Serial.print("HTTP status: ");
  Serial.println(status);
  http.end();
}
