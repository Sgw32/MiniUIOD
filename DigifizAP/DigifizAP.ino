#include "network.h"
#include "commands.h"

// Board pins
constexpr uint8_t RELAY_PIN1 = 32;
constexpr uint8_t RELAY_PIN2 = 33;
constexpr uint8_t RELAY_PIN3 = 25;
constexpr uint8_t RELAY_PIN4 = 26;
constexpr uint8_t SPD_IN = 33;
constexpr uint8_t OIL18 = 35;

void setup() {
  Serial.begin(9600);

  setupRelaySensor();
  setupSPIFFS();
  setupWifiHost();
  startWebServer();

  pinMode(SPD_IN, INPUT);
  pinMode(OIL18, INPUT);
}

void loop() {
  digifizRead();
}
