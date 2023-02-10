/*
 # Product: analog pH meter V1.0.
 # SKU    : SEN0161
 # Hardware: ESP8266
*/
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "SUTD_Wifi"
#define WIFI_PASSWORD "Sutd6062000"

// Project's API_KEY
#define API_KEY "AIzaSyCpDaE2L7-mikCmMKnLxKGEEsbbUkvLnSc"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "mynameisjoshua66@gmail.com"
#define USER_PASSWORD "testtest"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://sustaining-life-underwater-default-rtdb.asia-southeast1.firebasedatabase.app/"


#define SensorPin    A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection

// Instantiate Firebase objects. 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

// To store user's UID 
String uid;
String databasePath;
String phPath = "/ph";
String timePath = "/timestamp";
String parentPath;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
int timestamp;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup(void){
  pinMode(LED,OUTPUT);
  initWiFi();
  timeClient.begin();
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") 
  {
  Serial.print('.');
  delay(1000);
  }
  //save the user’s UID in the uid variable and print it in the Serial Monitor.
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.print(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

  Serial.begin(9600);
  Serial.println("pH meter experiment!");    //Test the serial monitor
}

void loop(void)
{
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;

  // check if it is time to send new readings
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
  sendDataPrevMillis = millis();
  }

  // Get current timestamp
  timestamp = getTime();
  Serial.print ("time: ");
  Serial.println (timestamp);

 // Update the parentPath variable to include the timestamp
  parentPath = databasePath + "/" + String(timestamp);
  
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();

      // Update parent path with timestamp
      json.set(timePath, String(timestamp));
      json.set(phPath.c_str(), String(pHValue));
   
      
      // Append data to parent path @ Firebase
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());

  }

  // Serial Monitor printer
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
        Serial.print(voltage,2);
        Serial.print("    pH value: ");
    Serial.println(pHValue,2);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
 
}

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}


double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}


