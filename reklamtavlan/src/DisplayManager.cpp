#include "DisplayManager.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>

// Konstruktor - sätter pin-nummer
DisplayManager::DisplayManager(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
    : m_rs_pin(rs), m_e_pin(e), m_d4_pin(d4), m_d5_pin(d5), m_d6_pin(d6), m_d7_pin(d7) {}

// Enkel fördröjningsfunktion
void DisplayManager::delayMs(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

// Pulsera Enable-pinnen för att låsa in data
void DisplayManager::pulseEnable() {
    gpio_set_level((gpio_num_t)m_e_pin, 1);
    delayMs(1);
    gpio_set_level((gpio_num_t)m_e_pin, 0);
    delayMs(1);
}

// Skicka 4 bitar (en nibble)
void DisplayManager::sendNibble(uint8_t nibble) {
    gpio_set_level((gpio_num_t)m_d4_pin, (nibble >> 0) & 0x01);
    gpio_set_level((gpio_num_t)m_d5_pin, (nibble >> 1) & 0x01);
    gpio_set_level((gpio_num_t)m_d6_pin, (nibble >> 2) & 0x01);
    gpio_set_level((gpio_num_t)m_d7_pin, (nibble >> 3) & 0x01);
    pulseEnable();
}

// Skicka en hel byte (först höga 4 bitar, sedan låga 4 bitar)
void DisplayManager::sendByte(uint8_t data, bool isData) {
    // Sätt RS-pinnen (0=kommando, 1=data)
    gpio_set_level((gpio_num_t)m_rs_pin, isData ? 1 : 0);
    
    // Skicka de 4 högsta bitarna
    sendNibble(data >> 4);
    
    // Skicka de 4 lägsta bitarna
    sendNibble(data & 0x0F);
}

// Skicka ett kommando till displayen
void DisplayManager::writeCommand(uint8_t cmd) {
    sendByte(cmd, false);
    delayMs(2); // De flesta kommandon behöver lite tid
}

// Skicka data (tecken) till displayen
void DisplayManager::writeData(uint8_t data) {
    sendByte(data, true);
    delayMs(1);
}

// Initiera displayen i 4-bitarsläge
void DisplayManager::begin() {
    // Konfigurera alla pinnar som utgångar
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << m_rs_pin) | (1ULL << m_e_pin) |
                           (1ULL << m_d4_pin) | (1ULL << m_d5_pin) |
                           (1ULL << m_d6_pin) | (1ULL << m_d7_pin);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Sätt alla pinnar låga initialt
    gpio_set_level((gpio_num_t)m_rs_pin, 0);
    gpio_set_level((gpio_num_t)m_e_pin, 0);
    gpio_set_level((gpio_num_t)m_d4_pin, 0);
    gpio_set_level((gpio_num_t)m_d5_pin, 0);
    gpio_set_level((gpio_num_t)m_d6_pin, 0);
    gpio_set_level((gpio_num_t)m_d7_pin, 0);
    
    delayMs(50); // Vänta att displayen startar
    
    // Initieringssekvens för 4-bitarsläge (HD44780)
    // Steg 1: Sätt till 8-bitarsläge (3 gånger)
    sendNibble(0x03);
    delayMs(5);
    sendNibble(0x03);
    delayMs(1);
    sendNibble(0x03);
    delayMs(1);
    
    // Steg 2: Sätt till 4-bitarsläge
    sendNibble(0x02);
    delayMs(1);
    
    // Steg 3: Konfigurera displayen
    writeCommand(0x28); // 2 rader, 5x8 dots, 4-bitarsläge
    writeCommand(0x0C); // Display på, cursor av, blink av
    writeCommand(0x06); // Entry mode: inkrementell, ingen shift
    writeCommand(0x01); // Rensa displayen
    delayMs(2);
}

// Rensa displayen
void DisplayManager::clear() {
    writeCommand(0x01);
    delayMs(2);
}

// Sätt cursorposition
void DisplayManager::setCursor(uint8_t col, uint8_t row) {
    uint8_t address;
    if (row == 0) {
        address = col;
    } else {
        address = 0x40 + col;
    }
    writeCommand(0x80 | address);
}

// Skriv ut en sträng
void DisplayManager::print(const char* str) {
    while (*str) {
        writeData(*str++);
    }
}

// Skriv ut ett enstaka tecken
void DisplayManager::print(char c) {
    writeData(c);
}

// Visa vanlig statisk text
void DisplayManager::showStaticText(const char* str) {
    clear();
    setCursor(0, 0);
    print(str);
}

// Rullande text - texten rullar från höger till vänster
void DisplayManager::scrollText(const char* str, int durationMs) {
    int len = strlen(str);
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Skapa en buffert för rullande text
    char displayBuffer[LCD_COLS + 1];
    displayBuffer[LCD_COLS] = '\0';
    
    // Rulla texten tills tiden är slut
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS - startTime) < durationMs) {
        for (int offset = 0; offset <= len + LCD_COLS; offset++) {
            // Fyll bufferten
            for (int i = 0; i < LCD_COLS; i++) {
                int idx = offset + i;
                if (idx >= 0 && idx < len) {
                    displayBuffer[i] = str[idx];
                } else {
                    displayBuffer[i] = ' ';
                }
            }
            
            // Visa på displayen
            clear();
            setCursor(0, 0);
            print(displayBuffer);
            
            delayMs(150); // Rullningshastighet
            
            // Kontrollera om tiden är slut
            if ((xTaskGetTickCount() * portTICK_PERIOD_MS - startTime) >= durationMs) {
                break;
            }
        }
    }
}

// Blinkande text
void DisplayManager::blinkText(const char* str, int durationMs) {
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
    bool visible = true;
    
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS - startTime) < durationMs) {
        clear();
        if (visible) {
            setCursor(0, 0);
            print(str);
        }
        delayMs(500); // Blinkhastighet
        visible = !visible;
    }
    
    // Se till att texten är synlig när vi är klara
    clear();
    setCursor(0, 0);
    print(str);
}
