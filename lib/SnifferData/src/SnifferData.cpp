
#include "Arduino.h"
#include "SnifferData.h"
#include "user_interface.h" //to gain access to promiscuous mode


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
