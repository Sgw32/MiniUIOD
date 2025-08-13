#include "network.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <AsyncElegantOTA.h>
#include "commands.h"

constexpr char WIFI_SSID[] = "DigifizAP";
constexpr char WIFI_PASS[] = "87654321";
constexpr char HOST_NAME[] = "digifiz";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
AsyncWebSocketClient *lastClient = NULL;

static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
#ifdef DEBUG_SERIAL
    Serial.println("Websocket client connection received");
#endif
    lastClient = client;
  } else if (type == WS_EVT_DATA) {
    String dataReceived;
    for (size_t i = 0; i < len; i++) {
      dataReceived += (char)data[i];
    }
    processData(dataReceived);
  }
}

void setupRelaySensor() {
  // initialize relay pins
  //pinMode (relay_pin1, OUTPUT);
  //pinMode (relay_pin2, OUTPUT);
  //pinMode (relay_pin3, OUTPUT);
  //pinMode (relay_pin4, OUTPUT);
}

void setupSPIFFS() {
  if (!SPIFFS.begin(true)) {
#ifdef DEBUG_SERIAL
    Serial.println("An Error has occurred while mounting SPIFFS");
#endif
  }
}

void setupWifiHost() {
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  if (MDNS.begin(HOST_NAME)) {
    MDNS.addService("http", "tcp", 80);
  }
#ifdef DEBUG_SERIAL
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(myIP);
#endif
}

void setupHTTPRoutes() {
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
}

void startWebServer() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  setupHTTPRoutes();
  AsyncElegantOTA.begin(&server);
  server.begin();
}
