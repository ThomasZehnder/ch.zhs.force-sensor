# avm-force-sensor-arduino
2'000N force sensor with arduino board

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

    //D3 = WS2812 data line (LEDS need 5V, signal only 3.3V works)

    //D4 is reserved for build In led -> show http activity
    
    // D7 used for IR presence sensor
    
#define KEY1_PIN D7
#define KEY2_PIN D8

#define FORCE_DOUT_PIN D6
#define FORCE_SCK_PIN D5


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

## Libraries

### Ticker

see [https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#ticker]


# Configure AVM Force Sensor

DEVICEID (String): Used as Identification of device and is used as prefix in the MQTT Topic definition.

ACCESSPOINT (bool): force to switch on accesspoint even WIFI is connected. 


### Sample Configuration

    {
        "DEVICEID": "force_sensor",
        "ACCESSPOINT": true
    }

## configure Wifi and Webserver

https://tttapa.github.io/ESP8266/Chap12%20-%20Uploading%20to%20Server.html

Lets inspire: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/FSBrowser/FSBrowser.ino

Configure three WLAN, in case none is connected, ESP8266 will act as WLAN Hotspot. 

### Upload Files to ESP8266

e.g. for Updating configuration or other files.
http://192.168.1.157/a-upload.html


### Reboot Arduino by REST Call
http://192.168.1.157/reboot

### File Directory Arduino by REST Call
http://192.168.1.157/dir


