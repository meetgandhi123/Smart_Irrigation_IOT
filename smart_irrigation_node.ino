#include <NTPClient.h>
#include <ESP8266WiFi.h>                 
#include <WiFiUdp.h>                                  
#include <FirebaseArduino.h>                                               
#include <ArduinoJson.h>

#define firebase_host "temp-nodewise.firebaseio.com"
#define firebase_auth "WrXvNdIzt9lyGonFsevXW94KpijCfHJZ3pN6Tpws"
#define WIFI_SSID "dhrumil"                                             //wifi name for node mcu to connect with 
#define WIFI_PASSWORD "9825381851"

WiFiUDP ntpUDP;
const int moisture_pin =A0;

const long utcOffsetInSeconds = 19800;   // off set of current timezone from utc
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);   // for extraction of current date and time from pool.ntp.org with the off set from timezone

void setup() {

  Serial.begin(9600);
  delay(2000);                
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //node mcu connects with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  timeClient.begin();                                                       // timeclient begin for extracting hrs,minutes,seconds
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                     
  Firebase.begin(firebase_host, firebase_auth);                             //connection with firebase is begin here
}
void reset_fun()
{
  Serial.println("Resetting the controller.....");
  ESP.restart();
}

 
String formattedDate;                                                     
String dayStamp;
int mcu_reset;

void loop() {

 timeClient.update();
   
    formattedDate =(String)timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.println(dayStamp);
    
    String hrs=(String)timeClient.getHours();
    String min1=(String)timeClient.getMinutes();
    String time1=(String)timeClient.getHours()+":"+(String)timeClient.getMinutes()+":"+(String)timeClient.getSeconds();
    Serial.println(time1);
    mcu_reset=Firebase.getInt("RESET_NODES/RESET_NODE1");
    Serial.println(mcu_reset);
    if(mcu_reset==1)
    {
      Firebase.setInt("RESET_NODES/RESET_NODE1",0);
      reset_fun();
    }
    //block of moisture starts here
     
    int moisture_value=analogRead(moisture_pin);         // moisture value extract 
    Serial.println(moisture_value);

    //block of moisture ends here

     //firebase upload of the soil moisture data starts here
    Firebase.setInt("/node1/"+dayStamp+"/Moisture/"+time1, moisture_value);
    Firebase.setInt("/node1/temp",moisture_value);
    if(Firebase.failed())
    {
      Serial.println("Failed!!!");
    }
    else{
      Serial.println("success");
      }
   /* Firebase.pushString("/"+dayStamp+"/node1"+"/Moisture/"+hrs+"/"+min1, moisture);                           
    if(Firebase.failed())
    {
      Serial.println("Failed"); 
    }
    else
    {
      Serial.println("Success"); 
    }*/
    delay(30000);
    //firebase upload of the soil moisture data ends here

}
