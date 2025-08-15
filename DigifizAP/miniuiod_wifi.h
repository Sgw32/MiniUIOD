#pragma once

#ifndef ELEGANTOTA_USE_ASYNC_WEBSERVER
#define ELEGANTOTA_USE_ASYNC_WEBSERVER
#endif

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncWebSocketClient *lastClient;

void setupRelaySensor();
void setupSPIFFS();
void setupWifiHost();
void setupHTTPRoutes();
void startWebServer();
void otaLoop();
