#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Message.h"
#include <cstdlib>
#include <vector>

class Customer {
private:
    const char* m_name;           // Kundens namn
    int m_payment;                // Betalning i kronor
    std::vector<Message> m_messages;  // Kundens meddelanden

public:
    // Konstruktor
    Customer(const char* name, int payment) 
        : m_name(name), m_payment(payment) {}
    
    // Lägg till ett vanligt meddelande
    void addMessage(const char* text, MessageType type) {
        m_messages.push_back(Message(text, type));
    }
    
    // Lägg till ett tidsbaserat meddelande
    void addTimeBasedMessage(const char* text, MessageType type, int modulo, int value) {
        m_messages.push_back(Message(text, type, modulo, value));
    }
    
    // Hämta kundens betalning (används för viktning)
    int getPayment() const { return m_payment; }
    
    // Hämta kundens namn
    const char* getName() const { return m_name; }
    
    // Hämta ett slumpmässigt meddelande från kunden
    // currentMinute: aktuell minut (används för tidsbaserade meddelanden)
    Message getRandomMessage(int currentMinute) {
        if (m_messages.empty()) {
            return Message("Inget meddelande", MessageType::TEXT);
        }
        
        // Första steget: om det finns tidsbaserade meddelanden, filtrera
        std::vector<Message> validMessages;
        
        for (const auto& msg : m_messages) {
            if (!msg.isTimeBased) {
                // Vanliga meddelanden är alltid giltiga
                validMessages.push_back(msg);
            } else {
                // Kontrollera om tidsvillkoret är uppfyllt
                if (currentMinute % msg.timeModulo == msg.timeValue) {
                    validMessages.push_back(msg);
                }
            }
        }
        
        // Om inga meddelanden är giltiga just nu (t.ex. inget tidsbaserat matchar)
        // så använd alla meddelanden som fallback
        if (validMessages.empty()) {
            // Fallback: använd alla meddelanden
            for (const auto& msg : m_messages) {
                validMessages.push_back(msg);
            }
        }
        
        // Slumpa fram ett meddelande från de giltiga
        int randomIndex = rand() % validMessages.size();
        return validMessages[randomIndex];
    }
    
    // Kontrollera om kunden har några meddelanden
    bool hasMessages() const {
        return !m_messages.empty();
    }
    
    // Hämta antalet meddelanden
    int getMessageCount() const {
        return m_messages.size();
    }
};

#endif
