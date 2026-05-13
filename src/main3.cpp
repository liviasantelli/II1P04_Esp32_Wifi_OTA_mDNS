#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

const char *ssid = "InovaIndustria";
const char *password = "industria50";
const char *hostName = "esp32name";

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(100);
  WiFi.setHostname(hostName);

  // Tenta listen até conseguir
  Serial.begin(115200);
  Serial.println("[IP] is " + String(WiFi.localIP().toString()));

  if (!MDNS.begin(hostName)) Serial.println("[mDNS] begin failed");
  else {
    // Também pode anunciar o canal UDP usado pelo LasecPlot
    MDNS.addService("wserial", "udp", 47268);
    Serial.println("[mDNS] begin in " + String(hostName));
  }

  ArduinoOTA
      .onStart([]() {Serial.println("[OTA] Start");})
      .onEnd([]() {Serial.println("[OTA] End"); })
      .onProgress([](unsigned int p, unsigned int t) {Serial.println("[OTA] " + String((p*100)/t));})
      .onError([](ota_error_t e) { Serial.println("[OTA] Error " + String(e)); })
      .setHostname(hostName)
      .begin();
}

void loop() {
  ArduinoOTA.handle();
  Serial.flush();

  static float t = 0.0f;      // variável de tempo para o seno
  static uint32_t lastRetry1 = 0;
  if (millis() - lastRetry1 > 100) {
    lastRetry1 = millis();
    Serial.println("[PLOT] seno: " + String(sin(t)));   // envia para o gráfico
    t += 0.2f;                      // incrementa o tempo (ajuste a velocidade)
    if (t > 2 * M_PI) t = 0;        // reinicia o ciclo a cada 2π
  }

  static float tReta = 0.0f;      // variável de tempo para a reta
  static uint32_t lastRetry2 = 0;
  if (millis() - lastRetry2 > 200) {
    lastRetry2 = millis();
    Serial.println("[PLOT] reta: " + String(10*tReta));
    tReta += 0.2f;                      // incrementa o tempo (ajuste a velocidade)
  }
}
