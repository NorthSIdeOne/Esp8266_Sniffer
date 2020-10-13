
#ifndef SnifferData_h
#define SnifferData_h

#include "Arduino.h"

class SnifferData{

private:

  std::vector<std::string> macAddr;
  std::vector<std::string> ssid;
  std::vector<std::string> rssi;
  std::vector<std::string> ch;
  String WIFI_SSID;
  String WIFI_PASSWORD;
public:
  SnifferData(String WifiSSID,String WifiPassword);
  SnifferData();
  void Sniff();
  void StopSniff();
  void ConnectToWifi();
};

#endif
