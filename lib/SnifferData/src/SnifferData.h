
#ifndef SnifferData_h
#define SnifferData_h

#include "Arduino.h"
#define DATA_LENGTH           112

#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02


#define SUBTYPE_ASSOC_REQUEST 0x00
#define SUBTYPE_ASSOC_RESP 0x01
#define SUBTYPE_REASSOC 0x02
#define SUBTYPE_REASSOC_RESP 0x03
#define SUBTYPE_PROBE_REQUEST 0x04
#define SUBTYPE_PROBE_RESPONSE 0x05
#define SUBTYPE_BEACON 0x08
#define SUBTYPE_ATIM 0x09
#define SUBTYPE_DISAC 0x0A
#define SUBTYPE_AUTH 0x0B
#define SUBTYPE_DEAUTH 0x0C


#define DISABLE 0
#define ENABLE  1
struct RxControl {
  signed rssi: 8;
  unsigned rate: 4;
  unsigned is_group: 1;
  unsigned: 1;
  unsigned sig_mode: 2;
  unsigned legacy_length: 12;
  unsigned damatch0: 1;
  unsigned damatch1: 1;
  unsigned bssidmatch0: 1;
  unsigned bssidmatch1: 1;
  unsigned MCS: 7;
  unsigned CWB: 1;
  unsigned HT_length: 16;
  unsigned Smoothing: 1;
  unsigned Not_Sounding: 1;
  unsigned: 1;
  unsigned Aggregation: 1;
  unsigned STBC: 2;
  unsigned FEC_CODING: 1;
  unsigned SGI: 1;
  unsigned rxend_state: 8;
  unsigned ampdu_cnt: 8;
  unsigned channel: 4;
  unsigned: 12;
};

struct SnifferPacket {
  struct RxControl rx_ctrl;
  uint8_t data[112];
  uint16_t cnt;
  uint16_t len;
};


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
  void getMAC(char *addr, uint8_t* data, uint16_t offset);
  String printDataSpan(uint16_t start, uint16_t size, uint8_t* data);
  void showMetadata(uint8_t *buffer);
  void SendData(String URL);
  void ClearData();
};

#endif
