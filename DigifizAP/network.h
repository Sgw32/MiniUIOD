#pragma once

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncWebSocketClient *lastClient;

void setupRelaySensor();
void setupSPIFFS();
void setupWifiHost();
void setupHTTPRoutes();
void startWebServer();
