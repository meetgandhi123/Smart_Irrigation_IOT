#include <NTPClient.h>
#include <ESP8266WiFi.h>                 
#include <WiFiUdp.h>                                  
#include <FirebaseArduino.h>                                               
#include <ArduinoJson.h>
#include <SimpleDHT.h>                                                  //comment this if not having humidity and temperature sensor

#define FIREBASE_HOST "smartirrigationdata.firebaseio.com"              //host name from firebase
#define FIREBASE_AUTH "4ykpilHpXdiKucCIPWah6pTSaj8PGcwIMgu0z7Tz"        //auth key for the database   
#define WIFI_SSID "dhrumil"                                             //wifi name for node mcu to connect with 
#define WIFI_PASSWORD "9825381851"                                      //wifi password                                              

const int moisture_pin = A0; //analog pin for moisture reading


const int DHT11sensor = 4;                                               //comment this if not having humidity and temperature sensor
SimpleDHT11 dht11(DHT11sensor);                                          //comment this if not having humidity and temperature sensor



WiFiUDP ntpUDP;

const long utcOffsetInSeconds = 19800;   // off set of current timezone from utc

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; 

const int relay1 = 5;  // digital pin2 gpio 5 declaration for relay to be connect with the pump

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
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());
  pinMode(relay1, OUTPUT);                                                  // pin mode is declared for the relay.
  //digitalWrite(relay1, LOW);                                           
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                             //connection with firebase is begin here
}

  const int index_moisture_dry= 500;                                          // reading for the dry state
  const int index_moisture_notdry=370;                                        //reading for the moist state
 
  byte temperature = 0;                                                       //declaration for temperature        //comment this if not having humidity and temperature sensor
  byte humidity = 0;                                                          //declaration for humidity           //comment this if not having humidity and temperature sensor
  String dht_combine;
  
  String formattedDate;                                                     
  String dayStamp;





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

    //block of moisture starts here
     
    int moisture_value=analogRead(moisture_pin);         // moisture value extract 
    Serial.println(moisture_value);
    String moisture = String(moisture_value);

    //block of moisture ends here

     //block of temperature and humidity starts here

    int err = SimpleDHTErrSuccess;                                                        //comment this if not having humidity and temperature sensor
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)         //comment this if not having humidity and temperature sensor
    {                                                                                     
        Serial.print("Read DHT11 failed, err="); Serial.println(err);                     //comment this if not having humidity and temperature sensor
        delay(1000);                                                                      //comment this if not having humidity and temperature sensor
        return;                                                                           //comment this if not having humidity and temperature sensor
    }
    else{                                                                                 //comment this if not having humidity and temperature sensor  
          Serial.print((int)temperature); Serial.print(" *C, ");                          //comment this if not having humidity and temperature sensor
          Serial.print((int)humidity); Serial.println(" H");                              //comment this if not having humidity and temperature sensor
          dht_combine=(String)temperature +"*C   :   " + (String)humidity;
    }
    Firebase.pushString("/"+dayStamp+"dhtvalues/"+hrs+"/"+min1, dht_combine);                           //comment this if not having humidity and temperature sensor
    if(Firebase.failed())                                                                 //comment this if not having humidity and temperature sensor
    {
      Serial.println("Failed");                                                           //comment this if not having humidity and temperature sensor
    }
    else                                                                                  //comment this if not having humidity and temperature sensor
    {
      Serial.println("Success");                                                          //comment this if not having humidity and temperature sensor
    }                                                                                     //comment this if not having humidity and temperature sensor

    //block of temperature and humidity ends here

    //firebase upload of the soil moisture data starts here
    
    Firebase.pushString("/"+dayStamp+"/Moisture/"+hrs+"/"+min1, moisture);                           
    if(Firebase.failed())
    {
      Serial.println("Failed"); 
    }
    else
    {
      Serial.println("Success"); 
    }

    //firebase upload of the soil moisture data ends here

    
    //relay sturcture starts here
    
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
    //relay structure end here
    
    delay(30000);                     // overall 30 secs delay for next entry
}
