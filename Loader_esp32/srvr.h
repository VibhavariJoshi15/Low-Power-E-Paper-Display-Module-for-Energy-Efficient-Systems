
// / *           + WiFiManager captive portal
//  *           + mDNS hostname: http://epaper.local
//  *           + Removed need for static IP
//  *           + No change to e-paper functionality
// 
//  */

#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>

#include "buff.h"
#include "epd.h"
#include "scripts.h"
#include "css.h"
#include "html.h"

/* ------------- No Static IP Needed Anymore ------------- */
// (We keep these definitions for compatibility but DO NOT use them)
IPAddress myIP;

/* ---------------- Server ------------------ */
WiFiServer server(80);
bool isIndexPage = true;

/* ============================================================
   WIFI + MDNS SETUP
   ============================================================ */
void Srvr__setup()
{
    Serial.println("\nStarting WiFiManager...");

    WiFiManager wm;

    // OPTIONAL: Reset WiFi settings by holding BOOT button on reset
    // wm.resetSettings();

    // Start captive portal if no stored WiFi
    if (!wm.autoConnect("EPAPER_SETUP")) {
        Serial.println("Failed to connect. Rebooting...");
        delay(3000);
        ESP.restart();
        return;
    }

    Serial.println("WiFi Connected!");

    // mDNS name → http://epaper.local
    if (MDNS.begin("epaper")) {
        Serial.println("mDNS responder started: http://epaper.local");
    } else {
        Serial.println("mDNS Error!");
    }

    server.begin();
    myIP = WiFi.localIP();

    Serial.print("Server running at IP: ");
    Serial.println(myIP);
}

/* ============================================================
   FILE SENDER (unchanged)
   ============================================================ */
bool Srvr__file(WiFiClient client, int fileIndex, char *fileName)
{
    Serial.print(fileName);

    client.print(fileIndex == 0
                 ? "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n"
                 : "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n");

    switch (fileIndex) {
    case 0: sendCSS(client); break;
    case 1: sendJS_A(client); break;
    case 2: sendJS_B(client); break;
    case 3: sendJS_C(client); break;
    case 4: sendJS_D(client); break;
    }

    client.print("\r\n");
    delay(1);

    Serial.println(">>>");

    return true;
}

/* ============================================================
   MAIN SERVER LOOP (unchanged)
   ============================================================ */
bool Srvr__loop()
{
    WiFiClient client = server.available();
    int16_t label = 0;
    int16_t fale = 1000;

    if (!client)
        return false;

    Serial.print("<<<");

    while (!client.available()) {
        delay(1);
        label++;
        if(label > fale) {
            label = fale + 1;
            if(!server.available())
                return false;
        }
    }

    Buff__bufInd = 0;

    while (client.available()) {
        int q = client.read();
        Buff__bufArr[Buff__bufInd++] = (byte)q;

        if ((q == 10) || (q == 13)) {
            Buff__bufInd = 0;
            continue;
        }

        if (Buff__bufInd >= 11) {
            if (Buff__signature(Buff__bufInd - 11, "/styles.css"))
                return Srvr__file(client, 0, "styles.css");

            if (Buff__signature(Buff__bufInd - 11, "/scriptA.js"))
                return Srvr__file(client, 1, "scriptA.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptB.js"))
                return Srvr__file(client, 2, "scriptB.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptC.js"))
                return Srvr__file(client, 3, "scriptC.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptD.js"))
                return Srvr__file(client, 4, "scriptD.js");
        }

        if (Buff__bufInd > 4) {
            isIndexPage = false;

            if (Buff__signature(Buff__bufInd - 4, "EPD")) {
                Serial.print("\r\nEPD\r\n");
                EPD_dispIndex = (int)Buff__bufArr[Buff__bufInd - 1] - (int)'a';
                if(EPD_dispIndex < 0)
                    EPD_dispIndex = (int)Buff__bufArr[Buff__bufInd - 1] - (int)'A' + 26;

                Serial.printf("EPD %s", EPD_dispMass[EPD_dispIndex].title);
                EPD_dispInit();
                break;
            }

            if (Buff__signature(Buff__bufInd - 4, "LOAD")) {
                Serial.print("LOAD");
                if (EPD_dispLoad != 0)
                    EPD_dispLoad();
                break;
            }

            if (Buff__signature(Buff__bufInd - 4, "NEXT")) {
                Serial.print("NEXT");
                int code = EPD_dispMass[EPD_dispIndex].next;

                if(EPD_dispIndex == 34) {
                    if(flag == 0)
                        code = 0x26;
                    else
                        code = 0x13;
                }

                if (code != -1) {
                    Serial.printf(" %x", code);
                    EPD_SendCommand(code);
                    delay(2);
                }

                EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;
                break;
            }

            if (Buff__signature(Buff__bufInd - 4, "SHOW")) {
                EPD_dispMass[EPD_dispIndex].show();
                Serial.print("\r\nSHOW");
                break;
            }

            isIndexPage = true;
        }
    }

    client.flush();

    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");

    if (isIndexPage)
        sendHtml(client, myIP);
    else
        client.print("Ok!");

    client.print("\r\n");
    delay(1);

    Serial.println(">>>");
    return true;
}
