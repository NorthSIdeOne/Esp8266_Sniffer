
#ifndef SnifferData_h
#define SnifferData_h

#include "Arduino.h"

class SnifferData{

private:

  std::vector<std::string> macAddr;
  std::vector<std::string> ssid;
  std::vector<std::string> rssi;
  std::vector<std::string> ch;
public:
  SnifferData();
  void Sniff();
};

#endif
