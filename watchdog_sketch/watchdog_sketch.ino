#include <watchdog.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>  

#define WD_PIN A0 //for analog pin 0 (default)
//#define WD_PIN 4 //for digital pin 4 (optional)

int WD_DEBUG = 0; /* set to 1 to test the hardware watchdog.
                     set to 0 for production. DO NOT leave this as 1 
                     in production or output will eat memory and 
                     Arduino may endlessly reset! */

//the Arduino's IP and MAC
IPAddress my_ip(10, 42, 16, 222);
byte mac[] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x17 };

// Watchdog server IP and port
IPAddress wd_server_ip(10, 42, 16, 17);
const unsigned int wd_server_port = 6666;

Watchdog watchdog(my_ip, mac, wd_server_ip, wd_server_port, WD_PIN, WD_DEBUG);

// the setup routine runs once when you press reset:
void setup() {
  Ethernet.begin(mac, my_ip);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  if (WD_DEBUG == 1)
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


