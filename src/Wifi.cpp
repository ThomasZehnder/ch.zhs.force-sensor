#include <ESP8266WiFi.h>
#include <Ticker.h>

#include "credentials.h"

#include "Global.h"

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
// Ticker wifiReconnectTimer;  //Wifi is handled in Http Context

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("onWifiConnect (CallBack) --> Connected to WiFi... " + WiFi.SSID());
  Serial.print("onWifiConnect (CallBack) --> IP address: ");
  Serial.println(WiFi.localIP());
  Assembly.wifiConnected = true;
  Assembly.localIp = WiFi.localIP().toString();
  Assembly.ssid = WiFi.SSID();

  // select try all coonfigurations
  byte cfgIndex = 0;
  for (cfgIndex = 0; cfgIndex < (sizeof(Assembly.cfg.wifi) / sizeof(Assembly.cfg.wifi[0])); cfgIndex++)
  {
    if (WiFi.SSID() == Assembly.cfg.wifi[cfgIndex].ssid)
    {
      Serial.print("onWifiConnect (CallBack) --> use cfg index: ");
      Serial.println(cfgIndex);
      Assembly.cfg.index = cfgIndex; // store for httpRest

      break;
    }
  }

}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("WifiSetup (CallBack) --> Disconnected from WiFi.");
  Assembly.wifiConnected = false;
  // wifiReconnectTimer.once(2, connectToWifi);
}

void wifiSetup()
{
  Serial.println("WifiSetup --> Start");
  // the SDK's own auto-reconnect otherwise retries a targeted SSID every ~2s on its own,
  // independent of and in addition to wifiMulti.run() in HttpServer.cpp - disable it so
  // reconnect attempts only happen on wifiMulti's own (throttled) schedule
  WiFi.setAutoReconnect(false);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  Serial.println("WifiSetup --> End");
}


void wifiLoop()
{
  // nothing, all "event based"
}
