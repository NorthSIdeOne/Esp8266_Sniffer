
#ifndef SnifferData_h
#define SnifferData_h 

#include "string_utils.h"
#include "sdk_structs.h"
#include "ieee80211_structs.h"
#include "Arduino.h"
#include "SnifferDataDefine.h"
#include <ArduinoJson.h>
#include <map>
class SnifferData{

private:

  std::vector<String> macAddr;
  std::vector<String> ssid;
  std::vector<String> rssi;
  std::vector<String> ch;
  std::map<String,String> collectedSSID;
  String NODE_NAME = "NodeMCU";
  String serverPostLink = "http://192.168.100.36:5000/api/sensor_data";
  String WIFI_SSID;
  String WIFI_PASSWORD;
  int channelSniffTime = 10;
  int totalSnifftime = 25000;

public:

  SnifferData(String WifiSSID,String WifiPassword);
  SnifferData();
  void Sniff();
  void StopSniff();
  void ConnectToWifi();
  void getMAC(char *addr, uint8_t* data, uint16_t offset);
  String printDataSpan(uint16_t start, uint16_t size, uint8_t* data);
  void showMetadata(uint8_t *buffer,uint16_t length);
  void displayData(SnifferPacket *snifferPacket,char addr[]);
  void SendData(String URL);
  void ClearData();
  void setChannelSniffTime(int sniffTime);
  void anylizerToDsData(String addr1,String addr2,int ch,int rssi);
  void anylizerInternalData(wifi_mgmt_subtypes_t subtype,String addr1,String addr2,String addr3,int ch,int rssi,wifi_mgmt_beacon_t *beacon_frame);
  String ssidFinder(String addr);
  std::vector<String> getMacAddr();
  std::vector<String> getRSSI();
  std::vector<String> getSSID();
  std::vector<String> getCh();
  String getServerPostLink();
};

#endif
