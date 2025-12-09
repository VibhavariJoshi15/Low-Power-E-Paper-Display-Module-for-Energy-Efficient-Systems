

#include "srvr.h"   // Updated server with WiFiManager + mDNS
//#include "epd.h"    // E-paper driver (unchanged)

/* Entry point ----------------------------------------------------------------*/
void setup() 
{
    // Serial initialization
    Serial.begin(115200);
    delay(200);

    Serial.println("\nBooting...");

    // WiFi + mDNS + Server initialization
    Srvr__setup();

    // SPI initialization for E-Paper
    EPD_initSPI();

    Serial.println("\r\nInitialization Complete.\r\n");
}

/* Main loop ------------------------------------------------------------------*/
void loop() 
{
    // Handle server requests
    Srvr__loop();

    // Auto-reconnect if WiFi drops
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi disconnected! Reconnecting...");
        delay(1000);
        WiFi.reconnect();
    }

    // Small delay to prevent watchdog reset
    delay(5);
}
