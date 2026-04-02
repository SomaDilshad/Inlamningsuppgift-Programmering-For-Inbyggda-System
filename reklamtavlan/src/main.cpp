/**
 * Reklamtavlan - Programmering för inbyggda system
 * 
 * Detta program styr en reklamskylt som visar meddelanden från olika kunder.
 * Visningarna är viktade baserat på hur mycket kunderna har betalat.
 * Samma kund visas aldrig två gånger i rad.
 * 
 * Författare: Somadilshad Abdullah
 * Kurs: Programmering för inbyggda system
 * Datum: 2026-04-02
 */

#include <Arduino.h>
#include "DisplayManager.h"
#include "Scheduler.h"

// Definiera GPIO-pinnar (matchar diagram.json)
#define PIN_RS   GPIO_NUM_22
#define PIN_E    GPIO_NUM_21
#define PIN_D4   GPIO_NUM_19
#define PIN_D5   GPIO_NUM_18
#define PIN_D6   GPIO_NUM_5
#define PIN_D7   GPIO_NUM_17

// Globala objekt
DisplayManager display(PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
Scheduler scheduler;

// Task-handle för RTOS
TaskHandle_t displayTaskHandle = NULL;

/**
 * RTOS-task som hanterar displayen
 * Körs i en separat tråd och uppdaterar displayen var 20:e sekund
 */
void displayTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(20000); // 20 sekunder
    
    while (1) {
        // Kör schemaläggaren - väljer nästa kund och visar meddelandet
        scheduler.run(display);
        
        // Vänta exakt 20 sekunder till nästa visning
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * Arduino-style setup-funktion
 * Körs en gång vid start
 */
void setup() {
    // Initiera seriell kommunikation för debugging
    Serial.begin(115200);
    Serial.println("\n\n=== Reklamtavlan startar ===\n");
    
    // Initiera slumptalsgeneratorn med ESP32s hårdvarubaserade slump
    srand(esp_random());
    
    // Initiera displayen
    Serial.println("Initierar display...");
    display.begin();
    display.clear();
    display.print("Reklamtavlan");
    display.setCursor(0, 1);
    display.print("Startar...");
    
    // Skapa alla kunder enligt uppgiften
    Serial.println("Skapar kunder...");
    scheduler.setupCustomers();
    Serial.printf("Skapade %d kunder\n", scheduler.getCustomerCount());
    
    // Skapa RTOS-task för displayhantering
    Serial.println("Startar display-task...");
    xTaskCreatePinnedToCore(
        displayTask,     // Funktionspekare
        "DisplayTask",   // Tasknamn
        4096,            // Stack-storlek (bytes)
        NULL,            // Parameter
        1,               // Prioritet
        &displayTaskHandle, // Task-handle
        0                // Kör på core 0
    );
    
    Serial.println("Systemet är redo!\n");
}

/**
 * Arduino-style loop-funktion
 * Tom eftersom all logik ligger i displayTask
 */
void loop() {
    // Idle - låt RTOS hantera tasken
    vTaskDelay(pdMS_TO_TICKS(1000));
}
