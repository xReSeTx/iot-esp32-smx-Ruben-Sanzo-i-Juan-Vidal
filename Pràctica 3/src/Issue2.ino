#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "holaa";
const char* password = "s7z6ncfe";

WebServer server(80);

void enviarPaginaPrincipal() {
  server.send(200, "text/plain", "Hola, soc el teu ESP32");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  Serial.print("Connectant...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Connectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", enviarPaginaPrincipal);
  server.begin();
  Serial.println("Servidor iniciat!");
}

void loop() {
  server.handleClient();
}
