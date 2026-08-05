// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy: #include "SSD1306.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL); // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

#include "Oled.h"

#include "Global.h"
#include "credentials.h"

#include <ESP8266WiFi.h> //show Wifi connections
// start pixel of blue area
#define Y_OFFSET 16
//"table line 1"
#define X_OFFSET_1 54

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;
void drawStartScreen();

void oledSetup()
{

    Serial.println("OledSetup --> Start");

    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    drawStartScreen();

    Serial.println("OledSetup --> End");
}
// helper
void drawMillisAsTime()
{
    String t;
    unsigned long ms = millis();
    unsigned long s = ms / 1000;
    unsigned long m = s / 60;
    unsigned long h = m / 60;

    ms = ms % 1000 / 100; // only 100ms resolution
    s = s % 60;
    m = m % 60;

    t = String(h, DEC) + ":" + String(m, DEC) + ":" + String(s, DEC) + ":" + String(ms, DEC);
    // Draw actual millis in h,m,s,ms top right
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 0, t); // top right
}

void drawStartScreen()
{
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, Assembly.deviceId);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0 + Y_OFFSET, "Search WiFi");
    display.drawString(0, 12 + Y_OFFSET, Assembly.cfg.wifi[0].ssid);
    display.drawString(0, 24 + Y_OFFSET, Assembly.cfg.wifi[1].ssid);
    display.drawString(0, 36 + Y_OFFSET, Assembly.cfg.wifi[2].ssid);
    display.display();
}

void oledShowNetworks()
{
    display.clear();

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw actual millis top right
    drawMillisAsTime();

    // Show Assembly Name
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.deviceId); // top left
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("oledShowNetworks() -->scan done");
    if (n == 0)
    {
        display.drawString(0, Y_OFFSET, String("no networks found"));
    }
    else
    {
        for (int j = 0; j <= n / 4; j++)
        {
            Serial.println(String("oledShowNetworks() -->") + j);
            for (int i = 0; i < 4; ++i)
            {
                // Print SSID and RSSI for each network found
                String s("");
                s += j * 4 + i + 1;
                s += ": ";
                s += WiFi.SSID(j * 4 + i);
                s += " (";
                s += WiFi.RSSI(j * 4 + i);
                s += ")                                 ";

                display.drawString(0, Y_OFFSET + i * 12, s);
            }
            display.display();
            delay(1000);
            display.clear();
        }
    }
}

void oledLoop()
{

    // drawStartScreen();

    display.clear();

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw ip
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.localIp); // top right

    if (Assembly.state == StateTare)
    {
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 0, "Tare");

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0 + Y_OFFSET, "Remove load from sensor!");

        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0 + Y_OFFSET + 14, "TARA 0 ...");
    }
    else if (Assembly.state == StateCalibrate)
    {
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 0, "Calibrate");

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0 + Y_OFFSET, "Place 1kg on sensor!");

        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0 + Y_OFFSET + 14, "CALIBRATE 1kg ...");
    }
    else if (Assembly.state == StateReboot)
    {
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 0, "Reboot");

        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0 + Y_OFFSET + 14, "REBOOTING ...");
    }
    else
    {
        display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.drawString(128, 0, "Force"); //top right

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(0, 0 + Y_OFFSET + 0, "Force: " + String(Assembly.force.value) + "N ");
    }

    //  write the buffer to the display
    display.display();

    counter++;
}
