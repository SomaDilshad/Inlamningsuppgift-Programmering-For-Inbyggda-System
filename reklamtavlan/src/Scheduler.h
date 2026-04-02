#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Customer.h"
#include "DisplayManager.h"
#include <vector>

class Scheduler {
private:
    std::vector<Customer> m_customers;
    int m_lastCustomerIndex;      // Senast visade kund (för att undvika repetition)
    unsigned long m_lastUpdateTime; // Tidpunkt för senaste uppdatering
    int m_currentMinute;          // Nuvarande minut (sedan start eller från RTC)
    bool m_initialized;
    
    // Viktad slumpning - returnerar index för vald kund
    int getWeightedRandomCustomerIndex();
    
    // Kontrollera om en kund har giltiga meddelanden för aktuell tid
    bool customerHasValidMessages(const Customer& customer, int currentMinute);
    
public:
    Scheduler();
    
    // Lägg till en kund
    void addCustomer(const Customer& customer);
    
    // Skapa alla fördefinierade kunder (Hederlige Harry, Farmor Anka, etc.)
    void setupCustomers();
    
    // Uppdatera tiden (anropas regelbundet)
    void updateTime();
    
    // Hämta nuvarande minut
    int getCurrentMinute() const { return m_currentMinute; }
    
    // Huvudlogik - välj nästa kund och visa meddelandet
    void run(DisplayManager& display);
    
    // Hämta antalet kunder
    int getCustomerCount() const { return m_customers.size(); }
};

#endif
