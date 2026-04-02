# Inlämningsuppgift – Programmering för inbyggda system

Detta repository innehåller inlämningsuppgiften **Reklamtavlan**: ett ESP32-projekt (PlatformIO) som visar viktade reklammeddelanden på en 16×2 LCD.

## Var finns koden?

Själva inlämningen ligger i undermappen **`reklamtavlan/`**:

- `reklamtavlan/platformio.ini` – byggkonfiguration (ESP32, Arduino-ramverk)
- `reklamtavlan/src/` – källkod (`main.cpp`, `DisplayManager`, `Scheduler`, `Customer`, `Message`)
- `reklamtavlan/diagram.json` – kopplingsschema för Wokwi
- `reklamtavlan/README.md` – fullständig beskrivning, krav och arkitektur

Öppna **`reklamtavlan`** i VS Code med PlatformIO-tillägget, bygg och ladda upp till kortet, eller simulera med Wokwi enligt `diagram.json`.
