#include "Arduino.h"
#include "watchdog.h"

Watchdog::Watchdog(IPAddress arduino_ip, byte arduino_mac[], IPAddress server_ip, int server_port, int pin, int wd_debug) {
  //copies over / initializes all the variables
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
  _watchdog_millis = millis();
  _wd_debug = wd_debug; /* Serial.println() eats memory and can cause resets!
			   pass in a 0 to silence Serial output, unless you
			   are debugging or testing the HW watchdog. */
}

/* Because the arduino pins stay in the LAST state we need to make sure that
   the watchdog never gets out of sendMsg() with the pin low (i.e. draining
   the capacitor), or there's a chance that the arduino will crash in the 
   main loop, the capacitor will never re-fill (because the pin is stuck low) 
   and the watchdog will fail to reset the arduino -- ever.
   
   This is why the pet is split into pet() and pet2(). One part happens at the 
   beginning of sendMsg() to drop the pin low, one at the end to reset it high.
*/

void Watchdog:: pet()
{
  _curmillis = millis();
  if( _curmillis - last_pet > HW_TIME_TILPAT )
    {
      
      if (_wd_debug == 1) {
	// prints seconds of uptime
	Serial.println(_curmillis / 1000);
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
      /* this delay handles the patting timing for the hardware watchdog. Without this delay the timing won't work and you'll get endless resets. */

      delay(40);
      
      // if (!_client.connected()) {
      //	delay(50);
      // }
      
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
  _watchdog_millis = millis();
  pin_low = true;
  _client.begin(7777); // start UDP client
}

/* Connects to the watchdog server and sends an OKAY message (also adds 60 to uptime) if at least 60 seconds have passed since last send. Then drops the connection. */

void Watchdog::sendMsg(char *msg) 
{
  // pet HW watchdog, part 1.
  // to do this you must call pet() before you do anything, then pet2()
  // before leaving this function (sendMsg).

  pet();

  /* check time. if > 60 seconds since the last time we sent a message, send */

  //grab current time and compare to the last time we sent
  _curmillis = millis();
  if (_curmillis - _watchdog_millis > HW_SENDMSG_TIME) {

    _uptime += HW_SENDMSG_TIME;
    sprintf(_message, "%s %d.%d.%d.%d %lu ", msg, _arduino_ip[0], _arduino_ip[1],
	    _arduino_ip[2], _arduino_ip[3], _uptime/1000);

    _client.beginPacket(_server_ip, _server_port);
    _client.write(_message);
    _client.endPacket();
    
    if (_wd_debug == 1)
      Serial.println(_message);      
    //update time so the next send happens in 60 s
    _watchdog_millis = millis();
  }
  //pet HW watchdog, part 2
  pet2();
}
  

