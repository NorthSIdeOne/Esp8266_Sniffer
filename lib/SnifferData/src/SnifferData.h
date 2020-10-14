
#ifndef SnifferData_h
#define SnifferData_h

#include "Arduino.h"
#include "SnifferDataDefine.h"


class SnifferData{

private:

  std::vector<String> macAddr;
  std::vector<String> ssid;
  std::vector<String> rssi;
  std::vector<String> ch;
  String WIFI_SSID;
  String WIFI_PASSWORD;
  int channelSniffTime = 2000;

public:

  SnifferData(String WifiSSID,String WifiPassword);
  SnifferData();
  void Sniff();
  void StopSniff();
  void ConnectToWifi();
  void getMAC(char *addr, uint8_t* data, uint16_t offset);
  String printDataSpan(uint16_t start, uint16_t size, uint8_t* data);
  void showMetadata(uint8_t *buffer);
  void SendData(String URL);
  void ClearData();
  void setChannelSniffTime(int sniffTime);
  std::vector<String> getMacAddr();
  std::vector<String> getRSSI();
  std::vector<String> getSSID();
  std::vector<String> getCh();
};

#endif
