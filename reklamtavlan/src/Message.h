#ifndef MESSAGE_H
#define MESSAGE_H

// Enum för olika typer av visningseffekter
enum class MessageType {
    TEXT,      // Vanlig text som visas statiskt
    SCROLL,    // Text som rullar från höger till vänster
    BLINK      // Text som blinkar
};

// Struktur som representerar ett enskilt reklammeddelande
struct Message {
    const char* text;           // Själva meddelandetexten
    MessageType type;           // Vilken typ av visning
    
    // För tidsbaserade meddelanden (används av Svarte Petter)
    bool isTimeBased;           // True om meddelandet är tidsberoende
    int timeModulo;             // Modulovärde (t.ex. 2 för jämna/ojämna)
    int timeValue;              // Vilket värde som ska matchas (0=jämn, 1=ojämn)
    
    // Konstruktor för vanliga meddelanden
    Message(const char* t, MessageType ty) 
        : text(t), type(ty), isTimeBased(false), timeModulo(0), timeValue(0) {}
    
    // Konstruktor för tidsbaserade meddelanden
    Message(const char* t, MessageType ty, int mod, int val) 
        : text(t), type(ty), isTimeBased(true), timeModulo(mod), timeValue(val) {}
    
    // Default-konstruktor (för att kunna skapa tomma arrayer)
    Message() : text(""), type(MessageType::TEXT), isTimeBased(false), timeModulo(0), timeValue(0) {}
};

#endif
