Reklamtavlan – inbyggt system för reklamvisning

Översikt

Detta projekt är en examinationsuppgift i kursen Programmering för inbyggda system vid Stockholms Tekniska Institut. Det implementerar en digital reklamskylt som visar meddelanden från olika kunder med en visningsfrekvens som är proportionell mot kundens betalning.

Funktionella krav (uppfyllda)

- Fem olika kunder med unika meddelanden
- Viktad visning baserat på betalning (t.ex. Hederlige Harry 5000 kr visas oftare än Farmor Anka 3000 kr)
- Samma kund visas inte två gånger i rad
- Varje meddelande visas i 20 sekunder
- Tre visningstyper: text (statisk), scroll, blink
- Tidsbaserade meddelanden för Svarte Petter (jämna respektive ojämna minuter i den simulerade tiden)
- LCD styrs med egen GPIO-kod (ingen `LiquidCrystal`-biblioteksdrivrutin)

Teknisk arkitektur

Hårdvara

- Mikrokontroller: ESP32 (ESP32-DevKit-V1)
- Display: 16×2 LCD med HD44780-kompatibelt gränssnitt
- Anslutning: 4-bitars parallellt gränssnitt (se `diagram.json` för Wokwi)

Mjukvara

```
reklamtavlan/src/
├── main.cpp                 Huvudprogram och FreeRTOS-task
├── DisplayManager.cpp/.h    Lågnivå-LCD (HD44780, 4-bit)
├── Customer.h               Kund, meddelanden och urval (header med implementation)
├── Message.h                Meddelandetyper och struktur
└── Scheduler.cpp/.h         Viktad slumpning och visningsloop
```

DisplayManager

Egen LCD-initiering och skrivning via GPIO, utan Arduino `LiquidCrystal`.

cheduler

Viktad slumpning (roulette wheel) utifrån betalning, undviker att samma kund visas direkt efter sig själv, och anropar rätt visning (statisk, scroll, blink) i 20 sekunder.

FreeRTOS

Separat task för displaylogik med `vTaskDelayUntil` för jämna 20 s-intervall.

Motivering av tekniska val (VG)

C++

Objektorientering passar för kunder och meddelanden; tydlig uppdelning mellan display, schemaläggning och data.

Egen LCD-drivrutin

Visar förståelse för HD44780/4-bit och GPIO, och uppfyller krav på hårdvarunära kod utan färdig LCD-klass.

FreeRTOS

ESP32 använder FreeRTOS; tasks ger ren separation och förutsägbar timing.

Förbättringsförslag

1. NTP – synka klocka över WiFi så jämna/ojämna minuter motsvarar verklig tid.
2. SPIFFS/LittleFS** – lagra kunder och texter i fil istället för i källkod.
3. Webbgränssnitt** – enkel konfiguration via HTTP på ESP32.
4. Energi** – deep sleep mellan visningar vid batteridrift.
5. Felhantering** – robust hantering av tomma listor och displayfel.

