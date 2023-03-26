/*
 # Product: analog pH meter V1.0.
 # SKU    : SEN0161
 # Hardware: ESP8266
*/
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
// #include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// // Provide the token generation process info.
// #include "addons/TokenHelper.h"
// // Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ephraim"
#define WIFI_PASSWORD "ephraim$$$"

#define SensorPin    A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection

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


#define API_KEY "AIzaSyDYe5fJgfC9J1NKhkdV-3gMlfM4qIlnDkM"

/* 3. Define the RTDB URL */
#define DATABASE_URL "gsc-team-1-water-default-rtdb.asia-southeast1.firebasedatabase.app/" 
//<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "tes@test.com"
#define USER_PASSWORD "pass1234"
// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//initalize function
double avergearray(int* , int );
void initWiFi();
unsigned long getTime();
void FirebaseSetup();

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("WIFI IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void setup(void){
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
 // Initialize Wifi
  initWiFi();
// initializee time
  timeClient.begin();
//Firebase
// Firebase.begin(host,key);
FirebaseSetup();

//  // Assign API Key
  // config.api_key = API_KEY;
 
//  // Assign the user sign in credentials
  // auth.user.email = USER_EMAIL;
  // auth.user.password = USER_PASSWORD;
 
  // Assign the RTDB URL
  // config.database_url = DATABASE_URL;
  // fbdo.setResponseSize(4096);
 
  // Assign the callback function for the long running token generation task
  // config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  // config.max_token_generation_retry = 5;


  // Initialize the library with the Firebase authen and config  
  // Firebase.begin(&config,&auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  // while ((auth.token.uid) == "") 
  // {
  // Serial.print('.');
  // delay(1000);
  // }
  // //save the user’s UID in the uid variable and print it in the Serial Monitor.
  // uid = auth.token.uid.c_str();
  // Serial.print("User UID: ");
  // Serial.print(uid);

  // Update database path

  // databasePath = "/UsersData/" + uid + "/readings";


  Serial.begin(9600);
  Serial.println("pH meter experiment!");    //Test the serial monitor
}

void loop(void)
{
  long samplingTime=0;
  long sendTime=0;
  // static unsigned long samplingTime = millis();
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
      // json.set(timePath, String(timestamp));
      // json.set(phPath.c_str(), String(pHValue));
      
      // Append data to parent path @ Firebase
      // Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());

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
  if(millis() - sendTime > 5000){
    Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/test/float"), millis()) ? "ok" : fbdo.errorReason().c_str());

    Serial.println("data sent");
    sendTime=millis();
 }
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

void FirebaseSetup(){
  /* Assign the api key (required) */
  config.api_key = "AIzaSyDYe5fJgfC9J1NKhkdV-3gMlfM4qIlnDkM";

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
}
