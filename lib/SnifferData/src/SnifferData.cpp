
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