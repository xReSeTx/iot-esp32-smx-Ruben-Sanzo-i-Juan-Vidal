void credits() {
  server.send(200, "text/html", "<h1>Fet per [El teu nom]</h1>");
}

void setup() {
  server.on("/credits", credits);
}
