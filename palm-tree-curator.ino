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

#include "BME280.h"

#include <InfluxDb.h>
//#include <InfluxData.h>

/* A BME280 object with I2C address 0x76 (SDO to GND) */
BME280 bme(Wire, 0x76);

#define INFLUXDB_HOST "192.168.0.10"
#define WIFI_SSID "pheeKeezie5jeipoow"
#define WIFI_PASS "cryingwithcomputer"

//#define DUMMY_INFLUX // do not send data to influxdb server

#define SERIAL_DEBUG // send values to serial port if defined

#define INFLUX_DEVICE "wemos"

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

//const int sclPin = D1;
//const int sdaPin = D2;

//
// write row to InfluxDb server
//
void influxWriteRow (InfluxData &row) {
#ifndef DUMMY_INFLUX
  influx.write(row);  
#endif
}

void setup() {
  pinMode(A0, INPUT);
//  Wire.begin(sdaPin, sclPin);
  
  Serial.begin(9600);
  
  while(!Serial){
    // waiting for serial
  }
  
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

  // begin communication with
  // BME280 and set to default
  // sampling, iirc, and standby settings
  int bmeStatus = bme.begin();
  if (bmeStatus < 0) {
    Serial.print("Error communicating with sensor, check wiring and I2C address. Status: ");
    Serial.print(bmeStatus);
    while(1){}
  }

  Serial.println("BME280 connected");
  
  influx.setDb("wemos_tst");

  Serial.println("Setup done");  

  InfluxData row("device_status");
  row.addTag("device", INFLUX_DEVICE);
  row.addTag("event", "boot");
//  row.addValue("loopCount", loopCount);
  row.addValue("value", 1);

  influxWriteRow(row);
  
}


void serialDebug (char *label, int value) {
#ifdef SERIAL_DEBUG  
  Serial.print(label);
  Serial.print(": ");
  Serial.println(value);
#endif
}

void serialDebug (char *label, float value, int precision) {
#ifdef SERIAL_DEBUG    
  Serial.print(label);
  Serial.print(": ");
  Serial.println(value, precision);
#endif
}

void sendSoilMoisture () {
  int rawMoisture = analogRead(A0);
  
  InfluxData row("palm_moisture");
  row.addTag("device", INFLUX_DEVICE);
  row.addTag("sensor", "one");
  row.addTag("mode", "analog");
  row.addValue("value", rawMoisture);

  influxWriteRow(row);
  serialDebug("soil moisture", rawMoisture);
}

void sendAirTemperature () {
  float temp = bme.getTemperature_C();
  
  InfluxData row("palm_moisture");
  row.addTag("device", INFLUX_DEVICE);
  row.addTag("sensor", "air_temp");
  row.addTag("mode", "digital");
  row.addValue("value", temp);

  influxWriteRow(row);  

  serialDebug("air temperature", temp, 2);
}

void sendAirPressure () {
  float pressure = bme.getPressure_Pa();
  
  InfluxData row("palm_moisture");
  row.addTag("device", INFLUX_DEVICE);
  row.addTag("sensor", "air_pressure");
  row.addTag("mode", "digital");
  row.addValue("value", pressure);

  influxWriteRow(row);  

  serialDebug("air pressure", pressure, 2);
}

void sendAirHumidity () {
  float humidity = bme.getHumidity_RH();
  InfluxData row("palm_moisture");
  row.addTag("device", INFLUX_DEVICE);
  row.addTag("sensor", "air_humidity");
  row.addTag("mode", "digital");
  row.addValue("value", humidity);

  influxWriteRow(row);  

  serialDebug("air humidity", humidity, 2);
}


void loop() {
 sendSoilMoisture();
 
 bme.readSensor();

 sendAirTemperature();
 sendAirPressure();
 sendAirHumidity();
  
 delay(10000);
}  
