//-----------------------------------------------
//Author:
//Paulo Vinícius

//-----------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include<stdlib.h>

//-------------------------------------------------------------------------------
const char* ssid = "---------------";
const char* password = "-----------------";
WiFiClient client; //define 'client' as object
#define DHTPIN 13     // what pin the sensor is connected to
#define DHTTYPE DHT21   // DHT 22  (AM2302)
float temp, umid, hic; //
boolean lastConnected = false;  
DHT dht(DHTPIN, DHTTYPE, 30);
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  dht.begin(); // set up the DHT22.
  Serial.println();
  Serial.println(ssid);
  WiFi.begin(ssid, password); //inserido
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  delay(1000);
}
//------------------------------------------------------------------------------
void loop(){
   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   float umid = dht.readHumidity();
   float temp = dht.readTemperature();
   float hic = dht.computeHeatIndex(temp, umid, false);
   // converter float to string
   char ct[10];
   dtostrf(temp, 7, 2, ct);
   String st = ct;
   st.trim();
   int i = st.length()+1;
   st.toCharArray(ct, i);

   //-------------------------------------------------
   // Converte umidade
   char cu[10];
   dtostrf(umid, 7, 2, cu);
   String su = cu;
   su.trim();
   i = su.length()+1;
   su.toCharArray(cu, i);


   
   Serial.print(ct);
   Serial.print(" ");
   Serial.println(cu);
     
    sendToPushingBox(ct);
    
      
   // if there's no net connection, but there was one last time
   // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  lastConnected = client.connected();
  
  delay(117000); // interval
}

//Function for sending the request to PushingBox
void sendToPushingBox(char T[]){ //T[]
  client.stop();
  
  Serial.println("connecting...");
  delay(100);
  if (client.connect("api.pushingbox.com", 80)) {
    Serial.println("connected");
    client.print("GET /pushingbox?devid=-----------------&tempData=");
    client.print(T);
    client.println(" HTTP/1.1");
    client.println("Host: api.pushingbox.com");
    //client.println("Accept: */*"); //new
    client.println("Connection: Close"); //keep-alive close
    client.println("User-Agent: ESP8266");
    client.println();
    
    // Read all the lines of the reply from server and print them to Serial
      unsigned long timeout = millis();
      while (client.available() == 0) {
      if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      Serial.println(T);
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    }
    Serial.println(T);
    Serial.println("closing connection");
    Serial.println(client.read());
  } 
  else {
    Serial.println("connection failed");
  }
}
