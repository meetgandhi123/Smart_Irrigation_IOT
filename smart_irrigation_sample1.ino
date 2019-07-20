#include <NTPClient.h>
#include <ESP8266WiFi.h>                 
#include <WiFiUdp.h>                                  
#include <FirebaseArduino.h>                                               
#include <ArduinoJson.h>                                                            

#define FIREBASE_HOST "iothings-6ca95.firebaseio.com"                          
#define FIREBASE_AUTH "icB3ynjEPGTYF6YxrOA1SkQVLSlYEAH9n5gE7glo"           
#define WIFI_SSID "dhrumil"                                              
#define WIFI_PASSWORD "9825381851"                                    
                                                
const int moisture_pin = A0;
const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String formattedDate;
String dayStamp;//----------------
void setup() {
  Serial.begin(9600);
  delay(2000);                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  timeClient.begin();
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                           
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   
}

void loop() { 
    timeClient.update();
//    Serial.print(daysOfTheWeek[timeClient.getDay()]);
//    Serial.print(", ");
//    Serial.print(timeClient.getHours());
//    Serial.print(":");
//    Serial.print(timeClient.getMinutes());
//    Serial.print(":");
//    Serial.println(timeClient.getSeconds()); 
//    formattedDate = timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.println(dayStamp);
    String hrs=(String)timeClient.getHours();
    String min1=(String)timeClient.getMinutes();
    String time1=(String)timeClient.getHours()+":"+(String)timeClient.getMinutes()+":"+(String)timeClient.getSeconds();
    Serial.println(time1);
     int moisture_value=analogRead(moisture_pin);
     Serial.println(moisture_value);
     String moisture = String(moisture_value);
     Firebase.pushString("/Moisture/"+hrs+"/"+min1, moisture);                             
     
     if(Firebase.failed())
     {
     Serial.println("Failed"); 
     }
     else
     {
      Serial.println("Success"); 
     }
     delay(15000);
  
}
