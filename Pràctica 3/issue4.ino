#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "holaa";
const char* password = "s7z6ncfe";

const int pinLDR  = 35;
const int pinLM35 = 34;

WebServer server(80);

void enviarPaginaPrincipal() {
  int valorLDR = analogRead(pinLDR);
  float tempC = (analogRead(pinLM35) * 5.0 / 4095.0) * 100.0;

  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:sans-serif; text-align:center;} .card{padding:20px; color:white; display:inline-block; margin:10px; border-radius:10px;}</style>";
  html += "</head><body><h1>Monitor CPD</h1>";
  html += "<div class='card' style='background:gray;'><h3>Temp</h3><p>" + String(tempC, 1) + "°C</p></div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
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
