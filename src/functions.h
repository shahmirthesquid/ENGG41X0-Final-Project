#include <Arduino.h>
#include "LittleFS.h"

#include "WiFiManager.h"
#include "webServer.h"
#include "updater.h"
#include "configManager.h"
#include "dashboard.h"
#include "timeSync.h"

#include <Wire.h>
#include <SPI.h>
#include "Adafruit_ADS1X15.h"
#include <SD.h>
#include "ArduinoJson.h"

#include <time.h>
#include "Timezone.h"



#define RELAYCOUNT 16
#define RELAYLIST bool* relayList[16] = {&dash.data.relay_01,&dash.data.relay_02,&dash.data.relay_03,&dash.data.relay_04,&dash.data.relay_05,&dash.data.relay_06,&dash.data.relay_07,&dash.data.relay_08,&dash.data.relay_09,&dash.data.relay_10,&dash.data.relay_11,&dash.data.relay_12,&dash.data.relay_13,&dash.data.relay_14,&dash.data.relay_15,&dash.data.relay_16};
#define RELAYSTATELIST bool* relayStateList[16] = {&dash.data.relayState_01,&dash.data.relayState_02,&dash.data.relayState_03,&dash.data.relayState_04,&dash.data.relayState_05,&dash.data.relayState_06,&dash.data.relayState_07,&dash.data.relayState_08,&dash.data.relayState_09,&dash.data.relayState_10,&dash.data.relayState_11,&dash.data.relayState_12,&dash.data.relayState_13,&dash.data.relayState_14,&dash.data.relayState_15,&dash.data.relayState_16};
#define RELAYNAMELIST char(*relayNameList[16])[64] = {&dash.data.relay_name_01, &dash.data.relay_name_02, &dash.data.relay_name_03, &dash.data.relay_name_04, &dash.data.relay_name_05, &dash.data.relay_name_06, &dash.data.relay_name_07, &dash.data.relay_name_08, &dash.data.relay_name_09, &dash.data.relay_name_10, &dash.data.relay_name_11, &dash.data.relay_name_12, &dash.data.relay_name_13, &dash.data.relay_name_14, &dash.data.relay_name_15, &dash.data.relay_name_16};
#define RELAYMODELIST char (*relayModeList[16])[32] = {&dash.data.relayMode_01,&dash.data.relayMode_02,&dash.data.relayMode_03,&dash.data.relayMode_04,&dash.data.relayMode_05,&dash.data.relayMode_06,&dash.data.relayMode_07,&dash.data.relayMode_08,&dash.data.relayMode_09,&dash.data.relayMode_10,&dash.data.relayMode_11,&dash.data.relayMode_12,&dash.data.relayMode_13,&dash.data.relayMode_14,&dash.data.relayMode_15,&dash.data.relayMode_16};


#define DAILYSTARTLIST uint8_t (*dailyStartList[16])[2] = {&dash.data.timer_start_daily_01, &dash.data.timer_start_daily_02, &dash.data.timer_start_daily_03 , &dash.data.timer_start_daily_04 , &dash.data.timer_start_daily_05 , &dash.data.timer_start_daily_06 , &dash.data.timer_start_daily_07 , &dash.data.timer_start_daily_08 , &dash.data.timer_start_daily_09, &dash.data.timer_start_daily_10, &dash.data.timer_start_daily_11, &dash.data.timer_start_daily_12, &dash.data.timer_start_daily_13, &dash.data.timer_start_daily_14, &dash.data.timer_start_daily_15, &dash.data.timer_start_daily_16  };
#define DAILYLENGTHLIST float *dailyLengthList[16] = {&dash.data.timer_length_daily_01, &dash.data.timer_length_daily_02, &dash.data.timer_length_daily_03 , &dash.data.timer_length_daily_04 , &dash.data.timer_length_daily_05 , &dash.data.timer_length_daily_06 , &dash.data.timer_length_daily_07 , &dash.data.timer_length_daily_08 , &dash.data.timer_length_daily_09, &dash.data.timer_length_daily_10, &dash.data.timer_length_daily_11, &dash.data.timer_length_daily_12, &dash.data.timer_length_daily_13, &dash.data.timer_length_daily_14, &dash.data.timer_length_daily_15, &dash.data.timer_length_daily_16  };

#define WEEKDAYLIST char(*weekDayList[16])[32] = {&dash.data.timer_day_weekl_01, &dash.data.timer_day_weekl_02, &dash.data.timer_day_weekl_03, &dash.data.timer_day_weekl_04, &dash.data.timer_day_weekl_05, &dash.data.timer_day_weekl_06, &dash.data.timer_day_weekl_07, &dash.data.timer_day_weekl_08, &dash.data.timer_day_weekl_09, &dash.data.timer_day_weekl_10, &dash.data.timer_day_weekl_11, &dash.data.timer_day_weekl_12, &dash.data.timer_day_weekl_13, &dash.data.timer_day_weekl_14, &dash.data.timer_day_weekl_15, &dash.data.timer_day_weekl_16};
#define WEEKSTARTLIST uint8_t (*weekStartList[16])[2] = {&dash.data.timer_start_weekl_01, &dash.data.timer_start_weekl_02, &dash.data.timer_start_weekl_03, &dash.data.timer_start_weekl_04, &dash.data.timer_start_weekl_05, &dash.data.timer_start_weekl_06, &dash.data.timer_start_weekl_07, &dash.data.timer_start_weekl_08, &dash.data.timer_start_weekl_09, &dash.data.timer_start_weekl_10, &dash.data.timer_start_weekl_11, &dash.data.timer_start_weekl_12, &dash.data.timer_start_weekl_13, &dash.data.timer_start_weekl_14, &dash.data.timer_start_weekl_15, &dash.data.timer_start_weekl_16};
#define WEEKLENGTHLIST float *weekLengthList[16] = {&dash.data.timer_length_weekl_01, &dash.data.timer_length_weekl_02, &dash.data.timer_length_weekl_03, &dash.data.timer_length_weekl_04, &dash.data.timer_length_weekl_05, &dash.data.timer_length_weekl_06, &dash.data.timer_length_weekl_07, &dash.data.timer_length_weekl_08, &dash.data.timer_length_weekl_09, &dash.data.timer_length_weekl_10, &dash.data.timer_length_weekl_11, &dash.data.timer_length_weekl_12, &dash.data.timer_length_weekl_13, &dash.data.timer_length_weekl_14, &dash.data.timer_length_weekl_15, &dash.data.timer_length_weekl_16};


#define INTERVALINTRVLIST float *intervalIntrvList[16] = {&dash.data.timer_interval_intrv_01, &dash.data.timer_interval_intrv_02, &dash.data.timer_interval_intrv_03, &dash.data.timer_interval_intrv_04, &dash.data.timer_interval_intrv_05, &dash.data.timer_interval_intrv_06, &dash.data.timer_interval_intrv_07, &dash.data.timer_interval_intrv_08, &dash.data.timer_interval_intrv_09, &dash.data.timer_interval_intrv_10, &dash.data.timer_interval_intrv_11, &dash.data.timer_interval_intrv_12, &dash.data.timer_interval_intrv_13, &dash.data.timer_interval_intrv_14, &dash.data.timer_interval_intrv_15, &dash.data.timer_interval_intrv_16};
#define INTERVALLENGTHLIST float *intervalLengthList[16] = {&dash.data.timer_length_intrv_01, &dash.data.timer_length_intrv_02, &dash.data.timer_length_intrv_03, &dash.data.timer_length_intrv_04, &dash.data.timer_length_intrv_05, &dash.data.timer_length_intrv_06, &dash.data.timer_length_intrv_07, &dash.data.timer_length_intrv_08, &dash.data.timer_length_intrv_09, &dash.data.timer_length_intrv_10, &dash.data.timer_length_intrv_11, &dash.data.timer_length_intrv_12, &dash.data.timer_length_intrv_13, &dash.data.timer_length_intrv_14, &dash.data.timer_length_intrv_15, &dash.data.timer_length_intrv_16};

//bool relayList[5] = {dash.data.relay1,dash.data.relay2,dash.data.relay3,dash.data.relay4,dash.data.relay5};
//bool relayStateList[5] = {dash.data.relay1State,dash.data.relay2State,dash.data.relay3State,dash.data.relay4State,dash.data.relay5State};


//writes serial transmission of relay state, does not write to file
void writeRelays();
void updateRelays(unsigned long* sinceLastInterval);
//checks if certain amount of time has elapsed
bool hasBeen(int seconds,unsigned long sinceLast);

bool writeSD(const char*, const char*);
String readSD(const char*);
bool writeSDJSON(const char* path, const JsonDocument& doc);
void loadSettingsJSON();
void saveSettingsJSON();


//Serial Communication Functions-----------------
//Processes sensor data from slave
void processCall(String command);
//Recieves sensor data from slave
void receiveEvent(int howMany);

