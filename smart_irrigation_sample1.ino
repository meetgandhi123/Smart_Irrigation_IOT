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

const int relay1 = 5;

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
  pinMode(relay1, OUTPUT);
  //digitalWrite(relay1, LOW);                                           
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   
}

const int index_moisture_dry= 500;
const int index_moisture_humid=370;

void loop() { 
    timeClient.update();
   
    formattedDate =(String)timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.println(dayStamp);
    
    String hrs=(String)timeClient.getHours();
    String min1=(String)timeClient.getMinutes();
    String time1=(String)timeClient.getHours()+":"+(String)timeClient.getMinutes()+":"+(String)timeClient.getSeconds();
    Serial.println("time 1"+time1);
     
    int moisture_value=analogRead(moisture_pin);         // moisture value extract 
    Serial.println(moisture_value);
    String moisture = String(moisture_value);

    
    Firebase.pushString("/"+dayStamp+"/Moisture/"+hrs+"/"+min1, moisture);                           
    if(Firebase.failed())
    {
      Serial.println("Failed"); 
    }
    else
    {
      Serial.println("Success"); 
    }
    
    //relay sturcture
    
    if(moisture_value >= index_moisture_dry)
    {
      int temp=moisture_value;
      int n=5;
      time1=time1+"ON";
      Firebase.pushString("/"+dayStamp+"/logs", time1);
      if(Firebase.failed())
      {
        Serial.println("log not created!!!");
      }
      else{
        Serial.println("log created!!!");
      }
      while(temp>=index_moisture_dry)
      {
        digitalWrite(relay1, LOW);
        temp=analogRead(moisture_pin);
        if(n>=0)
        {
          n--;
          delay(1000);
        }
        else{
          break;
        }
        delay(4000);
      }
      digitalWrite(relay1, HIGH);
    }
    //relay end
   
    delay(30000);
}
