#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

//ssid and password of your wifi network
const char* ssid = "wifi_ssid";
const char* password = "wifi_password";

//ssid and password to connect to local hotspot of ESP8266
const char *esp_ssid = "your_desired_ssid";
const char *esp_password = "your_desired_password";

IPAddress ip(192, 168, 1, xx); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 254); // set gateway to match your wifi network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your wifi network

ESP8266WebServer server(80);
int status = LOW;
const int pin = 5;

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

//Authorization username and password before updating firmware
const char* www_username = "admin";
const char* www_password = "esp8266";

void connectToWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while(WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //ESP.restart();
  }
  Serial.print("WiFi connected to ");
  Serial.println(WiFi.localIP());
}

void createAccessPoint() {
  WiFi.disconnect();
  delay(1000);
  WiFi.mode(WIFI_OFF);
  Serial.println("Configuring access point for wifi network *godawari*...");
  WiFi.softAP(esp_ssid, esp_password);
  IPAddress accessIP = WiFi.softAPIP();
  Serial.print("ESP AccessPoint IP address: ");
  Serial.println(accessIP);
  /* Go to http://192.168.4.1 in a web browser
   * connected to this access point to see it.
   */
}

void serverResponse(){
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", String(status));
}


void setup() {
  Serial.begin(115200);
  pinMode(pin, OUTPUT);
  connectToWifi();
  ArduinoOTA.begin();
  server.on("/fupdate", [](){
    if(!server.authenticate(www_username, www_password)){
      createAccessPoint();
      return server.requestAuthentication();
    }
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/status", [](){
    serverResponse();
  });
  server.on("/on", [](){
    status = HIGH;
    serverResponse();
  });
  server.on("/off", [](){
    status = LOW;
    serverResponse();
  });
  server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
  server.begin();
  MDNS.addService("http", "tcp", 80);

  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop() {
  digitalWrite(pin, status);
  ArduinoOTA.handle();
  server.handleClient();
  delay(1);
}
