#include "commands.h"
#include "network.h"
#include "protocol.h"

int getAllDataFlag = 0;
uint8_t parameter_pending = 0;
uint8_t parameter_next = 0;

struct CommandMap {
  const char *cmd;
  uint8_t param;
};

static const CommandMap commandMap[] = {
  {"PARAMETER_GET_ACCUMULATED_UPTIME", PARAMETER_GET_ACCUMULATED_UPTIME},
  {"PARAMETER_GET_COOLANT_TEMPERATURE", PARAMETER_GET_COOLANT_TEMPERATURE},
  {"PARAMETER_GET_OIL_TEMPERATURE", PARAMETER_GET_OIL_TEMPERATURE},
  {"PARAMETER_GET_AMBIENT_TEMPERATURE", PARAMETER_GET_AMBIENT_TEMPERATURE},
  {"PARAMETER_GET_FUEL_IN_TANK", PARAMETER_GET_FUEL_IN_TANK},
  {"PARAMETER_GET_SPEED", PARAMETER_GET_SPEED},
  {"PARAMETER_GET_RPM", PARAMETER_GET_RPM},
  {"PARAMETER_SET_DAY", PARAMETER_SET_DAY},
  {"PARAMETER_SET_MONTH", PARAMETER_SET_MONTH},
  {"PARAMETER_SET_YEAR", PARAMETER_SET_YEAR},
  {"PARAMETER_GET_DAY", PARAMETER_GET_DAY},
  {"PARAMETER_GET_MONTH", PARAMETER_GET_MONTH},
  {"PARAMETER_GET_YEAR", PARAMETER_GET_YEAR},
  {"PARAMETER_GET_HOUR", PARAMETER_GET_HOUR},
  {"PARAMETER_GET_MINUTE", PARAMETER_GET_MINUTE},
  {"PARAMETER_GET_GPIO_PINS", PARAMETER_GET_GPIO_PINS}
};

static uint8_t mapCommand(const String &cmd) {
  for (const auto &entry : commandMap) {
    if (cmd == entry.cmd) {
      return entry.param;
    }
  }
  return 0;
}

static void handleParameterResponse(const String &cmd) {
  switch (parameter_pending) {
    case PARAMETER_GET_ACCUMULATED_UPTIME:
      ws.textAll(String(PARAMETER_GET_ACCUMULATED_UPTIME) + ": " + cmd);
      break;
    case PARAMETER_GET_COOLANT_TEMPERATURE:
      ws.textAll(String(PARAMETER_GET_COOLANT_TEMPERATURE) + ": " + cmd);
      break;
    case PARAMETER_GET_OIL_TEMPERATURE:
      ws.textAll(String(PARAMETER_GET_OIL_TEMPERATURE) + ": " + cmd);
      break;
    case PARAMETER_GET_AMBIENT_TEMPERATURE:
      ws.textAll(String(PARAMETER_GET_AMBIENT_TEMPERATURE) + ": " + cmd);
      break;
    case PARAMETER_GET_FUEL_IN_TANK:
      ws.textAll(String(PARAMETER_GET_FUEL_IN_TANK) + ": " + cmd);
      break;
    case PARAMETER_GET_SPEED:
      ws.textAll(String(PARAMETER_GET_SPEED) + ": " + cmd);
      break;
    case PARAMETER_GET_RPM:
      ws.textAll(String(PARAMETER_GET_RPM) + ": " + cmd);
      break;
    case PARAMETER_GET_DAY:
      ws.textAll(String(PARAMETER_GET_DAY) + ": " + cmd);
      break;
    case PARAMETER_GET_MONTH:
      ws.textAll(String(PARAMETER_GET_MONTH) + ": " + cmd);
      break;
    case PARAMETER_GET_YEAR:
      ws.textAll(String(PARAMETER_GET_YEAR) + ": " + cmd);
      break;
    case PARAMETER_GET_HOUR:
      ws.textAll(String(PARAMETER_GET_HOUR) + ": " + cmd);
      break;
    case PARAMETER_GET_MINUTE:
      ws.textAll(String(PARAMETER_GET_MINUTE) + ": " + cmd);
      break;
    case PARAMETER_GET_GPIO_PINS:
      ws.textAll(String(PARAMETER_GET_GPIO_PINS) + ": " + cmd);
      break;
    default:
      break;
  }
  parameter_pending = 0;
}

void processData(String data) {
  if (lastClient != NULL && lastClient->status() == WS_CONNECTED) {
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
      String hour = data.substring(10,2);
      String minute = data.substring(12,2);
      Serial.print("255 "+hour+"\n");
      Serial.flush();
      Serial.print("254 "+minute+"\n");
    }
    else if (data.startsWith("setTCapBtn")) {
      String tcap = data.substring(10,2);
      Serial.print("15 "+tcap+"\n");
    }
    else if (data.startsWith("sendCmdBtn")) {

    }
    else if (data.startsWith("setBckBtn")) {
      String bck = data.substring(9,2);
      Serial.print("14 "+bck+"\n");
    }
    else if (data.startsWith("getDataBtn")) {
      //Serial.print(String(PARAMETER_GET_FUEL_IN_TANK)+" 0\n");
      //parameter_pending = PARAMETER_GET_FUEL_IN_TANK;
      parameter_pending = PARAMETER_GET_MINUTE;
      Serial.print(String(parameter_pending)+" 0\n");
      getAllDataFlag = 1;
    }
    else if (data == "startLogBtn") {

    }
    else if (data == "stopLogBtn") {

    }
    else if (data == "getLogBtn") {

    }
    else
    {
      String par = data.substring(0,data.indexOf(' '));
      parameter_pending = par.toInt();
#ifdef DEBUG_SERIAL
        Serial.println("par pending:"+par);
#endif
      Serial.print(data+"\n"); //send cmd directly
    }

#ifdef DEBUG_SERIAL
        Serial.println(data);
#endif
    sendDataToAllWS(data);
  }
}

void digifizRead() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
#ifdef DEBUG_SERIAL
    Serial.println("CMD:" + cmd);
    Serial.println("end");
#endif
    uint8_t mapped = mapCommand(cmd);
    if (mapped) {
      parameter_pending = mapped;
    } else if (parameter_pending > 0) {
      handleParameterResponse(cmd);
    }
    Serial.flush();
  }
}

void sendDataToAllWS(const String &dataReceived) {
  ws.textAll(dataReceived);
}

