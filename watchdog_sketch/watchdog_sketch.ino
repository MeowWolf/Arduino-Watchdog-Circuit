#include <watchdog.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>  

//the Arduino's IP and MAC
IPAddress my_ip(192, 168, 1, 222);
byte mac[] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x17 };

// Watchdog server IP and port. The Watchdog won't connect for the HW Watchdog
// test, but we need to set this up anyway
IPAddress wd_server_ip(192, 168, 1, 17);
const unsigned int wd_server_port = 6666;

Watchdog watchdog(my_ip, mac, wd_server_ip, wd_server_port);

// the setup routine runs once when you press reset:
void setup() {
  Ethernet.begin(mac, my_ip);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);  
  Serial.println("Arduino reset");
  
  // Send an initial heartbeat.
  watchdog.setup();
}

// the loop routine runs over and over again forever:
void loop() {

  watchdog.sendMsg("ERRPI_ACKCLEAR");

  // delay in between loops
  delay(100);
}


