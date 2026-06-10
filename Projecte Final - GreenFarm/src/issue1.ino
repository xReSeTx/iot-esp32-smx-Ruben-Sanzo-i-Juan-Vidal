

#include <WiFi.h>
#include <WebServer.h>

// ── CONFIGURACIÓ WiFi ─────────────────────────────
const char* ssid     = "TU_WIFI";
const char* password = "TU_PASSWORD";

// ── CONFIGURACIÓ DE PINS ──────────────────────────
#define LM35_PIN     34
#define BOMBA_PIN     5
#define LED_VERD     18
#define LED_VERMELL  19

// ── LLINDARS ─────────────────────────────────────
#define TEMP_MAX    20.0   // Si temperatura > 20°C → atura reg
#define TEMP_MIN    14.0   // Si temperatura < 14°C → activa reg
#define INTERVAL_MS 5000

// ── VARIABLES ────────────────────────────────────
bool regActiu = false;
unsigned long ultimaLectura = 0;
float temperaturaActual = 0.0;

WebServer server(80);

// ── HTML DE LA PÀGINA WEB ────────────────────────
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ca">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>GreenFarm IoT</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: sans-serif; background: #f4f6f4; color: #222; padding: 1.5rem; }
    h1 { font-size: 1.3rem; font-weight: 500; margin-bottom: 1.5rem; display: flex; align-items: center; gap: 8px; }
    .cards { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 12px; margin-bottom: 1.5rem; }
    .card { background: #fff; border-radius: 10px; border: 1px solid #e0e0e0; padding: 1rem; }
    .card-label { font-size: 12px; color: #888; margin-bottom: 6px; }
    .card-value { font-size: 26px; font-weight: 500; }
    .card-unit { font-size: 14px; font-weight: 400; color: #888; }
    .badge { display: inline-flex; align-items: center; gap: 5px; font-size: 13px; font-weight: 500; padding: 4px 10px; border-radius: 6px; }
    .badge-ok  { background: #e6f7f0; color: #0f6e56; }
    .badge-act { background: #fdecea; color: #a32d2d; }
    .status-row { display: flex; align-items: center; gap: 10px; background: #fff; border: 1px solid #e0e0e0; border-radius: 10px; padding: 0.85rem 1.1rem; margin-bottom: 0.75rem; font-size: 14px; }
    .dot { width: 10px; height: 10px; border-radius: 50%; flex-shrink: 0; }
    .label { flex: 1; color: #666; }
    .val { font-weight: 500; }
    .log-box { background: #fff; border: 1px solid #e0e0e0; border-radius: 10px; padding: 1rem; }
    .log-title { font-size: 12px; color: #888; margin-bottom: 8px; }
    .log-entry { display: flex; gap: 8px; font-size: 12px; padding: 4px 0; border-bottom: 1px solid #f0f0f0; }
    .log-entry:last-child { border-bottom: none; }
    .log-time { color: #aaa; width: 60px; flex-shrink: 0; }
    footer { margin-top: 1rem; font-size: 11px; color: #bbb; text-align: center; }
  </style>
</head>
<body>
  <h1>&#127807; GreenFarm IoT</h1>

  <div class="cards">
    <div class="card">
      <div class="card-label">Temperatura</div>
      <div class="card-value" id="temp">—<span class="card-unit">°C</span></div>
    </div>
    <div class="card">
      <div class="card-label">Estat del reg</div>
      <div id="reg-badge" class="badge badge-ok">&#9749; Aturat</div>
    </div>
    <div class="card">
      <div class="card-label">Llindar activació</div>
      <div class="card-value">14<span class="card-unit">°C</span></div>
    </div>
    <div class="card">
      <div class="card-label">Llindar desactivació</div>
      <div class="card-value">20<span class="card-unit">°C</span></div>
    </div>
  </div>

  <div class="status-row">
    <span class="dot" id="dot-verd"></span>
    <span class="label">LED verd (sistema OK)</span>
    <span class="val" id="led-verd-val">—</span>
  </div>
  <div class="status-row">
    <span class="dot" id="dot-vermell"></span>
    <span class="label">LED vermell (reg actiu)</span>
    <span class="val" id="led-vermell-val">—</span>
  </div>
  <div class="status-row">
    <span style="font-size:16px">&#9881;</span>
    <span class="label">Bomba / vàlvula (pin 5)</span>
    <span class="val" id="bomba-val">—</span>
  </div>

  <br>
  <div class="log-box">
    <div class="log-title">Registre d'esdeveniments</div>
    <div id="log"></div>
  </div>

  <footer>Actualització automàtica cada 5 s</footer>

  <script>
    const log = [];

    function addLog(msg, color) {
      const now = new Date().toTimeString().slice(0,8);
      log.unshift({ t: now, msg, color });
      if (log.length > 8) log.pop();
      const el = document.getElementById('log');
      el.innerHTML = log.map(e =>
        `<div class="log-entry"><span class="log-time">${e.t}</span><span style="color:${e.color}">${e.msg}</span></div>`
      ).join('');
    }

    function update() {
      fetch('/data')
        .then(r => r.json())
        .then(d => {
          document.getElementById('temp').innerHTML = d.temp.toFixed(1) + '<span class="card-unit">°C</span>';

          const badge = document.getElementById('reg-badge');
          if (d.reg) {
            badge.className = 'badge badge-act';
            badge.innerHTML = '&#128167; Actiu';
          } else {
            badge.className = 'badge badge-ok';
            badge.innerHTML = '&#9749; Aturat';
          }

          const dv = document.getElementById('dot-verd');
          const dr = document.getElementById('dot-vermell');
          dv.style.background = d.reg ? '#ccc' : '#1D9E75';
          dr.style.background = d.reg ? '#E24B4A' : '#ccc';
          document.getElementById('led-verd-val').textContent   = d.reg ? 'APAGAT' : 'ENCÈS';
          document.getElementById('led-vermell-val').textContent = d.reg ? 'ENCÈS'  : 'APAGAT';
          document.getElementById('bomba-val').textContent       = d.reg ? 'HIGH'   : 'LOW';

          if (d.event) addLog(d.event, d.reg ? '#a32d2d' : '#0f6e56');
          else addLog(`Lectura: ${d.temp.toFixed(1)}°C · Reg: ${d.reg ? 'ACTIU' : 'ATURAT'}`, '#888');
        });
    }

    update();
    setInterval(update, 5000);
  </script>
</body>
</html>
)rawliteral";

// ── FUNCIONS ──────────────────────────────────────
float llegirTemperatura() {
  int valorADC = analogRead(LM35_PIN);
  float voltatge = (valorADC / 4095.0) * 3.3;
  return voltatge * 100.0;
}

void activarReg(float temp) {
  regActiu = true;
  digitalWrite(BOMBA_PIN, HIGH);
  digitalWrite(LED_VERD, LOW);
  digitalWrite(LED_VERMELL, HIGH);
  Serial.println(">>> REG ACTIVAT <<<");
  Serial.print("Motiu: temperatura baixa (");
  Serial.print(temp); Serial.println(" °C)");
}

void aturarReg(float temp) {
  regActiu = false;
  digitalWrite(BOMBA_PIN, LOW);
  digitalWrite(LED_VERD, HIGH);
  digitalWrite(LED_VERMELL, LOW);
  Serial.println(">>> REG ATURAT <<<");
  Serial.print("Motiu: temperatura alta (");
  Serial.print(temp); Serial.println(" °C)");
}

// ── HANDLERS DEL SERVIDOR ─────────────────────────
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleData() {
  String event = "";
  if (temperaturaActual < TEMP_MIN && !regActiu)      event = ">>> REG ACTIVAT";
  else if (temperaturaActual > TEMP_MAX && regActiu)  event = ">>> REG ATURAT";

  String json = "{";
  json += "\"temp\":"  + String(temperaturaActual, 2) + ",";
  json += "\"reg\":"   + String(regActiu ? "true" : "false") + ",";
  json += "\"event\":\"" + event + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// ── SETUP ─────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(BOMBA_PIN, OUTPUT);
  pinMode(LED_VERD, OUTPUT);
  pinMode(LED_VERMELL, OUTPUT);
  digitalWrite(BOMBA_PIN, LOW);
  digitalWrite(LED_VERD, HIGH);
  digitalWrite(LED_VERMELL, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connectant a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnectat! IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor web actiu.");
}

// ── LOOP ──────────────────────────────────────────
void loop() {
  server.handleClient();

  unsigned long ara = millis();
  if (ara - ultimaLectura >= INTERVAL_MS) {
    ultimaLectura = ara;
    temperaturaActual = llegirTemperatura();

    Serial.println("-------------------------------");
    Serial.print("Temperatura : ");
    Serial.print(temperaturaActual);
    Serial.println(" °C");

    if (temperaturaActual < TEMP_MIN && !regActiu) {
      activarReg(temperaturaActual);
    } else if (temperaturaActual > TEMP_MAX && regActiu) {
      aturarReg(temperaturaActual);
    } else {
      Serial.print("Estat reg   : ");
      Serial.println(regActiu ? "ACTIU" : "ATURAT");
    }
  }
}
