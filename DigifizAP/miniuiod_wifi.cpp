#include "miniuiod_wifi.h"
#include "commands.h"
#include <ESPmDNS.h>
#include <ElegantOTA.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>

constexpr char WIFI_SSID[] = "DigifizAP";
constexpr char WIFI_PASS[] = "87654321";
constexpr char HOST_NAME[] = "digifiz";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
AsyncWebSocketClient *lastClient = NULL;

static unsigned long ota_progress_millis = 0;

static void onOTAStart() {
#ifdef DEBUG_SERIAL
  Serial.println("OTA update started!");
#endif
}

static void onOTAProgress(size_t current, size_t final) {
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
#ifdef DEBUG_SERIAL
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current,
                  final);
#endif
  }
}

static void onOTAEnd(bool success) {
#ifdef DEBUG_SERIAL
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
#endif
}

static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
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
  // pinMode (relay_pin1, OUTPUT);
  // pinMode (relay_pin2, OUTPUT);
  // pinMode (relay_pin3, OUTPUT);
  // pinMode (relay_pin4, OUTPUT);
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
  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  server.begin();
}

void otaLoop() { ElegantOTA.loop(); }
