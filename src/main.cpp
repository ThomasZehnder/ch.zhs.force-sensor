#include <Arduino.h>

#include "HttpServer.h"

#include "Force.h"
#include "HwInterface.h"
#include "Oled.h"
#include "Wifi.h"

#include "Global.h"

void setup()
{
    Serial.begin(115200);
    Serial.println();

    Assembly.setup(); // read config file

    oledSetup();

    // scanNetworks(); // call before httpSetup, show result in serial out
    oledShowNetworks(); // show result on OLED

    wifiSetup();

    httpServerSetup(); // will not longer block until WLAN connected

    hwSetup();

    Force.setup();
}

void loop()
{

    hwLoop();

    

    if (hwSecoundTick())
    {
        if (Assembly.state == StateSetup)
        {
            Assembly.state = StateMeasure;
        };
    }

    // 50ms tick
    if (hwCentiSecoundTick())
    {
        Force.loop();
        oledLoop();

        pollKeyPressed();
        Assembly.processKeys();
    }

    wifiLoop();

    httpServerLoop();
}