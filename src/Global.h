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

struct tstForce
{
    float value; // measured force in Newton
    HX711 sensor;
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
    byte stateCountdown = 0; // seconds remaining until the pending action (tare/calibrate/reboot) fires

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
extern StaticJsonDocument<2048> doc;

#endif // GLOBAL_H