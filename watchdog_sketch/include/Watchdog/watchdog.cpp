#include "Arduino.h"
#include "watchdog.h"

Watchdog::Watchdog(IPAddress arduino_ip, byte arduino_mac[], IPAddress server_ip, int server_port, int pin, int wd_debug) {
  //copies over / initializes all the variables
  _watchdog_millis = millis();
  _arduino_ip[0] = arduino_ip[0];
  _arduino_ip[1] = arduino_ip[1];
  _arduino_ip[2] = arduino_ip[2];
  _arduino_ip[3] = arduino_ip[3];
  _arduino_mac[0] = arduino_mac[0];
  _arduino_mac[1] = arduino_mac[1];
  _arduino_mac[2] = arduino_mac[2];
  _arduino_mac[3] = arduino_mac[3];
  _arduino_mac[4] = arduino_mac[4];
  _arduino_mac[5] = arduino_mac[5];
  _server_ip[0] = server_ip[0];
  _server_ip[1] = server_ip[1];
  _server_ip[2] = server_ip[2];
  _server_ip[3] = server_ip[3];
  _server_port = server_port;
  _uptime = 0;
  _pin = pin;
  _wd_debug = wd_debug; /* Serial.println() eats memory and causes resets!
			   set this to 0 to silence Serial, unless you
			   are debugging. */
}

/* Because the arduino pins stay in the LAST state we need to make sure that
   the watchdog never gets out of sendMsg() with the pin low (i.e. draining
   the capacitor), or there's a chance that the arduino will crash in the 
   mainloop, the capacitor will never re-fill (because the pin is stuck low) 
   and the watchdog will fail to reset the arduino -- ever.
   
   This is why the pet is split into pet() and pet2(). One happens at the 
   beginning of sendMsg() to drop the pin low, one at the end to reset it high.
*/

void Watchdog:: pet()
{
  tmpmillis = millis();
  if( tmpmillis - last_pet > HW_TIME_TILPAT )
    {
      
      if (_wd_debug == 1) {
	// prints seconds of uptime
	Serial.println(millis() / 1000);
      }
      
      last_pet = millis();
      if(!pin_low)
	{
	  // bring it low
	  pinMode(_pin, OUTPUT);
	  digitalWrite(_pin, LOW);
  	  pin_low = true; //pin is now low
	}
    }
}

void Watchdog::pet2()
{
  if (pin_low)
    {
      /* this delay handles the patting timing for the case where the Ethernet shield is disconnected, physically or otherwise. Without this delay the timing won't work and you'll get endless resets. */
      if (!_client.connected()) {
	delay(50);
      }
      
      //if the pin is low we have NOT reset it to high, so do so.
      //otherwise nothing.
      // set to HIGH-Z
      pinMode(_pin, INPUT);
      pin_low = false; //pin is now high
    }
}

/* Sets initial variables. */
/* Needs to be called in the SETUP loop of your function. */
void Watchdog::setup() 
{
  last_pet = millis();
  pin_low = true;
  _reconnect_millis = 0;
  
  /* seems like a better idea to let the first attempt at connection happen 
     later on (after ~60 seconds). CML */
  
}

/* Checks to see if the Ethernet client is connected. If so, sends a message over the wire if at least 60 seconds have passed since last send. If not, tries to reconnect. Also increases uptime by 60 seconds. */

void Watchdog::sendMsg(char *msg) 
{
  // pet HW watchdog.
  // to do this you must call pet() before you do anything, then pet2()
  // before leaving this function (sendMsg).
  pet();
  
  // if client is connected, send message to watchdog server every 60 seconds
  if (_client.connected()) 
  {
    _curmillis = millis();
    if (_curmillis - _watchdog_millis > 60000) {
      _uptime += 60000;
      sprintf(_message, "%s %d.%d.%d.%d %lu ", msg, _arduino_ip[0], _arduino_ip[1],
	      _arduino_ip[2], _arduino_ip[3], _uptime/1000);
      
      _client.write(_message);
      
      if (_wd_debug == 1)
	Serial.println(_message);
      
      _watchdog_millis = millis();
    }
  } else {
    // if the client is not connected, wait 60 seconds before trying to
    // reconnect.
    _curmillis = millis();
    if (_curmillis - _watchdog_millis > 60000) {
      _uptime += 60000;
      _watchdog_millis = millis();
    }
  }
  
  // if the server's disconnected, try to reconnect every X milliseconds
  if (!_client.connected()) {
    if (millis() - _reconnect_millis > HW_RECONNECT_TIME) {
      int bytesFree;
      _reconnect_millis = millis();
      _client.stop();
      if (_wd_debug == 1)
	Serial.println(F("Recon"));
      if (_client.connect(_server_ip, _server_port)) {
	if (_wd_debug == 1)
	  Serial.println(F("Connd"));
      }
      else {
	// if you didn't get a connection to the server, say so
	if (_wd_debug == 1)
	  Serial.println(F("No Ethr"));
      }
    }
  }
  //pet HW watchdog, part 2
  pet2();
  
}
  

