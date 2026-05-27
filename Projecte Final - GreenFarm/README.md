# 🌱 Sistema de Reg Intel·ligent - GreenFarm IoT

Projecte IoT desenvolupat per al client **Granja GreenFarm** com a part del repte industrial del mòdul IoT Projects Lab.

## 📋 Descripció

Sistema automatitzat de reg basat en temperatura. Quan la temperatura supera els **28°C**, el sistema activa la bomba de reg automàticament. Quan baixa dels **20°C**, l'atura.

## 🛠️ Hardware necessari

| Component | Pin ESP32 | Funció |
|-----------|-----------|--------|
| Sensor LM35 | Pin 34 | Mesura la temperatura |
| Relé (bomba) | Pin 5 | Activa/atura el reg |
| LED verd | Pin 18 | Sistema en repòs |
| LED vermell | Pin 19 | Reg actiu |

## ⚙️ Com funciona

```
Temperatura > 28°C  →  Reg ACTIVAT  🔴
Temperatura < 20°C  →  Reg ATURAT   🟢
Cada 5 segons       →  Dades al monitor sèrie
```

## 📁 Estructura del repositori

```
projecte_iot/
 ├── README.md
 ├── src/
 │   └── main.ino       ← Codi principal ESP32
 └── doc/
     └── document_tecnic_greenfarm.docx
```

## 🚀 Instal·lació i ús

1. Obre `src/main.ino` amb l'**Arduino IDE**
2. Connecta l'ESP32 per USB
3. Selecciona la placa: `ESP32 Dev Module`
4. Puja el codi
5. Obre el **Monitor Sèrie** a 115200 bauds per veure les dades

> ⚠️ No cal instal·lar cap llibreria externa. El sensor LM35 és analògic i es llegeix directament.

## 👥 Autors

- Ruben Sanzo
- Juan Vidal

## 📄 Llicència

Projecte acadèmic - IoT Projects Lab 2024-2025
