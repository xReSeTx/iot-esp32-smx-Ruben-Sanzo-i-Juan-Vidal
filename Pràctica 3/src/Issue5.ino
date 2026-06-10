void enviarPaginaPrincipal() {
  int valorLDR = analogRead(pinLDR);
  float tempC = (analogRead(pinLM35) * 5.0 / 4095.0) * 100.0;

  String colorTemp = "#28a745";
  if (tempC > 24.0) {
    colorTemp = "#dc3545";
  }

  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:sans-serif;text-align:center;}";
  html += ".card{padding:20px;color:white;display:inline-block;margin:10px;border-radius:10px;}</style>";
  html += "</head><body><h1>Monitor CPD</h1>";
  html += "<div class='card' style='background:" + colorTemp + ";'><h3>Temp</h3><p>" + String(tempC, 1) + " °C</p></div>";
  html += "<div class='card' style='background:gray;'><h3>Llum</h3><p>" + String(valorLDR) + "</p></div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
