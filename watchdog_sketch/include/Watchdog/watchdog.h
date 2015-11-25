#ifndef Watchdog_h
#define Watchdog_h

#include "Arduino.h"
#include <Ethernet.h>
#include <EthernetUdp.h>

#define HW_TIME_TILPAT 5000 // 5 seconds between pats
#define HW_SENDMSG_TIME 60000 // 60 seconds between OKAY messages

class Watchdog
{
  public:
  Watchdog(IPAddress arduino_ip, byte arduino_mac[], IPAddress server_ip, int server_port, int pin, int wd_debug);
  void sendMsg(char *msg); //sends "msg" over Ethernet with IP and uptime
  void setup(); //initializes variables & connection
  
  // hardware watchdog. Two-part, you must call both parts within ONE function.
  // don't call them in separate functions, because your stuck Arduino may
  // never reset!
  void pet();
  void pet2();

  private:
  //software watchdog variables
  EthernetUDP _client; //UDP connection for the watchdog server
  unsigned long _uptime; //total uptime. Figured by adding 120 sec each msgSend
  unsigned long _watchdog_millis; //total milliseconds since the last msgSend
  unsigned long _curmillis; //global var for msgSend (current ms)

  IPAddress _arduino_ip; //IPAddress for the arduino
  byte _arduino_mac[]; //mac address for the arduino
  IPAddress _server_ip; //IPAddress for the server
  int _server_port; //port number for the server
  char _message[300]; //global var for msgSend
  int _pin; //Arduino pin
  int _wd_debug; //print to Serial if 1, if 0 don't
  
  //hardware watchdog variables
  unsigned long last_pet; //holds the last time we pet
  bool pin_low; //signals whether to pull the pin low
}; 

#endif
