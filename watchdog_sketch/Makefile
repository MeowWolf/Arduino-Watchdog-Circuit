# do different things depending on MAC or LINUX
# this depends on the package Arduino-mk, 
# avaliable through brew or apt-get

MONITOR_BAUDRATE	= 115200
BOARD_TAG 				= uno 
ARDUINO_LIBS 			= Watchdog Ethernet SPI 
USER_LIB_PATH			= include

#VARIANT = mega
#F_CPU = 16000000


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ifneq ("$(wildcard $(/dev/arduino1))","")
		#it exists
		ARDUINO_PORT 	= /dev/arduino1
		MONITOR_PORT	= /dev/arduino1
	else
		ARDUINO_PORT 	= /dev/ttyACM*
		MONITOR_PORT	= /dev/ttyACM*
	endif

	ARDUINO_DIR = /usr/share/arduino
	BAUD  = 115200
	include /usr/share/arduino/Arduino.mk
endif

ifeq ($(UNAME_S),Darwin)
	ARDUINO_DIR				= /Applications/Arduino.app/Contents/Java
	MONITOR_PORT 			= ls /dev/cu.usb*
	AVRDUDE 					= /usr/local/bin/avrdude
	AVRDUDE_CONF			= /Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf
	include  /usr/local/Cellar/arduino-mk/1.5/Arduino.mk
endif


