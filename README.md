# zhs-force-sensor-arduino
2'000N force sensor with arduino board

## Features

- **Force measurement** via HX711 load cell amplifier, calibrated in Newton (tare / 1kg reference calibration)
- **Physical controls**: two buttons (Key0 = Tare, Key1 = Calibrate 1kg) with a 1s confirmation delay on the OLED before the action fires, so accidental or held presses don't trigger it
- **Rolling force history**: samples every 200ms, keeps the last 10s (50 values) in a ring buffer; cleared automatically on tare/calibrate
- **OLED display** (SSD1306 128x64): access point SSID, live force reading, and an autoscaled line graph of the last 50 force values in the lower half of the screen; dedicated screens for Tare/Calibrate/Reboot and an optional WiFi network scan at boot
- **Web frontend** served from LittleFS: Home page with a live-updating status table (force/offset/scale/state), Diagnose page, file browser & upload, and a CodeMirror-based JSON/JS config editor (lazy-loaded so the home page stays fast)
- **REST API**: `/assembly` (JSON with force value, forceHistory array, offset, scale, state, WiFi/key status), `/dir`, `/reboot`, `/json`
- **Browser caching**: static assets (HTML/CSS/JS/images) are cached for 5 days; the JSON REST endpoints and the editable `.json` config files are always sent with `Cache-Control: no-store`, so live data and configuration edits are never served stale
- **WiFi**: connects to up to 3 configured networks (WiFiMulti); falls back to its own Access Point (SSID = device ID + MAC suffix, IP `192.168.4.1`) if none connect at boot, or if forced via config. If none of the 3 configured networks was found at boot, the device commits to Access Point only mode and never retries again (until rebooted) - the OLED then shows the Access Point IP instead of a station IP. While a station connection is still possible, reconnect attempts are throttled to once every 60s, since a WiFi scan+connect attempt blocks the whole loop for several seconds - retrying every loop() iteration would otherwise stall force sampling almost permanently
- **Configuration** via `config_main.json` (DEVICEID, ACCESSPOINT, SCANNETWORKS, SCALE, OFFSET) and `config_wlan.json` (WLAN credentials), editable through the web UI

## Get from Git
    git clone tbd

## Used Hardware ESP8266
The ESP8266 requires a 3.3V power supply. 
see https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/

![ESP8266 Pinout](/images/ESP-12E-Development-Board-ESP8266-NodeMCU-Pinout.png)


### Wirering
    // Secound Timer
    #define TOGGLE_LED_PIN D0 

    //D1 = SDL for OLED display
    //D2 = SDA for OLED display

    #define KEY1_PIN D3 Zero Tara
    //D4 is reserved for build In led -> show http activity
    #define KEY2_PIN D5 Weight Tara

    #define FORCE_DOUT_PIN D6
    #define FORCE_SCK_PIN D7
    // D8 dont use, Boot feature on it


## Use PlatformIO
In VSCode see elements in the buttom left corner to transfer projekt to arduino board.

## Use Filesysten

https://randomnerdtutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/

SPIFFS is currently deprecated and may be removed in future releases of the core.

Use LittleFS instead

### Uploading Filesystem Image
After creating and saving the file or files you want to upload under the data folder, follow the next steps:

* Click the PIO icon at the left side bar. The project tasks should open.
* Select env:nodemcuv2 (it may different depending on the board you�re using).
* Expand the Platform menu.
* Select Build Filesystem Image.
* Finally, click Upload Filesystem Image.

Note: static files are served with `Cache-Control: public, max-age=432000` (5 days, see `handleFileRead()` in `HttpServer.cpp`). After uploading a changed static file (e.g. `index.html`, `style.css`), do a hard-refresh (or clear the browser cache) to see the update - `.json` config files are excluded from this and always served fresh.

## Libraries

### Ticker

see [https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#ticker]

### HX711 (vendored local fork)

`lib/HX711` is a local fork of `bogde/HX711@^0.7.5` (not fetched via `lib_deps`, see `platformio.ini`). Upstream's `read()` disables interrupts (`noInterrupts()`/`interrupts()`) for the duration of every 24-bit sample to protect the bit-banged timing. On the ESP8266 that collides with the WiFi stack's own interrupt-driven servicing and caused intermittent partial/aborted HTTP transfers while a measurement was in progress. This fork removes the interrupt-disable; the trade-off is a rare corrupted sample (visible as a brief spike) if a WiFi interrupt stretches the clock pulse past the HX711's 60us window - the chip just restarts its conversion cycle and the next read recovers on its own.


# Configure ZHS Force Sensor

DEVICEID (String): Used as Identification of device

ACCESSPOINT (bool): force to switch on accesspoint even WIFI is connected. 

SCALE (float) / OFFSET (long): HX711 calibration, persisted automatically whenever Tare (Key0) or Calibrate 1kg (Key1) is used, and loaded again on the next boot instead of re-taring blindly.


### Sample Configuration

    {
        "DEVICEID": "force_sensor",
        "ACCESSPOINT": true,
        "SCANNETWORKS" : false,
        "SCALE": 1.0,
        "OFFSET": 0.0
    }

## configure Wifi and Webserver

https://tttapa.github.io/ESP8266/Chap12%20-%20Uploading%20to%20Server.html

Lets inspire: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino

Configure three WLAN, in case none is connected, ESP8266 will act as WLAN Hotspot (SSID = device ID + MAC suffix, IP `192.168.4.1`).

If none of the three is found already at boot, the device commits to this Access Point only mode and stops trying to find a network - it will not reconnect on its own even if a configured network comes into range later; only a reboot re-triggers the connection attempt. The OLED reflects this by showing the Access Point IP (`192.168.4.1`) instead of a station IP.

**Security note**: `data/config_wlan.json` holds WLAN passwords in plain text and is listed in `.gitignore` - never force-add or commit it. If it ever does end up in git history, treat the passwords as compromised and rotate them; `git filter-branch`/`git filter-repo` can remove it from history afterwards, but that's a last resort, not a substitute for keeping it out in the first place.

### Upload Files to ESP8266

e.g. for Updating configuration or other files.
http://192.168.1.157/a-upload.html


### Reboot Arduino by REST Call
http://192.168.1.157/reboot

### File Directory Arduino by REST Call
http://192.168.1.157/dir


