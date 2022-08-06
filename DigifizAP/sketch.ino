#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"

// defining constants
const char* wifi_name = "DigifizAP"; // wifi network name
const char* wifi_pass = "87654321"; // wifi network password
const char* host = "digifz"; // host name to access web

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


void setup() {
  // serial port for debugging purposes
  Serial.begin (115200);

  
  setupRelaySensor();
  setupSPIFFS();
  setupWifiHost();

  // sockets events
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  setupHTTPRoutes();

  // start server
  server.begin();
}

void loop() {
  dhtRead();
  delay(1000);
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
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

}

void setupWifiHost() {
  // connecting to wifi and getting ip
  WiFi.softAP(wifi_name, wifi_pass);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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

}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  // websocket connect event
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    lastClient = client;

    // checking state of relays and forwarding to all clients
    if (digitalRead(relay_pin1) == LOW) {
      sendDataToAllWS("on-1");
    } else if (digitalRead(relay_pin1) == HIGH) {
      sendDataToAllWS("off-1");
    }

    if (digitalRead(relay_pin2) == LOW) {
      sendDataToAllWS("on-2");
    } else if (digitalRead(relay_pin2) == HIGH) {
      sendDataToAllWS("off-2");
    }

    if (digitalRead(relay_pin3) == LOW) {
      sendDataToAllWS("on-3");
    } else if (digitalRead(relay_pin3) == HIGH) {
      sendDataToAllWS("off-3");
    }

    if (digitalRead(relay_pin4) == LOW) {
      sendDataToAllWS("on-4");
    } else if (digitalRead(relay_pin4) == HIGH) {
      sendDataToAllWS("off-4");
    }

    // websocket disconnect event
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Websocket client connection finished");

    // websocket data receive event
  } else if (type == WS_EVT_DATA) {
    String dataReceived = "";
    for (int i = 0; i < len; i++) {
      dataReceived += (char) data[i];
    }
    toggleRelay(dataReceived);
  }

}

void toggleRelay(String relayId) {
  // checking if any client is connected
  if (lastClient != NULL && lastClient->status() == WS_CONNECTED) {
    // if id matches then toggle relays
    if (relayId == "on-1") {
      digitalWrite(relay_pin1, LOW);
    }
    if (relayId == "off-1") {
      digitalWrite(relay_pin1, HIGH);
    }
    if (relayId == "on-2") {
      digitalWrite(relay_pin2, LOW);
    }
    if (relayId == "off-2") {
      digitalWrite(relay_pin2, HIGH);
    }
    if (relayId == "on-3") {
      digitalWrite(relay_pin3, LOW);
    }
    if (relayId == "off-3") {
      digitalWrite(relay_pin3, HIGH);
    }
    if (relayId == "on-4") {
      digitalWrite(relay_pin4, LOW);
    }
    if (relayId == "off-4") {
      digitalWrite(relay_pin4, HIGH);
    }

    // after changing state of relays forward to all clients to keep things synced
    sendDataToAllWS(relayId);
  }

}

void dhtRead() {
  // reading temperature and humidity values
  

  // calculating heat index
  float heatIndex = 10.0;

  // forwarding respective values to clients
  sendDataToAllWS("T: " + String(27.0));
  sendDataToAllWS("H: " + String(60.0));
  sendDataToAllWS("I: " + String(heatIndex));
}

void sendDataToAllWS(String dataReceived) {
  // sending data to all clients
  ws.textAll(dataReceived);
}

// coder: Shanezzar (www.shanezzar.com)
// dated: 07-05-2020
