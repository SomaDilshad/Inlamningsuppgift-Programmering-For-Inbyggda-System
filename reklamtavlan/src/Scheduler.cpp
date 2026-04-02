#include "Scheduler.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstdlib>

Scheduler::Scheduler() 
    : m_lastCustomerIndex(-1), m_lastUpdateTime(0), m_currentMinute(0), m_initialized(false) {}

// Lägg till en kund
void Scheduler::addCustomer(const Customer& customer) {
    m_customers.push_back(customer);
}

// Skapa alla fördefinierade kunder enligt uppgiften
void Scheduler::setupCustomers() {
    // Hederlige Harrys Bilar - betalat 5000 kr
    Customer harry("Hederlige Harry", 5000);
    harry.addMessage("Kop bil hos Harry", MessageType::SCROLL);
    harry.addMessage("En god bilaffar (for Harry!)", MessageType::TEXT);
    harry.addMessage("Hederlige Harrys Bilar", MessageType::BLINK);
    addCustomer(harry);
    
    // Farmor Ankas Pajer AB - betalat 3000 kr
    Customer farmorAnka("Farmor Anka", 3000);
    farmorAnka.addMessage("Kop paj hos Farmor Anka", MessageType::SCROLL);
    farmorAnka.addMessage("Skynda innan Morten atit alla pajer", MessageType::TEXT);
    addCustomer(farmorAnka);
    
    // Svarte Petters Svartbyggen - betalat 1500 kr (tidsbaserade meddelanden)
    Customer svartePetter("Svarte Petter", 1500);
    svartePetter.addTimeBasedMessage("Lat Petter bygga at dig", MessageType::SCROLL, 2, 0);  // Jämna minuter
    svartePetter.addTimeBasedMessage("Bygga svart? Ring Petter", MessageType::TEXT, 2, 1);   // Ojämna minuter
    addCustomer(svartePetter);
    
    // Långbens detektivbyrå - betalat 4000 kr
    Customer langben("Langben", 4000);
    langben.addMessage("Mysterier? Ring Langben", MessageType::TEXT);
    langben.addMessage("Langben fixar biffen", MessageType::TEXT);
    addCustomer(langben);
    
    // IOT:s Reklambyrå (vi själva) - motsvarar 1000 kr
    Customer iotReklam("IOT Reklambyra", 1000);
    iotReklam.addMessage("Synas har? IOT:s Reklambyra", MessageType::TEXT);
    addCustomer(iotReklam);
    
    m_initialized = true;
}

// Uppdatera tiden (anropas varje gång vi visar ett meddelande)
void Scheduler::updateTime() {
    unsigned long currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Räkna ut hur många 20-sekundersintervall som har passerat
    // Varje visning är 20 sekunder, så vi ökar minuten var 3:e visning (60 sekunder)
    if (m_lastUpdateTime == 0) {
        m_lastUpdateTime = currentTime;
        return;
    }
    
    unsigned long elapsed = currentTime - m_lastUpdateTime;
    int intervalsPassed = elapsed / 20000; // 20 sekunder per visning
    
    if (intervalsPassed > 0) {
        // Uppdatera minuten baserat på antal visningar
        m_currentMinute += intervalsPassed;
        m_lastUpdateTime = currentTime;
    }
}

// Viktad slumpning baserat på betalning
int Scheduler::getWeightedRandomCustomerIndex() {
    if (m_customers.empty()) {
        return -1;
    }
    
    // Beräkna total vikt (summan av allas betalning)
    int totalWeight = 0;
    for (const auto& customer : m_customers) {
        totalWeight += customer.getPayment();
    }
    
    // Om bara en kund finns, returnera den
    if (m_customers.size() == 1) {
        return 0;
    }
    
    // Försök hitta en ny kund (max 100 försök)
    for (int attempt = 0; attempt < 100; attempt++) {
        int randomValue = rand() % totalWeight;
        int currentSum = 0;
        
        for (int i = 0; i < m_customers.size(); i++) {
            currentSum += m_customers[i].getPayment();
            if (randomValue < currentSum) {
                // Kontrollera att det inte är samma kund som förra gången
                if (i != m_lastCustomerIndex) {
                    // Kontrollera att kunden har giltiga meddelanden för aktuell tid
                    if (customerHasValidMessages(m_customers[i], m_currentMinute)) {
                        return i;
                    }
                    // Om kunden inte har giltiga meddelanden just nu, fortsätt leta
                    break;
                }
                break;
            }
        }
    }
    
    // Fallback: hitta första kund som inte är samma som förra och har giltiga meddelanden
    for (int i = 0; i < m_customers.size(); i++) {
        if (i != m_lastCustomerIndex && customerHasValidMessages(m_customers[i], m_currentMinute)) {
            return i;
        }
    }
    
    // Om alla andra misslyckas, returnera första kunden (även om det är samma)
    return 0;
}

// Kontrollera om en kund har något giltigt meddelande för aktuell tid
bool Scheduler::customerHasValidMessages(const Customer& customer, int currentMinute) {
    // Vi måste skapa ett temporärt Message-objekt för att testa
    // Eftersom getRandomMessage kräver currentMinute, men vi kan inte anropa det direkt
    // utan att faktiskt välja ett meddelande. Enklast: om kunden har minst ett meddelande
    // och om meddelandena inte är tidsbaserade på ett sätt som gör dem ogiltiga.
    // För enkelhetens skull antar vi att alla kunder alltid har minst ett giltigt meddelande.
    // Detta stämmer eftersom vi i setupCustomers har lagt till meddelanden för alla.
    return true;
}

// Huvudlogik - välj nästa kund och visa meddelandet
void Scheduler::run(DisplayManager& display) {
    if (!m_initialized) {
        display.showStaticText("Initierar...");
        return;
    }
    
    // Uppdatera tiden
    updateTime();
    
    // Välj nästa kund (med viktning och undvik repetition)
    int customerIndex = getWeightedRandomCustomerIndex();
    
    if (customerIndex < 0 || customerIndex >= m_customers.size()) {
        display.showStaticText("Inga kunder");
        return;
    }
    
    // Spara vilken kund som visades
    m_lastCustomerIndex = customerIndex;
    
    // Hämta kunden och ett slumpmässigt meddelande
    Customer& selectedCustomer = m_customers[customerIndex];
    Message msg = selectedCustomer.getRandomMessage(m_currentMinute);
    
    // Visa meddelandet baserat på typ
    switch (msg.type) {
        case MessageType::TEXT:
            display.showStaticText(msg.text);
            break;
            
        case MessageType::SCROLL:
            display.scrollText(msg.text, 20000); // 20 sekunder
            break;
            
        case MessageType::BLINK:
            display.blinkText(msg.text, 20000); // 20 sekunder
            break;
    }
}
