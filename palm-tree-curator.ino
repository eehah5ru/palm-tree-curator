#include <WiFiServerSecure.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiType.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFiAP.h>
#include <WiFiClient.h>
#include <BearSSLHelpers.h>
#include <WiFiServer.h>
#include <ESP8266WiFiScan.h>
#include <WiFiServerSecureBearSSL.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiServerSecureAxTLS.h>

#include <InfluxDb.h>
//#include <InfluxData.h>

#define INFLUXDB_HOST "192.168.0.10"
#define WIFI_SSID "pheeKeezie5jeipoow"
#define WIFI_PASS "cryingwithcomputer"



ESP8266WiFiMulti WiFiMulti;

Influxdb influx(INFLUXDB_HOST);

//connection/ database stuff that needs configuring
//const char WIFI_NAME[] = "";
//const char WIFI_PASS[] = "";
//const char INFLUX_DATABASE[] = "";
//const char INFLUX_IP[] = "XXX.XXX.XXX.XXX";
//const char INFLUX_USER[] = ""; //username if authorization is enabled.
//const char INFLUX_PASS[] = ""; //password for if authorization is enabled.
//const char INFLUX_MEASUREMENT[] = ""; //measurement name for the database. (in practice, you can use several, this example just uses the one)

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  Serial.println(" ### Hello ###");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  
  Serial.print("Connecting to WIFI");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  influx.setDb("wemos_tst");

  Serial.println("Setup done");  

  InfluxData row("device_status");
  row.addTag("device", "wemos");
  row.addTag("event", "boot");
//  row.addValue("loopCount", loopCount);
  row.addValue("value", 1);

  influx.write(row);
  
}

//void loop() {
//  int raw = analogRead(A0);
//  Serial.println(raw);
//  delay(500);
//}

//int loopCount = 0;

void loop() {
//  loopCount++;

  int raw = analogRead(A0);
  InfluxData row("palm_moisture");
  row.addTag("device", "wemos");
  row.addTag("sensor", "one");
  row.addTag("mode", "analog");
//  row.addValue("loopCount", loopCount);
  row.addValue("value", raw);

  influx.write(row);

  delay(10000);
}  
