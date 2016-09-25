# ESPWifiServer

## Description
Here the idea is, I am using ESP8266 wifi module to control appliances in a room using my home wifi network. But I was facing a issue that if I changed the ssid and wifi-password then I was not able to connect to my wifi. For the solution of this problem I thought of [OTA (Over the Air)](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html) update of my ESP8266 firmware(code) with updated password in it. For this I thought since I would not be able to connect to my wifi because of password change, let's use the ESP8266's (AP)access-point feature which creates its own hotspot. I can connect to access-point to update my ESP8266's firmware.
If you want a web server with **static ip** in your ESP8266 wifi module which is capable of creating access-point from which you can **update firmware** of your ESP8266, you can use this arduino code. Here **GPIO5** pin of ESP8266-12 module(`const int pin = 5;`) is used for controlling relay or leds. For that you have to do follwing changes in the code:
* First thing is, you will have to provide your wifi's **ssid** and **wifipassword** in the code.
```cpp
const char* ssid = "mywifi";
const char* password = "password";
```
* Then you will have to provide give a desired ssid and password for connecting to your ESP8266 through **AccessPoint** to update firmware. It can be anything.
```cpp
const char *esp_ssid = "your_desired_ssid";
const char *esp_password = "your_desired_password";
```
* Now this is for authorization before updating ESP8266's firmware.
```cpp
const char* www_username = "admin";
const char* www_password = "password";
```
* Get your gateway(or router) and subnet, then set the desired ip according to your gateway.
```cpp
IPAddress ip(192, 168, 1, xx); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 254); // set gateway to match your wifi network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your wifi network
```
* Now just use you ip, gateway and subnet to set static ip in `void setup()`.
```cpp
void setup(){
	......
	WiFi.config(ip, gateway, subnet);
	......
}
```
##Basic Requirements
Flash chip size should 2x the size of the sketch for using OTA update for your ESP8266 Wifi module.

## How to use
1. To control your home appliances, go to your web-browser,
  * Use http://your-ip/on to turn ON GPIO pins.
  * Use http://your-ip/off to turn OFF GPIO pins.
  * Use http://your-ip/status to get status(ON/OFF) of GPIO pins.
2. To update firmware(code) of your ESP8266 wifi module,
  * Use http://your-ip/fupdate to update firmware.
	* Give authorized user-name and password which you already set in the code.
	* After authorization ESP8266's access-point will enable.
	* Connect your computer to ESP8266's local hotspot network.
	* Choose your **firmware.bin file (example: updateHttpServer.ino.generic.bin)** to upload and click **Upload** button.
	* If firmware will be uploaded successfully, you will get **OK** message on your browser.
	* After uploading, restart your ESP8266 wifi module.

### Note:
> * If you went http://your-ip/fupdate and did authorization successfully but haven't uploaded new firmware then restart the ESP8266 module so the ESP8266's AP(access-point) mode will stop and don't consume unnecessary power.<br />
> * If firmware updation will **fail**, restart the ESP8266. It will re-run the old code and try to upload firmware again.<br />
> * Sometimes it takes 3-5 attempts to upload firmware successfully.


## Tools
You can download [ArdroidHttpClient](https://github.com/Arduino-and-RaspberryPi/ESPHttpClient/releases) to send request via HTTP. Or you can download or fork the project from [link](https://github.com/Arduino-and-RaspberryPi/ESPHttpClient.git) on **git-hub** and build **apk** using [Android Studio](https://developer.android.com/studio/index.html) and install to your android phone. You will have to provide a name(any name), your ESP8266 ip (the static ip) and port(as given in the code port is **80**). Now you can control your home appliances using the app.<br />
**This android application is not for updating firmware. You can use browser for that purpose.**

### Note:
> While using this code with ESP8266 wifi module, you phone or computer should be on same network.

## Connections:
<img src="https://github.com/Arduino-and-RaspberryPi/ESPWifiServer/blob/master/ESP-12Connection.jpg" width="540" height="360" />
