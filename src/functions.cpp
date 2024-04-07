#include "functions.h"

void updateRelays(unsigned long* sinceLastInterval){
  RELAYLIST
  RELAYMODELIST

  DAILYSTARTLIST
  DAILYLENGTHLIST

  WEEKDAYLIST
  WEEKSTARTLIST
  WEEKLENGTHLIST

  INTERVALINTRVLIST
  INTERVALLENGTHLIST

  TimeChangeRule EDT = { "EDT", Second, Sun, Mar, 2, -240 };    //Daylight time = UTC - 4 hours
  TimeChangeRule EST = { "EST", First, Sun, Nov, 2, -300 };     //Standard time = UTC - 5 hours
  Timezone Eastern(EDT, EST);

  const char *daysOfWeek[8] = {"Null", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


  for (int i = 0; i < RELAYCOUNT; i++) {
    long long int totalSec = 0;
    if(strcmp(*relayModeList[i],"Daily") == 0){
      int debug = 0;

      
      
      int extractedHour = (int) (*dailyStartList[i])[0];
      int extractedMin = (int) (*dailyStartList[i])[1];
      int extractedSec = (extractedHour * 60 * 60) + (extractedMin * 60);

      float waterSeconds = *dailyLengthList[i] * 60.0;
      time_t now = time(nullptr);
      time_t est_time = Eastern.toLocal(now);
      int curHour = hour(est_time);
      int curMinute = minute(est_time);
      int curSecond = second(est_time);
      totalSec = (curHour * 60 * 60) + (curMinute * 60) + curSecond;

      if(debug){
        Serial.printf("totalSec = (%d*60*60)+(%d*60)+%d  = " ,curHour,curMinute,curSecond);
        Serial.println(totalSec);
        Serial.printf("seconds to start at = (%d*60*60) + (%d*60) = %d\n",extractedHour,extractedMin,extractedSec);
        Serial.printf("seconds to end at: %f\n", extractedSec + waterSeconds);

      }

      if (totalSec >= extractedSec && totalSec <= (extractedSec + waterSeconds)) {
        if(debug){
          Serial.println("SETTING HIGH");
          Serial.println("Watering time!");
        }
        if ((totalSec + 2) > (extractedSec + waterSeconds)) {
          *relayList[i] = false;

          //char tmpCstring[80];
          //sprintf(tmpCstring,"/relay%d.txt",i+1);
          //writeFile(SPIFFS, tmp.c_str(), "1"); no need to wear out the memory with this

        } else {
          *relayList[i] = true;
        }
      }
    }

    else if(strcmp(*relayModeList[i],"Weekly") == 0){
      time_t now = time(nullptr);
      time_t est_time = Eastern.toLocal(now);
      if(strcmp(*weekDayList[i],daysOfWeek[weekday(est_time)]) == 0){
        int debug = 0;

        int extractedHour = (int) (*weekStartList[i])[0];
        int extractedMin = (int) (*weekStartList[i])[1];
        int extractedSec = (extractedHour * 60 * 60) + (extractedMin * 60);

        float waterSeconds = *weekLengthList[i] * 60.0;
        time_t now = time(nullptr);
        time_t est_time = Eastern.toLocal(now);
        int curHour = hour(est_time);
        int curMinute = minute(est_time);
        int curSecond = second(est_time);
        totalSec = (curHour * 60 * 60) + (curMinute * 60) + curSecond;

        if(debug){
          Serial.printf("totalSec = (%d*60*60)+(%d*60)+%d  = " ,curHour,curMinute,curSecond);
          Serial.println(totalSec);
          Serial.printf("seconds to start at = (%d*60*60) + (%d*60) = %d\n",extractedHour,extractedMin,extractedSec);
          Serial.printf("seconds to end at: %f\n", extractedSec + waterSeconds);

        }

        if (totalSec >= extractedSec && totalSec <= (extractedSec + waterSeconds)) {
          if(debug){
            Serial.println("SETTING HIGH");
            Serial.println("Watering time!");
          }
          if ((totalSec + 2) > (extractedSec + waterSeconds)) {
            *relayList[i] = false;

            //char tmpCstring[80];
            //sprintf(tmpCstring,"/relay%d.txt",i+1);
            //writeFile(SPIFFS, tmp.c_str(), "1"); no need to wear out the memory with this

          } else {
            *relayList[i] = true;
          }
        }
      }

    }
  

    else if(strcmp(*relayModeList[i],"Time Interval") == 0){
      int timeOut = (60 * (*intervalIntrvList[i]) ) * 1000;
      int enableTime = (60 * (*intervalLengthList[i]) ) * 1000;
      if(hasBeen(timeOut,sinceLastInterval[i])){
        *relayList[i] = true;
      }
      if(hasBeen(timeOut + enableTime, sinceLastInterval[i])){
        *relayList[i] = false;
        sinceLastInterval[i] = millis();
      }
    }
  }
}

void writeRelays(){
    RELAYLIST
    for(int i = 0;i<RELAYCOUNT;i++){
    //delay(10);
    if(*relayList[i]==true){
        char tmp[32];
        sprintf(tmp,"{\"gpio\":%d,\"state\":0}",i+22);
        //String tmp;
        //tmp+="{\"gpio\":";
        //tmp+=i+22;
        //tmp+=",\"state\":1}";
        Wire.beginTransmission(8); 
        Wire.write(tmp);  
        Wire.endTransmission();     
    }
    else{
        //String tmp;
        //tmp+="{\"gpio\":";
        //tmp+=i+22;
        //tmp+=",\"state\":0}";

        char tmp[32];
        sprintf(tmp,"{\"gpio\":%d,\"state\":1}",i+22);

        Wire.beginTransmission(8); 
        Wire.write(tmp);  
        Wire.endTransmission();   
    }

    }

}
bool hasBeen(int seconds,unsigned long sinceLast){
    if( (unsigned long)   (millis() - sinceLast) > (unsigned long) seconds){
    return true;
    }
    return false;
}



bool initSD(){
    int pinCS = D8;
    if(SD.begin(pinCS)){
    Serial.println("SD init successful!");
    return true;
    }
    Serial.println("SD init failed!");
    return false;
}

String readSD(const char* filename) {

    if( !initSD() ){
    return "";
    }

    File file = SD.open(filename);
    String data = "";
    if (file) {
    while (file.available()) {
        data += (char)file.read();
    }
    file.close();
    } else {
    Serial.print("Error reading file ");
    Serial.println(filename);
    }
    SD.end();
    return data;
}

bool writeSDJSON(const char* path, const JsonDocument& doc){
    if( !initSD() ){
    return false;
    }

    File file = SD.open(path, FILE_WRITE | O_CREAT | O_TRUNC);
    file.truncate(0);

    if(file){
    serializeJsonPretty(doc,file);
    file.close();
    SD.end();
    return true;
    }
    Serial.print("Error writing to file ");
    Serial.println(path);
    SD.end();
    return false;
}

void loadSettingsJSON(){

  int debug = 1; //debug flag

  bool issue = false; //SD card issue flag
  bool sdPluggedIn = initSD(); 

  RELAYLIST
  for(int i = 0; i < RELAYCOUNT ;i++){
      *relayList[i] = false;
  }
  RELAYNAMELIST
  for(int i = 0; i < RELAYCOUNT ;i++){
    strcpy(*relayNameList[i],"");
  }
  RELAYMODELIST
  for(int i = 0; i < RELAYCOUNT ;i++){
      strcpy(*relayModeList[i], "None");
  }
  DAILYSTARTLIST
  DAILYLENGTHLIST
  WEEKDAYLIST
  WEEKSTARTLIST
  WEEKLENGTHLIST
  INTERVALINTRVLIST
  INTERVALLENGTHLIST
  for(int i = 0; i < RELAYCOUNT ;i++){
    *dailyStartList[i][0]=0;
    *dailyStartList[i][1]=0;
    *dailyLengthList[i]=0.0;

    strcpy(*weekDayList[i],"");
    *weekStartList[i][0]=0;
    *weekStartList[i][1]=0;
    *weekLengthList[i] = 0.0;

    *intervalIntrvList[i]=0.0;
    *intervalLengthList[i]=0.0;
  }

  if(sdPluggedIn){
    DynamicJsonDocument saveFile(6144);
    String extractedText  = readSD("config/config.txt");
    Serial.println(extractedText);
    DeserializationError error = deserializeJson(saveFile,extractedText );
    extractedText = "";
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());


      issue = true;
      if(debug){Serial.println("Factory settings loaded"); }

    } 
    else{

      char tmp[32];
      char tmp2[32];
      for(int i = 0 ;i<RELAYCOUNT;i++){
        //RELAY ON/OFF
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"relay_0%d",i+1);
        }
        else{
          sprintf(tmp,"relay_%d",i+1);
        }
        *relayList[i] = saveFile[tmp];
        
        //RELAY NAMES
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"relay_name_0%d",i+1);
        }
        else{
          sprintf(tmp,"relay_name_%d",i+1);
        }
        strcpy(*relayNameList[i],saveFile[tmp]);
        
        //RELAY MODE
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"relayMode_0%d",i+1);
        }
        else{
          sprintf(tmp,"relayMode_%d",i+1);
        }
        strcpy(*relayModeList[i],saveFile[tmp]);

        //DAILY START TIME
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_start_daily_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_start_daily_%d",i+1);
        }
        strcpy(tmp2,saveFile[tmp]);
        char* token = strtok(tmp2,":");
        (*dailyStartList[i])[0] = atoi(token);
        token = strtok(NULL,":");
        (*dailyStartList[i])[1] = atoi(token);

        //DAILY LENGTH TIME
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_length_daily_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_length_daily_%d",i+1);
        }
        *dailyLengthList[i] = saveFile[tmp];


        //Week day
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_day_weekl_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_day_weekl_%d",i+1);
        }
        strcpy(*weekDayList[i], saveFile[tmp]);

        //Weekly start time
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_start_weekl_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_start_weekl_%d",i+1);
        }
        strcpy(tmp2,"");
        strcpy(tmp2,saveFile[tmp]);
        char* token2 = strtok(tmp2,":");
        (*weekStartList[i])[0] = atoi(token2);
        token = strtok(NULL,":");
        (*weekStartList[i])[1] = atoi(token2);
        

        //Weekly length time
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_length_weekl_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_length_weekl_%d",i+1);
        }
        *weekLengthList[i] = saveFile[tmp];

        //interval timeout
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_interval_intrv_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_interval_intrv_%d",i+1);
        }
        *intervalIntrvList[i] = saveFile[tmp];

        //interval length
        strcpy(tmp,"");
        if(i<9){
          sprintf(tmp,"timer_length_intrv_0%d",i+1);
        }
        else{
          sprintf(tmp,"timer_length_intrv_%d",i+1);
        }
        *intervalLengthList[i] = saveFile[tmp];
      }

      
      if(debug){Serial.println("config loaded"); }
    }

  }

  //return issue;
  if(debug){
  Serial.print("SD card detected? ");
  Serial.println(sdPluggedIn);

  Serial.print("loading Issue detected? ");
  Serial.println(issue);

  }
  /*
  if(!sdPluggedIn){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" No SD card! :(");
  delay(500);
  }
  else if(issue){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("No files to load");
  delay(500);
  }
  else{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Settings loaded!");
  delay(500);
  }
  menu.switch_focus(true);
  */

}

void saveSettingsJSON(){

  RELAYLIST
  RELAYNAMELIST
  RELAYMODELIST
  DAILYSTARTLIST
  DAILYLENGTHLIST
  WEEKDAYLIST
  WEEKSTARTLIST
  WEEKLENGTHLIST
  INTERVALINTRVLIST
  INTERVALLENGTHLIST

  bool debug = true;

  int pinCS = D8;

  bool sdPluggedIn = initSD();

  if(!sdPluggedIn){
    return;
  }

  SD.begin(pinCS);

  char settingsFolder[64];
  strcpy(settingsFolder, "config");
  if( !SD.exists(settingsFolder) ){
    if( SD.mkdir(settingsFolder) ){
      Serial.println("Settings Folder Created!");
    }
    else{
      Serial.println("Issue Creating Settings Folder!  :( ");
      return;
    }
  }
  SD.end();

  DynamicJsonDocument saveFile(6144);

  char tmp[32];
  for(int i = 0 ;i<RELAYCOUNT;i++){

    //RELAY ON/OFF
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"relay_0%d",i+1);
    }
    else{
      sprintf(tmp,"relay_%d",i+1);
    }
    saveFile[tmp] = *relayList[i];

    //RELAY NAMES
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"relay_name_0%d",i+1);
    }
    else{
      sprintf(tmp,"relay_name_%d",i+1);
    }
    saveFile[tmp] = *relayNameList[i];
    //RELAY MODE
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"relayMode_0%d",i+1);
    }
    else{
      sprintf(tmp,"relayMode_%d",i+1);
    }
    saveFile[tmp] = *relayModeList[i];

    //DAILY START TIME
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_start_daily_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_start_daily_%d",i+1);
    }
    char tmp2[32];
    sprintf(tmp2,"%d:%d",(*dailyStartList[i])[0],(*dailyStartList[i])[1]);
    //if(debug){ Serial.printf("Daily Start %d: %s\n",i+1,tmp2);}
    saveFile[tmp] = tmp2;

    //Daily length time
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_length_daily_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_length_daily_%d",i+1);
    }
    saveFile[tmp] = *dailyLengthList[i];

    //Week day
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_day_weekl_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_day_weekl_%d",i+1);
    }
    saveFile[tmp] = *weekDayList[i];

    //Weekly start time
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_start_weekl_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_start_weekl_%d",i+1);
    }
    strcpy(tmp2,"");
    sprintf(tmp2,"%d:%d",(*weekStartList[i])[0],(*weekStartList[i])[1]);
    //if(debug){ Serial.printf("Daily Start %d: %s\n",i+1,tmp2);}
    saveFile[tmp] = tmp2;

    //Weekly length time
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_length_weekl_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_length_weekl_%d",i+1);
    }
    saveFile[tmp] = *weekLengthList[i];

    //interval timeout
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_interval_intrv_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_interval_intrv_%d",i+1);
    }
    saveFile[tmp] = *intervalIntrvList[i];

    //interval length
    strcpy(tmp,"");
    if(i<9){
      sprintf(tmp,"timer_length_intrv_0%d",i+1);
    }
    else{
      sprintf(tmp,"timer_length_intrv_%d",i+1);
    }
    saveFile[tmp] = *intervalLengthList[i];
  }

  if(debug){
    serializeJsonPretty(saveFile,Serial);
  }
  writeSDJSON("config/config.txt",saveFile);

}

void processCall(String command){



  int debug = 0;
  if(debug && command !=""){Serial.print("PARSING SENSORS:  ");Serial.println(command);}
  
  
  //Serial.println("TRYING TO PROCESS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  if (command.charAt(2)=='1'){
    if(debug){
    Serial.println("Runoff set to ON !");

    }
    //runoff = true;
  }
  else if(command.charAt(2)=='0'){
    if(debug){
    Serial.println("Runoff set to OFF !");

    }
    //runoff = false;
  }
  if(command.charAt(1)=='1'){
    if(debug){
    Serial.println("Drain set to ON !");

    }
    //sinceLastDrainTime = millis();
  }
  // no off condition for drain pump cuz it should stay on for a full 30 secs after being triggered.
  
  //set GPIO state  
  ////////////////////////////////////////////////////////////////////digitalWrite(gpio, state);
  //digitalWrite(PE0, state);
  
}

// function that executes when data is received from master
void receiveEvent(int howMany) {

  int debug = 0;

  if(debug){
    Serial.print("\nEvent Recieved!\nhowMany = ");
    Serial.println(howMany);
  }
  //howMany++;
  String data="";
 while (0 <Wire.available()) {
    char c = Wire.read();     
    data += c;
    
  }
  //Serial.print("RECEIVED:  ");
  //Serial.println(data);           
  processCall(data);         
}

