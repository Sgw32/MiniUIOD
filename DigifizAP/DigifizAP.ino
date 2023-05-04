#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "protocol.h"

// defining constants
const char* wifi_name = "DigifizAP"; // wifi network name
const char* wifi_pass = "87654321"; // wifi network password
const char* host = "digifiz"; // host name to access web

// defining board pins
int relay_pin1 = 32;
int relay_pin2 = 33;
int relay_pin3 = 25;
int relay_pin4 = 26;

// defining server
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

AsyncWebSocketClient * lastClient = NULL;
uint8_t parameter_pending = 0;
uint8_t parameter_next = 0;
//#define DEBUG_SERIAL

void setup() {
  // serial port for debugging purposes

  setupRelaySensor();
  setupSPIFFS();
  setupWifiHost();

  // sockets events
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  setupHTTPRoutes();

  // start server
  server.begin();
#ifdef DEBUG_SERIAL
  Serial.setDebugOutput(false);
#endif
  Serial.begin (9600);
  
}

void loop() {
  digifizRead();
  //delay(100);
}

void setupRelaySensor() {
  // initialize relay pins
  pinMode (relay_pin1, OUTPUT);
  pinMode (relay_pin2, OUTPUT);
  pinMode (relay_pin3, OUTPUT);
  pinMode (relay_pin4, OUTPUT);
}

void setupSPIFFS() {
  // initialize SPIFFS
  if (!SPIFFS.begin(true)) {
#ifdef DEBUG_SERIAL
    Serial.println("An Error has occurred while mounting SPIFFS");
#endif
    return;
  }

}

void setupWifiHost() {
  // connecting to wifi and getting ip
  WiFi.softAP(wifi_name, wifi_pass);
  IPAddress myIP = WiFi.softAPIP();
#ifdef DEBUG_SERIAL  
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void setupHTTPRoutes() {
  // main web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  // css and javascript
  server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
  });

  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css/style.css", "text/css");
  });

  server.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/jquery.min.js", "text/js");
  });

  server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/bootstrap.min.js", "text/js");
  });

  server.on("/js/gauge.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/gauge.min.js", "text/js");
  });

  server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/script.js", "text/js");
  });

  server.on("/digifiz.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/digifiz.png", "image/png");
  });
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  // websocket connect event
  if (type == WS_EVT_CONNECT) {
#ifdef DEBUG_SERIAL      
    Serial.println("Websocket client connection received");
#endif
    lastClient = client;

    // websocket disconnect event
  } else if (type == WS_EVT_DISCONNECT) {
#ifdef DEBUG_SERIAL      
    Serial.println("Websocket client connection finished");
#endif
    // websocket data receive event
  } else if (type == WS_EVT_DATA) {
    String dataReceived = "";
    for (int i = 0; i < len; i++) {
      dataReceived += (char) data[i];
    }
    processData(dataReceived);
  }

}

void processData(String data) {
  // checking if any client is connected
  if (lastClient != NULL && lastClient->status() == WS_CONNECTED) {
    // if id matches then toggle relays
    if (data == "mfareset") {
      Serial.print("28 0\n");
    }
    else if (data == "mfamode") {
      Serial.print("29 0\n");
    }
    else if (data == "mfablock") {
      Serial.print("30 0\n");
    }
    else if (data == "bckAutoBtn") {
      Serial.print("13 1\n");
    }
    else if (data == "bckManBtn") {
      Serial.print("13 0\n");
    }
    else if (data.startsWith("setTimeBtn")) {
      
    }
    else if (data.startsWith("setTCapBtn")) {
      
    }
    else if (data.startsWith("sendCmdBtn")) {
      
    }
    else if (data.startsWith("setBckBtn")) {
      
    }
    else if (data.startsWith("getDataBtn")) {
      Serial.print(String(PARAMETER_GET_FUEL_IN_TANK)+" 0\n");
      parameter_pending = PARAMETER_GET_FUEL_IN_TANK;
    }
    else if (data.startsWith("realtimeBtn")) {
      
    }
    else if (data == "startLogBtn") {
      
    }
    else if (data == "stopLogBtn") {
      
    }
    else if (data == "getLogBtn") {
      
    }
    else 
    {
      Serial.print(data+"\n"); //send cmd directly
    }
    
    #ifdef DEBUG_SERIAL      
        Serial.println(data);
    #endif
    // after changing state of relays forward to all clients to keep things synced
    sendDataToAllWS(data);
  }
}

void digifizRead() {
  if (Serial.available()>0)
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
#ifdef DEBUG_SERIAL
    Serial.println("CMD:"+ cmd);     
    Serial.println("end");
#endif
    if (cmd=="PARAMETER_GET_ACCUMULATED_UPTIME")
    {
      parameter_pending = PARAMETER_GET_ACCUMULATED_UPTIME;
    }
    else if (cmd=="PARAMETER_GET_COOLANT_TEMPERATURE")
    {
      parameter_pending = PARAMETER_GET_COOLANT_TEMPERATURE;
    }
    else if (cmd=="PARAMETER_GET_OIL_TEMPERATURE")
    {
      parameter_pending = PARAMETER_GET_OIL_TEMPERATURE;
    }
    else if (cmd=="PARAMETER_GET_AMBIENT_TEMPERATURE")
    {
      parameter_pending = PARAMETER_GET_AMBIENT_TEMPERATURE;
    }
    else if (cmd=="PARAMETER_GET_FUEL_IN_TANK")
    {
      parameter_pending = PARAMETER_GET_FUEL_IN_TANK;
    }
    else if (cmd=="PARAMETER_GET_SPEED")
    {
      parameter_pending = PARAMETER_GET_SPEED;
    }
    else if (cmd=="PARAMETER_GET_RPM")
    {
      parameter_pending = PARAMETER_GET_RPM;
    }
    else if (cmd=="PARAMETER_SET_DAY")
    {
      parameter_pending = PARAMETER_SET_DAY;
    }
    else if (cmd=="PARAMETER_SET_MONTH")
    {
      parameter_pending = PARAMETER_SET_MONTH;
    }
    else if (cmd=="PARAMETER_SET_YEAR")
    {
      parameter_pending = PARAMETER_SET_YEAR;
    }
    else if (cmd=="PARAMETER_GET_DAY")
    {
      parameter_pending = PARAMETER_GET_DAY;
    }
    else if (cmd=="PARAMETER_GET_MONTH")
    {
      parameter_pending = PARAMETER_GET_MONTH;
    }
    else if (cmd=="PARAMETER_GET_YEAR")
    {
      parameter_pending = PARAMETER_GET_YEAR;
    }
    else if (cmd=="PARAMETER_GET_HOUR")
    {
      parameter_pending = PARAMETER_GET_HOUR;
    }
    else if (cmd=="PARAMETER_GET_MINUTE")
    {
      parameter_pending = PARAMETER_GET_MINUTE;
    }
    else if (parameter_pending>0)
    {
      switch(parameter_pending)
      {
        case PARAMETER_GET_ACCUMULATED_UPTIME:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_ACCUMULATED_UPTIME");  
          #endif
          ws.textAll(String(PARAMETER_GET_ACCUMULATED_UPTIME)+": "+cmd);
          break;
        case PARAMETER_GET_COOLANT_TEMPERATURE:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_COOLANT_TEMPERATURE");  
          #endif
          ws.textAll(String(PARAMETER_GET_COOLANT_TEMPERATURE)+": "+cmd);
          break;
        case PARAMETER_GET_OIL_TEMPERATURE:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_OIL_TEMPERATURE");  
          #endif
          ws.textAll(String(PARAMETER_GET_OIL_TEMPERATURE)+": "+cmd);
          break;
        case PARAMETER_GET_AMBIENT_TEMPERATURE:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_AMBIENT_TEMPERATURE");  
          #endif
          ws.textAll(String(PARAMETER_GET_AMBIENT_TEMPERATURE)+": "+cmd);
          break;
        case PARAMETER_GET_FUEL_IN_TANK:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_FUEL_IN_TANK");  
          #endif
          ws.textAll(String(PARAMETER_GET_FUEL_IN_TANK)+": "+cmd);
          break;
        case PARAMETER_GET_SPEED:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_SPEED");  
          #endif
          ws.textAll(String(PARAMETER_GET_SPEED)+": "+cmd);
          break;
        case PARAMETER_GET_RPM:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_RPM");  
          #endif
          ws.textAll(String(PARAMETER_GET_RPM)+": "+cmd);
          break;
        case PARAMETER_GET_DAY:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_DAY");  
          #endif
          ws.textAll(String(PARAMETER_GET_DAY)+": "+cmd);
          break;
        case PARAMETER_GET_MONTH:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_MONTH");  
          #endif
          ws.textAll(String(PARAMETER_GET_MONTH)+": "+cmd);
          break;
        case PARAMETER_GET_YEAR:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_YEAR");  
          #endif
          ws.textAll(String(PARAMETER_GET_YEAR)+": "+cmd);
          break;
        case PARAMETER_GET_HOUR:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_HOUR");  
          #endif
          ws.textAll(String(PARAMETER_GET_HOUR)+": "+cmd);
          break;
        case PARAMETER_GET_MINUTE:
          #ifdef DEBUG_SERIAL
          Serial.println("ans PARAMETER_GET_MINUTE");  
          #endif
          ws.textAll(String(PARAMETER_GET_MINUTE)+": "+cmd);
          break;
        default:
          break;
      }
    }
    //Wait until send
    Serial.flush();
  }
}

void sendDataToAllWS(String dataReceived) {
  // sending data to all clients
  ws.textAll(dataReceived);
}
