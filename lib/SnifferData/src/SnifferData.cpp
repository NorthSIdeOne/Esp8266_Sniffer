

#include "Arduino.h"
#include "SnifferData.h"
#include "user_interface.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "string_utils.h"
#include "sdk_structs.h"
#include "ieee80211_structs.h"
/**
 * Start the sniffer
 * 
 */
void SnifferData::Sniff(){
  Serial.print("start sniffing channel :");
  int channel=1;
  unsigned long myTime = millis() + this->totalSnifftime;

  while(millis() < myTime){
    if(channel == 14)
      channel = 1;
 
  wifi_set_channel(channel);
  wifi_promiscuous_enable(1);
  channel++;
  delay(this->channelSniffTime);
  }
}
/**
 * Set the wifi ssid & password credentials
 */
SnifferData::SnifferData(String WifiSSID,String WifiPassword){
  this->WIFI_PASSWORD = WifiPassword;
  this->WIFI_SSID     = WifiSSID;
}
SnifferData::SnifferData(){
  
}

/**
 * Stop the sniffer and turn of promiscuous mode
 * 
 */
void SnifferData::StopSniff(){
  Serial.println("stop sniffing");
  wifi_promiscuous_enable(0);
  delay(500);
}

/**
 * Connects to wifi with WIFI_SSID & WIFI_PASSWORD.
 * 
 */
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

/**
 * Print the MAC address in standard format
 * 
 */
  void SnifferData::getMAC(char *addr, uint8_t* data, uint16_t offset) {
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset+0], data[offset+1], data[offset+2], data[offset+3], data[offset+4], data[offset+5]);
}

 String SnifferData::printDataSpan(uint16_t start, uint16_t size, uint8_t* data) {
  String ssid="";
  for(uint16_t i = start; i < DATA_LENGTH && i < start+size; i++) {
    ssid =ssid+ (char)data[i];
  }
  return ssid;

}

/**
 * Callback function that extract from the recived packet the following:
 * | MAC | RSSI | CH | SSID| and add it to mac,rssi,ch & ssid vectors.
 * 
 * @buffer: buffer
 **/
 void SnifferData::showMetadata(uint8_t *buffer,uint16_t length) {
   
  // struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;
  // unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];
  // char addr[] = "00:00:00:00:00:00";  
  
  // uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
  // uint8_t frameType    = (frameControl & 0b0000000000001100) >> 2;
  // uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
  // uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
  // uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

  // uint8_t SSID_length = snifferPacket->data[25];    

  // //Only look for probe request packets
  // if (frameType != TYPE_MANAGEMENT ||
  //     (frameSubType != SUBTYPE_PROBE_REQUEST && frameSubType != SUBTYPE_BEACON) )
  //       return;

  // //RSSI
  // this->rssi.push_back((String)snifferPacket->rx_ctrl.rssi);
  // //Chanel
  // this->ch.push_back((String)wifi_get_channel());
  // //MAC 
  // getMAC(addr, snifferPacket->data, 10);
  // this->macAddr.push_back(addr);
  // //SSID
  // if (frameSubType == SUBTYPE_BEACON)
  
  //  this->ssid.push_back(printDataSpan(26, SSID_length, snifferPacket->data));
  // else
  //  this->ssid.push_back("");
  
  // displayData(snifferPacket,addr);

// First layer: type cast the received buffer into our generic SDK structure
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buffer;
  // Second layer: define pointer to where the actual 802.11 packet is within the structure
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  // Third layer: define pointers to the 802.11 packet header and payload
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
  const uint8_t *data = ipkt->payload;

  // Pointer to the frame control section within the packet header
  const wifi_header_frame_control_t *frame_ctrl = (wifi_header_frame_control_t *)&hdr->frame_ctrl;

  // Parse MAC addresses contained in packet header into human-readable strings
  char addr1[] = "00:00:00:00:00:00\0";
  char addr2[] = "00:00:00:00:00:00\0";
  char addr3[] = "00:00:00:00:00:00\0";

  mac2str(hdr->addr1, addr1);
  mac2str(hdr->addr2, addr2);
  mac2str(hdr->addr3, addr3);


      if(frame_ctrl->to_ds == 1 && frame_ctrl->from_ds == 0 && (wifi_promiscuous_pkt_type_t)frame_ctrl->type == WIFI_PKT_DATA)
         anylizerToDsData(addr1,addr2,wifi_get_channel(),ppkt->rx_ctrl.rssi);
     else 
          if(frame_ctrl->to_ds == 0 && frame_ctrl->from_ds == 0)
            anylizerInternalData((wifi_mgmt_subtypes_t)frame_ctrl->subtype,addr1,addr2,addr3,wifi_get_channel(),ppkt->rx_ctrl.rssi,(wifi_mgmt_beacon_t*) ipkt->payload);
        

            
// if(frame_ctrl->to_ds == 1 && frame_ctrl->from_ds == 0){
  // Output info to serial
  // Serial.printf("\n%s | %s | %s | %u | %02d | %u | %u(%-2u) | %-28s | %u | %u | %u | %u | %u | %u | %u | %u | ",
  //   addr1,
  //   addr2,
  //   addr3,
  //   wifi_get_channel(),
  //   ppkt->rx_ctrl.rssi,
  //   frame_ctrl->protocol,
  //   frame_ctrl->type,
  //   frame_ctrl->subtype,
  //   wifi_pkt_type2str((wifi_promiscuous_pkt_type_t)frame_ctrl->type, (wifi_mgmt_subtypes_t)frame_ctrl->subtype),
  //   frame_ctrl->to_ds,
  //   frame_ctrl->from_ds,
  //   frame_ctrl->more_frag,
  //   frame_ctrl->retry,
  //   frame_ctrl->pwr_mgmt,
  //   frame_ctrl->more_data,
  //   frame_ctrl->wep,
  //   frame_ctrl->strict);

  // // Print ESSID if beacon
  // if (frame_ctrl->type == WIFI_PKT_MGMT && frame_ctrl->subtype == BEACON)
  // {
  //   const wifi_mgmt_beacon_t *beacon_frame = (wifi_mgmt_beacon_t*) ipkt->payload;
  //   char ssid[32] = {0};

    // if (beacon_frame->tag_length >= 32)
    // {
    //   strncpy(ssid, beacon_frame->ssid, 31);
    // }
    // else
    // {
    //   strncpy(ssid, beacon_frame->ssid, beacon_frame->tag_length);
    // }

  //   Serial.printf("%s", ssid);
  // }
}


void SnifferData::anylizerToDsData(String addr1,String addr2,int ch,int rssi){

    this->macAddr.push_back(addr2);
    this->rssi.push_back((String) rssi);
    this->ch.push_back((String)ch);
    this->ssid.push_back(ssidFinder(addr1).c_str());
    Serial.printf("\n CLIENT: %s | AP: %s | SSID: %s |",addr2.c_str(),addr1.c_str(),ssidFinder(addr1).c_str());

}


void SnifferData::anylizerInternalData(wifi_mgmt_subtypes_t subtype,String addr1,String addr2,String addr3,int ch,int rssi,wifi_mgmt_beacon_t *beacon_frame){

  switch(subtype)
      {
         case PROBE_REQ:
          {
              this->macAddr.push_back(addr2);
              this->rssi.push_back((String) rssi);
              this->ch.push_back((String)ch);
              this->ssid.push_back("Searching for AP");
              //Nu avem AP
          }
        //  case PROBE_RES:
        //  {
        //       this->macAddr.push_back(addr1);
        //       this->rssi.push_back((String) rssi);
        //       this->ch.push_back((String)ch);
        //      // this->ssid.push_back(ssidFinder(addr2).c_str());
        //      this->ssid.push_back("");
        //       //AP addr2
        //  }
         case BEACON:
        {
            char ssid[32] = {0};
            if (beacon_frame->tag_length >= 32)
            {
              strncpy(ssid, beacon_frame->ssid, 31);
            }
            else
            {
              strncpy(ssid, beacon_frame->ssid, beacon_frame->tag_length);
            }

          this->collectedSSID.insert(std::pair<String,String>(addr2,ssid)); 
        }

      }

}


String SnifferData::ssidFinder(String addr){

  if(this->collectedSSID.find(addr)->second == 0)
      return "";
  else
      return this->collectedSSID.find(addr)->second;


}
/**
 * Display data to the serial in the following format:
 * | MAC | RSSI | CH | SSID|
 * 
 * @snifferPacket: pointer of SnifferPacket type used to extract the RSSI
 * @addr:          MAC addres
 */
void SnifferData::displayData( SnifferPacket *snifferPacket,char addr[]){

  Serial.print("|  Peer MAC: ");
  Serial.print(addr);
  Serial.print(" |  RSSI: ");
  Serial.print(snifferPacket->rx_ctrl.rssi, DEC);
  Serial.print(" |  Ch: ");
  Serial.print(wifi_get_channel());
  Serial.print(" |  SSID: ");
  Serial.print(this->getSSID()[this->getSSID().size()-1]);
  Serial.print("  |  ");
  Serial.println();


}


/**
 * Create a json array for sending data to the server.
 * 
 * @mac:      hold all collected MACs.
 * @rssi:     hold all collected RSSI.
 * @ch:       hold all collected chanels.
 * @nodeName: hold the node name.
 * @nodeMac:  holde the MAC of the current node.
 * @return:  Return an json array.
 **/
String CreateJson(std::vector<String> mac ,std::vector<String> rssi,
                  std::vector<String> ch,std::vector<String>ssid, String nodeName , String nodeMac ){

  if(!(mac.size() > 0))
    return "[]";

  DynamicJsonDocument doc(2048);
  String json = "[";
  
  const int SIZE = mac.size(); 

  for(int i = 0; i < SIZE - 1 ;i++)
  {
    doc["MAC"]        = mac[i];
    doc["RSSI"]       = rssi[i];
    doc["CH"]         = ch[i];
    doc["SSID"]       = ssid[i];
    doc["NODE_NAME"]  = nodeName;
    doc["NODE_MAC"]   = nodeMac;

    serializeJson(doc,json);
    json +=",";
  }
    doc["MAC"]        = mac[SIZE - 1];
    doc["RSSI"]       = rssi[SIZE - 1];
    doc["CH"]         = ch[SIZE - 1];
    doc["SSID"]       = ssid[SIZE - 1];
    doc["NODE_NAME"]  = nodeName;
    doc["NODE_MAC"]   = nodeMac;
    
    Serial.println("#######################");
     Serial.println(nodeMac);
    Serial.println("#######################");
    serializeJson(doc,json);
    json +="]";

  return json;
}

/**
 * Send data to the server
 * 
 * @URL: the url for server post request
 */
void SnifferData::SendData(String URL){
  
    HTTPClient http;
    Serial.println("Send packet");

    http.begin(URL);
    http.addHeader("Content-Type", "application/json");
      
    //Send data to the server
    int code = http.POST(CreateJson(this->macAddr,this->rssi,this->ch,this->ssid,this->NODE_NAME, (String)WiFi.macAddress()));
    Serial.print(http.getString());
    Serial.println(code);
    http.end();  
}

/**
 * Delete all data from the class vectors:
 *   macAddr,ssid,rssi,ch. 
 */
void SnifferData::ClearData(){
  this->ssid.clear();
  this->ssid.shrink_to_fit();
  this->macAddr.clear();
  this->macAddr.shrink_to_fit();
  this->rssi.clear();
  this->rssi.shrink_to_fit();
  this->ch.clear();
  this->ch.shrink_to_fit();
 
}

/**
 * Set the channel hopping time.
 * 
 * @sniffTime: the time for channel hopping in ms. 
 */
void SnifferData::setChannelSniffTime(int sniffTime){
  this->channelSniffTime = sniffTime;
}

  /**
   * Get the MAC vector 
   */
  std::vector<String> SnifferData::getMacAddr(){
    return this->macAddr;
  }
  /**
  * Get the RSSI vector 
  */
  std::vector<String> SnifferData::getRSSI(){
    return this->rssi;
  }
  /**
  * Get the SSID vector 
  */
  std::vector<String> SnifferData::getSSID(){
    return this->ssid;
  }
  /**
  * Get the Channel vector 
  */
  std::vector<String> SnifferData::getCh(){
    return this->ch;
  }
/**
 * Get the server link
 */
  String SnifferData::getServerPostLink(){
    return this->serverPostLink;
  }

