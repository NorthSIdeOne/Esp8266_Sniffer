
#include "Arduino.h"
#include "SnifferData.h"
#include "user_interface.h" //to gain access to promiscuous mode
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

void SnifferData::Sniff(){
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
}

SnifferData::SnifferData(String WifiSSID,String WifiPassword){
  this->WIFI_PASSWORD = WifiPassword;
  this->WIFI_SSID     = WifiSSID;
}
SnifferData::SnifferData(){
  
}

void SnifferData::StopSniff(){
   
  Serial.println("stop sniffing");
  wifi_promiscuous_enable(0);
  delay(500);
}

void SnifferData::ConnectToWifi(){
  Serial.println("Set AP");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD.c_str());
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID.c_str());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


  void SnifferData::getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

 String SnifferData::printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
  String ssid="";
  for(uint16_t i = start; i < DATA_LENGTH && i < start+size; i++) {
    Serial.write(data[i]);
    ssid =ssid+ (char)data[i];
  }
  return ssid;

}


 void SnifferData::showMetadata(uint8_t *buffer) {
  struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
  unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];

  uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
  uint8_t frameType    = (frameControl & 0b0000000000001100) >> 2;
  uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
  uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

  // Only look for probe request packets
  if (frameType != TYPE_MANAGEMENT ||
      frameSubType != SUBTYPE_PROBE_REQUEST)
        return;

  Serial.print("RSSI: ");
  Serial.print(snifferPacket->rx_ctrl.rssi, DEC);
  this->rssi.push_back((String)snifferPacket->rx_ctrl.rssi);

  Serial.print(" Ch: ");
  Serial.print(wifi_get_channel());
  this->ch.push_back((String)wifi_get_channel());

  char addr[] = "00:00:00:00:00:00";
  getMAC(addr, snifferPacket->data, 10);
  Serial.print(" Peer MAC: ");
  Serial.print(addr);
  this->macAddr.push_back(addr);
  //macs.insert(addr);

  uint8_t SSID_length = snifferPacket->data[25];
  Serial.print(" SSID: ");
  this->ssid.push_back(printDataSpan(26, SSID_length, snifferPacket->data));
  Serial.println();
}


void SnifferData::SendData(String URL){
  HTTPClient http;

  for(int i=0;i<this->macAddr.size();i++)
  {
    Serial.println("Send packet");
    String mainUrl = URL;
    mainUrl = mainUrl +"?&RSSI=" + this->rssi[i];
    mainUrl = mainUrl +"&CH="+this->ch[i];
    mainUrl = mainUrl +"&MAC="+this->macAddr[i];
    mainUrl = mainUrl +"&SSID="+this->ssid[i];
    http.begin(mainUrl);  //Specify request destination
    int code = http.GET();
    Serial.println(code);

  }
  
}

void SnifferData::ClearData(){
  this->ssid.clear();
  this->macAddr.clear();
  this->rssi.clear();
  this->ch.clear();
}