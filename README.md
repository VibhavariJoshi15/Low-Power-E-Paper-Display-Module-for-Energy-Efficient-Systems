This repository contains webserver for epaper display image updation.
This transforms the E-Paper ESP32 Driver Board into a wireless image uploader + processing server, allowing users to update any supported Waveshare SPI e-Paper panel through a clean browser interface.
________________________________________
🚀 Features
✔️ Wireless Image Upload
Upload images directly through the browser—no USB cable needed.
✔️ Smart Image Processing
Supports two high-quality processing pipelines:
Mode	Description
Level (Color Scale)	Best for text, line art, and clean graphics.
Floyd–Steinberg Dithering	Best for photos and shaded images.
✔️ Multi-Color Display Support
Compatible with Black/White and Red/Yellow tri-color panels.
✔️ Accepts Standard Image Formats
BMP, PNG, JPEG, GIF → automatically converted to e-Paper format.
✔️ Power Efficient
Display enters deep sleep after every refresh.
✔️ Arduino Compatible
Easily modifiable through the Arduino IDE ecosystem.
✔️ mDNS Enabled
Access the device using
👉 http://epaper.local/
No need to check dynamic IPs.
________________________________________
📦 1. Prerequisites & Setup
1.1 Hardware Required
•	Waveshare ESP32 E-Paper Driver Board
•	Any Waveshare SPI E-Paper Raw Panel
•	USB cable
1.2 Fixed SPI Pin Mapping
Signal	ESP32 Pin	Description
SCK	GPIO13	SPI Clock
MOSI / DIN	GPIO14	Display Data In
CS	GPIO15	Chip Select
DC	GPIO27	Data/Command
RST	GPIO26	Display Reset
BUSY	GPIO25	Display Busy Indicator
These pins are fixed for the Waveshare board—no wiring needed.
1.3 Mandatory Hardware Switch Setting
The voltage-select resistor switch must match your panel type.
Wrong setting → display will not refresh.
________________________________________
⚙️ 2. Software Setup
2.1 Arduino IDE Setup
1.	Install ESP32 board package.
2.	Select board: ESP32 Dev Module.
3.	Install Waveshare e-paper libraries if required.
4.	Open the project folder provided here.
________________________________________
🌐 2.2 Wi-Fi + mDNS Configuration
Open srvr.h and configure:
1. Set your Wi-Fi credentials
const char *ssid = "Your_WiFi_SSID";
const char *password = "Your_WiFi_Password";
2. Disable static IP
Waveshare demo code originally used static IP.
Remove or comment out:
// IPAddress staticIP(192, 168, 1, 154);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress dns(223, 5, 5, 5);
3. Enable mDNS (hostname: epaper)
Add this in your setup section (or initialization block):
#include <ESPmDNS.h>

if (MDNS.begin("epaper")) {
    Serial.println("mDNS responder started: http://epaper.local/");
} else {
    Serial.println("Error starting mDNS");
}
Now your ESP32 will always be reachable at:
👉 http://epaper.local/
(No need to find IP; works even with dynamic IP.)
________________________________________
📤 3. Uploading & Operating the Web Interface
3.1 After Uploading the Firmware
1.	Open Serial Monitor → 115200 baud
2.	ESP connects to Wi-Fi
3.	You’ll see:
mDNS responder started: http://epaper.local/
3.2 Open the Web UI
Inside any browser:
➡️ http://epaper.local/
If mDNS is not supported on your OS/router:
Use the dynamic IP printed in Serial Monitor.
________________________________________
🖥️ 4. Web Interface Overview
The UI contains the following sections:
🎨 Image Processing
Select algorithm:
•	Level: mono
•	Level: color
•	Dithering: mono
•	Dithering: color
Each mode is optimized for different display types.
✂️ Cropping / Bounds
Input fields:
•	X (start position)
•	Y (start position)
•	Width (auto-set by display type)
•	Height (auto-set by display type)
🖼️ Display Model Selector
Choose your exact e-paper model for correct resolution & settings.
📤 Upload & Refresh
Automatically:
1.	Converts the image
2.	Splits channels (Black / Red, depending on panel)
3.	Transfers pixel data packet-by-packet
4.	Refreshes the display
5.	Puts ESP32 + E-Paper in deep sleep
________________________________________
🔌 5. E-Paper Command Protocol (Internal)
The browser sends structured commands:
Command	Purpose
EPDn	Select display model & initialize black channel
LOAD	Start pixel loading
NEXT	Switch to second color channel (Red/Yellow)
DONE	Finish → refresh display → deep sleep
This is handled automatically by the UI and ESP32 backend.
________________________________________
🏁 Final Notes
•	Works on any network with DHCP (dynamic IP)
•	No code changes needed for different users
•	Users just open http://epaper.local/
•	Latest browsers + Windows/macOS/Linux supported

