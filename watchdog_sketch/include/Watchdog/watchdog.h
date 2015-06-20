#ifndef Watchdog_h
#define Watchdog_h

#include "Arduino.h"
#include <Ethernet.h>
#include <EthernetUdp.h>

#define HW_WATCHDOG_PIN 4
#define HW_TIME_THRESHOLD 300
#define HW_TIME_TILPAT 5000
#define HW_RECONNECT_TIME 60000

class Watchdog
{
  public:
  Watchdog(IPAddress arduino_ip, byte arduino_mac[], IPAddress server_ip, int server_port);
  void sendMsg(char *msg); //sends "msg" over Ethernet with IP and uptime
  void setup(); //initializes variables & connection
  
  // hardware watchdog
  void pet();
  void pet2();

  private:
  EthernetClient _client;
  unsigned long _uptime; //total uptime. Figured by adding 120 sec each msgSend
  unsigned long _watchdog_millis; //total milliseconds since the last msgSend
  unsigned long _curmillis; //global var for msgSend
  unsigned long _reconnect_millis = 0; //total ms since the last reconnect attempt
  unsigned long tmpmillis;

  IPAddress _arduino_ip; //IPAddress for the arduino
  byte _arduino_mac[]; //mac address for the arduino
  IPAddress _server_ip; //IPAddress for the server
  int _server_port; //port number for the server
  char _message[300]; //global var for msgSend

  //hardware watchdog variables
  unsigned long last_pet;
  bool pin_low;

}; 

#endif
