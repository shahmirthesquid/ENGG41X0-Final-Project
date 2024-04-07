#ifndef CONFIG_H
#define CONFIG_H

struct configData
{
	char projectName[32];
	uint16_t outletCount;
	bool blueiris_bool;
	char blueiris_username[32];
	char blueiris_password[32];
	char blueiris_url[32];
	char blueiris_camera[32];
	char language[3];
};

extern uint32_t configVersion;
extern const configData defaults;

#endif