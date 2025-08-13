#pragma once

#include <Arduino.h>

void processData(String data);
void digifizRead();
void sendDataToAllWS(const String &dataReceived);
