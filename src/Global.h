#ifndef GLOBAL_H
#define GLOBAL_H
// global object definition
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HX711.h>

#define CPP_VERSION "V1.00.00 "

struct tstCfgWifi
{
    char ssid[32];
    char pw[32];
};

#define NBRCONNECTIONS 3
struct tstCfg
{
    tstCfgWifi wifi[NBRCONNECTIONS];
    bool accessPointEnabled = true;
    bool scanNetworks = false; // configurable via config_main.json, SCANNETWORKS
    byte index = 0; // index used for http and mqtt sebd to cloud, depends on found WIFI Network
};

#define NBRKEYS 2
struct tstKey
{
    int pin;
    bool edge;
    bool pressed;
    int pressedCounter;
};

// rolling history at 200ms sampling interval, covering the last 10 seconds
#define FORCE_HISTORY_SIZE 50
struct tstForce
{
    float value; // measured force in Newton
    HX711 sensor;

    float history[FORCE_HISTORY_SIZE] = {0}; // circular buffer, oldest to newest via historyIndex/historyFull
    int historyIndex = 0;                    // next slot to write
    bool historyFull = false;                // true once the buffer has wrapped around at least once
};

enum enMainState
{
    StateSetup,
    StateMeasure,
    StateTare,
    StateCalibrate,
    StateReboot
};

class clAssembly
{
public:
    String compileDate;
    char deviceId[32];

    bool wifiConnected = false;

    String localIp;
    String ssid;

    struct tstKey keys[NBRKEYS];

    tstCfg cfg;

    tstForce force;

    enMainState state = StateSetup;
    unsigned long stateStartMillis = 0; // millis() timestamp when the current state was entered

public:
    void setup();

    void rebootProcess();
    void wlanConnectedProcess();

    void processKeys();

private:
    void setupDevice();
    void setupWifi();
};

extern clAssembly Assembly;

// Allocate a static JsonDocument
// Use https://arduinojson.org/v6/assistant to compute the capacity.
extern StaticJsonDocument<4096> doc;

#endif // GLOBAL_H