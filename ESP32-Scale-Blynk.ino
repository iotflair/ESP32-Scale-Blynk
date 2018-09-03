#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"
#define DOUT  26
#define CLK  27

void getWeight(void);
bool triggerBlynkConnect = false;
bool isFirstConnect = true;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "AUTH";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SSID";
char pass[] = "PASS";
float weight = 0;

BlynkTimer timer;

HX711 scale(DOUT, CLK);
float calibration_factor = - 19800;	//You must change this factor depends on your scale,sensors and etc.

void myTimerEvent()
{
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(F("WiFi not connected"));
    
    Blynk.connectWiFi(ssid, pass);
  }
  else
  {
    Serial.println(F("WiFi in connected"));
  }

  if (!Blynk.connected() && WiFi.status() == WL_CONNECTED)
  {
    if (Blynk.connect())
    {
      Serial.println(F("Blynk reconnected"));
    }
    else
    {
      Serial.println(F("Blynk not reconnected"));
    }
  }
  else
  {
    Serial.println(F("Blynk in connected"));
  }

  if (Blynk.connected() && WiFi.status() == WL_CONNECTED)
  {

  }
}

BLYNK_READ(V16)
{
  getWeight();
  Blynk.virtualWrite(V16, weight);
}

BLYNK_CONNECTED()
{
  triggerBlynkConnect = true;

  Serial.println(F("Blynk Connected!"));
  Serial.println(F("local ip"));
  Serial.println(WiFi.localIP());

  if (isFirstConnect)
  {       
    Blynk.syncAll();    

    isFirstConnect = false;
  }
}


void setup() {
  Serial.begin(9600);

  Blynk.config(auth);
    Blynk.connectWiFi(ssid, pass);

  if (WiFi.status() == WL_CONNECTED)
  {
    if (Blynk.connect())
    {
      triggerBlynkConnect = true;
    }
    else
    {
      triggerBlynkConnect = false;
    }
  }
  
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  delay(2000);

  timer.setInterval(5000L, myTimerEvent);
}
void loop() {

  if (Blynk.connected())
  {
    Blynk.run(); // Initiates Blynk Server  
  }
  else
  {
    triggerBlynkConnect = false;
  }
  
  timer.run(); // Initiates BlynkTimer 

}

void getWeight(){
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  weight = scale.get_units();
  Serial.print(weight, 1);
  Serial.print(" kg"); // You can change this to other type of weighing value and re-adjust the calibration factor.
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
}

