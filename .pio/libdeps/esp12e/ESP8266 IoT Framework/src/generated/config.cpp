#include <Arduino.h>
#include "config.h"

uint32_t configVersion = 579140413; //generated identifier to compare config with EEPROM

const configData defaults PROGMEM =
{
	"fish gang",
	5,
	false,
	"Username",
	"Password",
	"192.168.69.420:6969",
	"Camera Name",
	"en"
};