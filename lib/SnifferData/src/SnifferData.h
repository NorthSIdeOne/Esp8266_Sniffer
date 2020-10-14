
#ifndef SnifferData_h
#define SnifferData_h

#include "Arduino.h"

class SnifferData{

private:

  std::vector<String> macAddr;
  std::vector<String> ssid;
  std::vector<String> rssi;
  std::vector<String> ch;
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
