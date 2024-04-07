#include "functions.h"


struct task
{    
    unsigned long rate;
    unsigned long previous;
};

task taskA = { .rate = 500, .previous = 0 };
int LED1 =2; //assign LED1 to gpio2 pin
int LED2 = 16; //assign LED2 to gpio16 pin

unsigned long sinceLastSerialMsg;
unsigned long sinceLastTimePrint;
unsigned long sinceLastInterval[16];

TimeChangeRule EDT = { "EDT", Second, Sun, Mar, 2, -240 };    //Daylight time = UTC - 4 hours
TimeChangeRule EST = { "EST", First, Sun, Nov, 2, -300 };     //Standard time = UTC - 5 hours
Timezone Eastern(EDT, EST);

int powerSum = 0;

//Adafruit_ADS1115 ads;



void setup() 
{

  pinMode(LED1, OUTPUT);
  pinMode(LED2,OUTPUT);

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,HIGH);

  Serial.begin(115200);

  #ifdef ESP32
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  #else
    if (!LittleFS.begin()) {
      Serial.println("An Error has occurred while mounting LittleFS");
      return;
    }
  #endif
  GUI.begin();
  configManager.begin();
  WiFiManager.begin(configManager.data.projectName);
  timeSync.begin();
  dash.begin(500);


  //Set the timezone to Amsterdam
  timeSync.begin();

  //Wait for connection
  timeSync.waitForSyncResult(10000);

  if (timeSync.isSynced())
  {
      time_t now = time(nullptr);
      Serial.print(PSTR("Current time in UTC: "));
      Serial.print(asctime(localtime(&now)));
      strcpy(dash.data.timeDisplay, asctime(localtime(&now)));
  }
  else 
  {
      Serial.println("Timeout while receiving the time");
  }



  //readSetRelays();
  loadSettingsJSON();

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,HIGH);

  // Initialize Serial to Arduino MEGA using Wire.h-------------
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  Wire.begin(D2, D1);

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,LOW);

  sinceLastSerialMsg = millis();

  RELAYLIST
  RELAYMODELIST
  for(int i = 0; i < RELAYCOUNT;i++){
    sinceLastInterval[i] = 0;
    if(strcmp(*relayModeList[i],"None") != 0){
      *relayList[i] = false;
    }
  }

  //if (!ads.begin()) {
  //  Serial.println("Failed to initialize ADS.");
  //  while (1);
  //}

  // Start the first conversion.
  //ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1, /*continuous=*/false);
  //strcpy(dash.data.relayMode1,"None");
}

void loop() 
{
  RELAYLIST
  RELAYSTATELIST

  //turn off both LEDs
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);


  //software interrupts
  WiFiManager.loop();
  updater.loop();
  configManager.loop();
  dash.loop();

  //your code here
  //task A
  if (taskA.previous == 0 || (millis() - taskA.previous > taskA.rate))
  {
    if (timeSync.isSynced())
    {
      time_t now = time(nullptr);
      Serial.printf("Bytes Free: %d  ", ESP.getFreeHeap());
      Serial.print(PSTR("Current time in UTC: "));
      Serial.print(asctime(localtime(&now)));
      time_t est_time = Eastern.toLocal(now);
      strcpy(dash.data.timeDisplay, asctime(localtime(&est_time) ) );

      //Serial.printf("Colour  R:%d G:%d B:%d\n",dash.data.timer_start_test[0], dash.data.timer_start_test[1],dash.data.timer_start_test[2]);
      //Serial.printf("Time  H:%d M:%d \n",dash.data.timer_start_daily_01[0], dash.data.timer_start_daily_01[1]);
    }
    taskA.previous = millis();

    String stringOne = "eel gang";
    stringOne.toCharArray(dash.data.projectName,32);
    
    dash.data.outletCount = configManager.data.outletCount;

    dash.data.dummyInt++;
    dash.data.inputInt++;


    if (dash.data.inputBool)
        dash.data.dummyBool = true;
    else
        dash.data.dummyBool = false;

    powerSum = 0;
    bool parameterChangedFlag = false;
    for(int i = 0 ; i<RELAYCOUNT ;i++){
      if(*relayList[i]!= *relayStateList[i]){
        parameterChangedFlag=true;
      }
      if(*relayList[i]){
        powerSum = powerSum+20;
        *relayStateList[i] = *relayList[i];

      }
      else{
        *relayStateList[i] = false;
      }
    }
    if(parameterChangedFlag){
      //saveSettingsJSON();
    }
    if (powerSum == 0){
      dash.data.wattsGraph = 0;
    }
    else{
    dash.data.wattsGraph = powerSum + (rand() % 5 + 1);

    }
    dash.data.watts = dash.data.wattsGraph;
    /*
    if (ads.conversionComplete()) {
      int16_t results = ads.getLastConversionResults();

      Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(ads.computeVolts(results)); Serial.println("V)");

      dash.data.watts = ads.computeVolts(results);
      dash.data.wattsGraph = ads.computeVolts(results);

      // Start another conversion.
      ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1, false); //not continuous
      
    }
    */
    
    updateRelays(sinceLastInterval);

  }

  if(hasBeen(10000,sinceLastTimePrint)){
    
    //Serial.printf("%s  --  Bytes Free: %d  --  Session: %s\n",formattedTime, ESP.getFreeHeap() , session);
    sinceLastTimePrint = millis();
    //dash.data.relay_01 = !dash.data.relay_01;
    saveSettingsJSON();

    //login2("shahmir","khan");
  }
  if(hasBeen(1000,sinceLastSerialMsg)){
    digitalWrite(LED1,LOW);
    //digitalWrite(LED2,LOW);
    sinceLastSerialMsg = millis();
    Wire.requestFrom(8, 10);
    receiveEvent(10);

    writeRelays();
    //Serial.println();
    /*
    for (int i = 0; i < 16; i++) {
      Serial.print("Relay(Inverted)");
      Serial.print((i + 1));
      Serial.print("\nStatus: ");
      Serial.print(relay[i]);
      Serial.println();
      Serial.println("name" + String(i + 1) + " IS " + names[i] + "   start" + String(i + 1) + ": " + startTimes[i] + "   Timer" + String(i + 1) + ": " + timer[i] + "    len" + String(i + 1) + ": " + String(timeLengths[i]));
    }
    */
    //Serial.println(ESP.getFreeHeap(),DEC);
  }
}
