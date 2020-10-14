#include <Arduino.h>
#include "SnifferData.h"
#ifdef ESP8266
extern "C" {
#include "user_interface.h" //to gain access to promiscuous mode
}
#endif

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

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

#include <set>



#define WIFI_SSID        "NodeTest" //SSID must not be hidden by the AP!
#define WIFI_PASSWORD    "11111111" //actually not necessary for clock sync

IPAddress ip(172, 24, 1, 91);
int port = 8888;



SnifferData sniffObj = SnifferData(WIFI_SSID,WIFI_PASSWORD);

std::set<String> macs;



// struct RxControl {
//   signed rssi: 8;
//   unsigned rate: 4;
//   unsigned is_group: 1;
//   unsigned: 1;
//   unsigned sig_mode: 2;
//   unsigned legacy_length: 12;
//   unsigned damatch0: 1;
//   unsigned damatch1: 1;
//   unsigned bssidmatch0: 1;
//   unsigned bssidmatch1: 1;
//   unsigned MCS: 7;
//   unsigned CWB: 1;
//   unsigned HT_length: 16;
//   unsigned Smoothing: 1;
//   unsigned Not_Sounding: 1;
//   unsigned: 1;
//   unsigned Aggregation: 1;
//   unsigned STBC: 2;
//   unsigned FEC_CODING: 1;
//   unsigned SGI: 1;
//   unsigned rxend_state: 8;
//   unsigned ampdu_cnt: 8;
//   unsigned channel: 4;
//   unsigned: 12;
// };

// struct SnifferPacket {
//   struct RxControl rx_ctrl;
//   uint8_t data[112];
//   uint16_t cnt;
//   uint16_t len;
// };



WiFiUDP Udp;

static void getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

static String printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
  String ssid="";
  for(uint16_t i = start; i < DATA_LENGTH && i < start+size; i++) {
    Serial.write(data[i]);
    ssid =ssid+ (char)data[i];
  }
  return ssid;

}
// static void showMetadata(SnifferPacket *snifferPacket) {

//   unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];

//   uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
//   uint8_t frameType    = (frameControl & 0b0000000000001100) >> 2;
//   uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
//   uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
//   uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

//   // Only look for probe request packets
//   if (frameType != TYPE_MANAGEMENT ||
//       frameSubType != SUBTYPE_PROBE_REQUEST)
//         return;

//   Serial.print("RSSI: ");
//   Serial.print(snifferPacket->rx_ctrl.rssi, DEC);

//   Serial.print(" Ch: ");
//   Serial.print(wifi_get_channel());

//   char addr[] = "00:00:00:00:00:00";
//   getMAC(addr, snifferPacket->data, 10);
//   Serial.print(" Peer MAC: ");
//   Serial.print(addr);
//   macs.insert(addr);

//   uint8_t SSID_length = snifferPacket->data[25];
//   Serial.print(" SSID: ");
//   printDataSpan(26, SSID_length, snifferPacket->data);
//   Serial.println();
// }






/**
 * Callback for promiscuous mode
 */
static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
  //struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
  sniffObj.showMetadata(buffer);
}
HTTPClient http;


void sendData(std::vector<std::string> v){

  for(auto mac : macs){
    //mac = mac+"\n";

    v.push_back(mac.c_str());
  }
  Serial.println(macs.size());
  for(int i=0;i<v.size();i++){
    Serial.println("Send packet");
    std::string macT = v.at(i);
    String sendData = "http://10.10.16.215:8080/sniffer/connect.php?&RSSI=-9&CH=7&MAC=";
    sendData=sendData+macT.c_str();
    sendData=sendData+"&SSID=Node";
    Serial.println(sendData);

    http.begin(sendData);  //Specify request destination
    int code = http.GET();
    Serial.println(code);
  }


}


int channel = 1;
void setup() {
  Serial.begin(115200);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
}
void sniff(){
  Serial.print("start sniffing channel :");
  int channel=1;
  while(channel < 14){
  Serial.println(channel);
  wifi_set_opmode(STATION_MODE);delay(10);
  wifi_set_channel(channel);delay(10);
  wifi_promiscuous_enable(0);delay(10);
//wifi_set_promiscuous_rx_cb(sniffer_callback);delay(10);
  wifi_promiscuous_enable(1);delay(10);
  channel++;
  delay(2000);
  }
  //  Serial.print("Stop sniffing sniffing");
}

void stopSniff(){
  Serial.println("stop sniffing");
  wifi_promiscuous_enable(0);
  //wifi_set_promiscuous_rx_cb(0);
  //wifi_promiscuous_enable(1);
  wifi_promiscuous_enable(0);

  delay(500);
}

void connectWifi(){
  Serial.println("Set AP");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//SnifferData sniffObj = SnifferData(WIFI_SSID,WIFI_PASSWORD);
void loop() {

  std::vector<std::string> v;
  //sniff();
  sniffObj.Sniff();
  //stopSniff();
  sniffObj.StopSniff();
  //connectWifi();
  sniffObj.ConnectToWifi();
  sendData(v);
  
  macs.clear();
  v.clear();
}
