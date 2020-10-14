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


#define WIFI_SSID        "NodeTest" //SSID must not be hidden by the AP!
#define WIFI_PASSWORD    "11111111" //actually not necessary for clock sync

SnifferData sniffObj = SnifferData(WIFI_SSID,WIFI_PASSWORD);

static void ICACHE_FLASH_ATTR sniffer_callback(uint8_t *buffer, uint16_t length) {
  sniffObj.showMetadata(buffer);
}

void setup() {
  Serial.begin(115200);
  //Set the callback function
  wifi_set_promiscuous_rx_cb(sniffer_callback);
}

void loop() {

  //Start sniffing process
  sniffObj.Sniff();
  //Stop sniffing mode
  sniffObj.StopSniff();
  //Connect to a wifi network
  sniffObj.ConnectToWifi();
  //Send data to the URL with GET request
  sniffObj.SendData("http://10.10.16.215:8080/sniffer/connect.php");
  //Clear the vectors data
  sniffObj.ClearData();

}
