/*
  GreenFarm IoT - Sistema de Reg Intel·ligent
  Client: Granja GreenFarm
  Hardware: ESP32 + Sensor LM35
  Autor: IoT Projects Lab
*/

// ── CONFIGURACIÓ DE PINS ──────────────────────────
#define LM35_PIN    34     // Pin analògic on connectes el LM35 (usa 34, 35, 36 o 39 en ESP32)
#define BOMBA_PIN    5     // Pin que controla la bomba/vàlvula de reg (relé)
#define LED_VERD    18     // LED verd = sistema OK
#define LED_VERMELL 19     // LED vermell = reg actiu

// ── LLINDARS ─────────────────────────────────────
#define TEMP_MAX    28.0   // Si temperatura > 28°C → activa reg
#define TEMP_MIN    20.0   // Si temperatura < 20°C → atura reg
#define INTERVAL_MS 5000   // Llegim el sensor cada 5 segons

// ── VARIABLES ────────────────────────────────────
bool regActiu = false;
unsigned long ultimaLectura = 0;

// ─────────────────────────────────────────────────
float llegirTemperatura() {
  int valorADC = analogRead(LM35_PIN);
  // L'ESP32 té ADC de 12 bits (0-4095) i referència de 3.3V
  // El LM35 dona 10mV per cada °C
  float voltatge = (valorADC / 4095.0) * 3.3;
  float temperatura = voltatge * 100.0;
  return temperatura;
}

// ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(BOMBA_PIN, OUTPUT);
  pinMode(LED_VERD, OUTPUT);
  pinMode(LED_VERMELL, OUTPUT);

  // Estat inicial: bomba apagada
  digitalWrite(BOMBA_PIN, LOW);
  digitalWrite(LED_VERD, HIGH);
  digitalWrite(LED_VERMELL, LOW);

  Serial.println("=== GreenFarm IoT - Sistema de Reg ===");
  Serial.println("Sensor: LM35");
  Serial.println("Inicialitzant...");
  delay(2000);
}

// ─────────────────────────────────────────────────
void loop() {
  unsigned long ara = millis();

  if (ara - ultimaLectura >= INTERVAL_MS) {
    ultimaLectura = ara;

    float temperatura = llegirTemperatura();

    // Mostrem dades per Serial Monitor
    Serial.println("-------------------------------");
    Serial.print("Temperatura : ");
    Serial.print(temperatura);
    Serial.println(" °C");

    // Lògica de reg automàtic
    if (temperatura > TEMP_MAX && !regActiu) {
      activarReg(temperatura);
    } else if (temperatura < TEMP_MIN && regActiu) {
      aturarReg(temperatura);
    } else {
      Serial.print("Estat reg   : ");
      Serial.println(regActiu ? "ACTIU" : "ATURAT");
    }
  }
}

// ─────────────────────────────────────────────────
void activarReg(float temp) {
  regActiu = true;
  digitalWrite(BOMBA_PIN, HIGH);
  digitalWrite(LED_VERD, LOW);
  digitalWrite(LED_VERMELL, HIGH);

  Serial.println(">>> REG ACTIVAT <<<");
  Serial.print("Motiu: temperatura alta (");
  Serial.print(temp);
  Serial.println(" °C)");
}

// ─────────────────────────────────────────────────
void aturarReg(float temp) {
  regActiu = false;
  digitalWrite(BOMBA_PIN, LOW);
  digitalWrite(LED_VERD, HIGH);
  digitalWrite(LED_VERMELL, LOW);

  Serial.println(">>> REG ATURAT <<<");
  Serial.print("Motiu: temperatura baixa (");
  Serial.print(temp);
  Serial.println(" °C)");
}
