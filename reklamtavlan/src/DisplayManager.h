#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <cstdint>

class DisplayManager {
private:
    // GPIO pin-nummer
    uint8_t m_rs_pin;
    uint8_t m_e_pin;
    uint8_t m_d4_pin;
    uint8_t m_d5_pin;
    uint8_t m_d6_pin;
    uint8_t m_d7_pin;
    
    // LCD-dimensioner
    static const int LCD_COLS = 16;
    static const int LCD_ROWS = 2;
    
    // Privata hjälpfunktioner för lågnivå-kommunikation
    void pulseEnable();
    void sendNibble(uint8_t nibble);
    void sendByte(uint8_t data, bool isData);
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    
    // Fördröjningsfunktion i millisekunder
    void delayMs(uint32_t ms);

public:
    // Konstruktor
    DisplayManager(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    
    // Initiera displayen
    void begin();
    
    // Rensa displayen
    void clear();
    
    // Sätt markören till en specifik position (col: 0-15, row: 0-1)
    void setCursor(uint8_t col, uint8_t row);
    
    // Skriv ut en sträng på aktuell position
    void print(const char* str);
    
    // Skriv ut ett enstaka tecken
    void print(char c);
    
    // Rulla en text (visas i 20 sekunder med rullning)
    void scrollText(const char* str, int durationMs);
    
    // Blinkande text (visas i durationMs millisekunder)
    void blinkText(const char* str, int durationMs);
    
    // Visa vanlig text
    void showStaticText(const char* str);
    
    // Hämta displayens bredd
    int getCols() const { return LCD_COLS; }
    
    // Hämta displayens höjd
    int getRows() const { return LCD_ROWS; }
};

#endif
