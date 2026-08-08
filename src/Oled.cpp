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

// force history graph, lower half of the 128x64 display
#define GRAPH_TOP 34
#define GRAPH_BOTTOM 63
#define GRAPH_HEIGHT (GRAPH_BOTTOM - GRAPH_TOP)

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

// draws the last FORCE_HISTORY_SIZE force values as a line, autoscaled to their own min/max,
// newest sample anchored at the right edge so the graph scrolls in from the right
void drawForceGraph()
{
    int historyCount = Assembly.force.historyFull ? FORCE_HISTORY_SIZE : Assembly.force.historyIndex;
    if (historyCount < 2)
    {
        return;
    }
    int historyStart = Assembly.force.historyFull ? Assembly.force.historyIndex : 0;

    float minValue = Assembly.force.history[historyStart];
    float maxValue = minValue;
    for (int i = 1; i < historyCount; i++)
    {
        float v = Assembly.force.history[(historyStart + i) % FORCE_HISTORY_SIZE];
        if (v < minValue)
        {
            minValue = v;
        }
        if (v > maxValue)
        {
            maxValue = v;
        }
    }
    float range = maxValue - minValue;

    float dx = 127.0f / (FORCE_HISTORY_SIZE - 1);
    int prevX = -1, prevY = -1;
    for (int i = 0; i < historyCount; i++)
    {
        float v = Assembly.force.history[(historyStart + i) % FORCE_HISTORY_SIZE];
        int age = historyCount - 1 - i; // 0 = newest sample
        int x = (int)(127.0f - age * dx + 0.5f);
        int y = (range > 0.0f)
                    ? GRAPH_BOTTOM - (int)((v - minValue) / range * GRAPH_HEIGHT + 0.5f)
                    : (GRAPH_TOP + GRAPH_HEIGHT / 2);

        if (prevX >= 0)
        {
            display.drawLine(prevX, prevY, x, y);
        }
        prevX = x;
        prevY = y;
    }

    // label the y-axis extremes (max top-left, min bottom-left), same idea as the web chart
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, GRAPH_TOP, String(maxValue, 1));
    display.drawString(0, GRAPH_BOTTOM - 9, String(minValue, 1));
}

void oledLoop()
{

    // drawStartScreen();

    display.clear();

    // Draw a line horizontally
    display.drawHorizontalLine(0, Y_OFFSET - 1, 128); // last yellow

    // Draw ip - the AP's own IP once no configured WiFi was found (no station IP to show then),
    // otherwise the station IP as usual
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, Assembly.apOnlyMode ? "AP:"+ Assembly.apIp : Assembly.localIp);

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

        drawForceGraph();
    }

    //  write the buffer to the display
    display.display();

    counter++;
}
