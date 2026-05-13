#include <WiFi.h>
#include <ESPmDNS.h>

void setup() {
  WiFi.begin("InovaIndustria","industria50");
  while (WiFi.status()!=WL_CONNECTED) delay(100);

  // Tenta listen até conseguir
  Serial.begin(115200);
  Serial.println("[IP] is " + String(WiFi.localIP().toString()));

  MDNS.begin("esp32name");          // acessa em esp32name.local
}
void loop() {
}
