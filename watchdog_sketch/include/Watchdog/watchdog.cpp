#include "Arduino.h"
#include "watchdog.h"

Watchdog::Watchdog(IPAddress arduino_ip, byte arduino_mac[], IPAddress server_ip, int server_port, int pin) {
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

      if (Serial) {
	char tmp[100];
	sprintf(tmp, "Heartbeat sent.\nUptime: %lu\n", millis() / 1000);
	Serial.println(tmp);
      }
	last_pet = millis();
      if(!pin_low)
	{
	  // bring it low
	  pinMode(_pin, OUTPUT);
	  digitalWrite(_pin, LOW);
	  /*	  if (Serial) {
	    Serial.println("first millis: ");
	    Serial.println(_curmillis);
	    }*/
  	  pin_low = true; //pin is now low
	}
    }
}

void Watchdog::pet2()
{
  if (pin_low)
    {
      if (!_client.connected()) {
	delay(50);
      }
      
      /*      if (Serial) {
	Serial.println("second millis: ");
	Serial.println(_curmillis);
	}*/
      
      /*      if (Serial)
	      Serial.println("Pet2!\n");*/
      //if the pin is low we have NOT reset it to high, so do so.
      //otherwise nothing.
      // set to HIGH-Z
      pinMode(_pin, INPUT);
      pin_low = false; //pin is now high
    }
}

/* Tries to make the initial connection to the server */ 
/* Needs to be called in the SETUP loop of your function. */
void Watchdog::setup() 
{
  last_pet = millis();
  pin_low = true;
  _reconnect_millis = 0;
  
  if (_client.connect(_server_ip, _server_port)) {
    // if you get a connection, report back via serial:
    if (Serial)
      Serial.println("Successfully connected");
  } else {
    // if you didn't get a connection to the server:
    if (Serial)
      Serial.println("No Ethernet connection");
  }
}

/* Checks to see if the Ethernet client is connected. If so, sends a message over the wire if at least 120 seconds have passed since last send. If not, tries to reconnect. Also increases uptime by 120 seconds. */

void Watchdog::sendMsg(char *msg) 
{
  //Serial.println("sendmsg\n");
  // pet HW watchdog
  pet();

  if (_client.connected()) 
  {
    _curmillis = millis();

    if (_curmillis - _watchdog_millis > 60000) {
      _uptime += 60000;
      sprintf(_message, "%s %d.%d.%d.%d %lu ", msg, _arduino_ip[0], _arduino_ip[1],
	      _arduino_ip[2], _arduino_ip[3], _uptime/1000);
      
      _client.write(_message);
      if (Serial)
	Serial.println(_message);
      _watchdog_millis = millis();
    }
  } else {
    _curmillis = millis();
    if (_curmillis - _watchdog_millis > 60000) {
      _uptime += 60000;
      _watchdog_millis = millis();
      //      Serial.println("dogdelay\n");
      //delay(100);
    }
  }

  // if the server's disconnected, loop to reconnect
  if (!_client.connected()) {
    if (millis() - _reconnect_millis > HW_RECONNECT_TIME) {
      _reconnect_millis = millis();
      _client.stop();
      if (Serial)
	Serial.println("Reconnecting, v2...");
      if (_client.connect(_server_ip, _server_port)) {
	if (Serial)
	  Serial.println("Connected");
      }
      else {
	// if you didn't get a connection to the server:
	if (Serial)
	  Serial.println("No Ethernet connection");
      }
    }
  }
  //pet HW watchdog, part 2
  pet2();

  
  /*  if (Serial) {
  if (pin_low == true)
    Serial.println("pin low\n");
  else
    Serial.println("pin high:\n");
    }*/
}
  

