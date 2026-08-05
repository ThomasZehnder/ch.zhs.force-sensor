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
        }
    }

    // 200ms tick
    if (hwForceSampleTick())
    {
        Force.loop();
        oledLoop();
    }

    // 50ms tick
    if (hwCentiSecoundTick())
    {
        

        pollKeyPressed();
        Assembly.processKeys();

        // 1s elapsed since the state was entered --> wait for key release before firing,
        // so the action doesn't run while the user is still pressing (e.g. shaking the sensor)
        bool actionDelayElapsed = (millis() - Assembly.stateStartMillis) >= 1000;
        if (actionDelayElapsed)
        {
            if (Assembly.state == StateTare && Assembly.keys[0].pressed)
            {
                Force.tare();
                Assembly.state = StateMeasure;
            }
            else if (Assembly.state == StateCalibrate && Assembly.keys[1].pressed)
            {
                Force.calibrate(1.0f * EARTH_GRAVITY_MPS2); // calibrate against a 1kg reference weight
                Assembly.state = StateMeasure;
            }
            else if (Assembly.state == StateReboot)
            {
                Serial.println("main.loop --> rebooting now");
                ESP.restart();
            }
        }
    }

    wifiLoop();

    httpServerLoop();
}