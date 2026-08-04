#include <Arduino.h>

#include "HttpServer.h"

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

}

void loop()
{

    hwLoop();



    // 50ms tick
    if (hwCentiSecoundTick())
    {
        oledLoop();


        pollKeyPressed();

    }


    wifiLoop();

    httpServerLoop();


}