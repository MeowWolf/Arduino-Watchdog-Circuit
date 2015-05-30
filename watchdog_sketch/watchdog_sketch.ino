int pulsePin = 4; //digital pin 4
unsigned long lastHeartbeat = 0;
unsigned long lastUptimeReport = 0;
unsigned long heartbeat_millis = millis();

void heartbeat() {
/* MEOW WOLF: Our heartbeat will use a timer to switch pin 8 from LOW to HIGH after about 300 ms (this gives the capacitor time to drain). We won't want to use a delay, but otherwise the heartbeat will be similar... -Cathy */ 

  // Sink current to drain charge from watchdog circuit
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  delay(300);
  // Return to high-Z
  pinMode(pulsePin, INPUT);
  lastHeartbeat = millis();
  Serial.println("Heartbeat sent");
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);  
  Serial.println("Arduino reset");
  
  // Send an initial heartbeat.
  heartbeat();
}

// the loop routine runs over and over again forever:
void loop() {
  /*MEOW WOLF: Because there is no serial output connected, the default version of the loop tests the failure (reboot) case. The Arduino will reboot after about 270 seconds. Uncomment the following code to test the success case -- a live heartbeat, sent every five seconds. As long as the heartbeat continues, the Arduino won't reboot, and you should see the uptime continue past 300+ seconds in the Serial Monitor. -Cathy */
  /* With the below heartbeat block commented out, Arduino should reset after approx 250 seconds */
  /* If below heartbeat block is present, Arduino should not reset. Test to at least 500 seconds to verify this */
  
  //UNCOMMENT THIS BLOCK TO TEST HEARTBEAT. Or pull wire from digital pin
  if (millis() - heartbeat_millis > 5000) {
     heartbeat();
     heartbeat_millis = millis();
  }


  // Check for serial inputs.  If found, send heartbeat.
  if (Serial.available()) {
    // Clear input buffer
    while (Serial.available()) {
      Serial.read();
    }
    heartbeat();
  }
  unsigned long uptime = millis();
  if ((uptime - lastUptimeReport) >= 5000) {
    // It has been at least 5 seconds since our last uptime report.  
    Serial.println("Uptime: " + String((uptime - (uptime % 5000)) / 1000) + " seconds (" + String((uptime - lastHeartbeat) / 1000) + " seconds since last heartbeat)");
    // Pretend we did it exactly on the 5 second mark so we don't start slipping.
    lastUptimeReport = (uptime - (uptime % 5000));
  }
  // delay in between loops
  delay(100);
}
