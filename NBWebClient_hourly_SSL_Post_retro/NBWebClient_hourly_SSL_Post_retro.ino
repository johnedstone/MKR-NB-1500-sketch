/*
 * Sketch: NBWebClient_hourly_SSL_Post
 * Device: MKR NB 1500
 * SIM: hologram
 * Firmware:
 *   AT+GMM
 *   SARA-R410M-02B
 *   AT+CGMR
 *   L0.0.00.00.05.06 [Feb 03 2018 13:00:41]
 *
 * Purpose: POST JSON over SSL to a REST API server
 *
 * Development based on these sketches: 
 *  - libraries/MKRNB/examples/NBSSLWebClient
 *  - GSMWebClient_hourly_SSL_Post (https://github.com/johnedstone/mkrgsm1400-post-json-ssl)
 *
 * Several library modification needed
 *   First: remove libraries/MKRNB and
 *     replace with libraries/MKRNB-master from https://github.com/arduino-libraries/MKRNB
 *   Second: update libraries/MKRNB-master/src/Modem.cpp and
 *     as noted at https://forum.arduino.cc/t/mkr-1500-nb-hangs-on-nbaccess-begin/636736
 *     proposed by hakondahle,
 *     update int ModemClass::begin(bool restart) and
 *                   void ModemClass::end()
 *   Third (optionally): while waiting on modem firmware updating and/or getting Root certs loaded,
 *     disable cert validation:
 *     See this file: libraries/MKRNB-master/src/NBClient.cpp
 *       125 //MODEM.send("AT+USECPRF=0,0,1");
 *       126  MODEM.sendf("AT+USECPRF=0");
 *
 * Purpose of this sketch: going back to an earlier design
 * Test #1: mised hour 20:58, with "unexpected response, ie. not 201" but continued on
 * Test #2: turned on reading all of the characters, except the last one (just in case we would hang there)
 *    in order to see this "unexpected respose"
 *
 * Added back modem.begin()
 * Up now for 4d+
 * Considering stable
 */

#include <Arduino.h>
#include <UpTime.h>     // https://github.com/jozef/Arduino-UpTime
#include <MKRNB.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h"

const char PINNUMBER[]     = SECRET_PINNUMBER;
int port = 443;

// Note: 3600000 is 1 hour.  Currently sleeping 1 hour-27 sec
int sleeping_ms = 3561000;
//int sleeping_ms = 180000; // 3 min
//int sleeping_ms = 1800000; // 30 min

char server[] = "your.rest.api";
char path[] = "/your/endpoint/to/post/";

String IMEI = "";
String ICCID = "";

void startModem() {
  GPRS gprs;
  NB nbAccess;
  //NB nbAccess(true);
  NBModem modem;

  Serial.println(F("Starting NB/GPRS/NBSSLClient."));
  boolean connected = false;

  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
      Serial.println(F("Connected to network"));

      //Get IP.
      IPAddress LocalIP = gprs.getIPAddress();
      Serial.print(F("Local IP address = "));
      Serial.println(LocalIP);

      IMEI = modem.getIMEI();
      Serial.print(F("Modem IMEI: "));
      Serial.println(IMEI);

      ICCID = modem.getICCID();
      Serial.print(F("ICCID: "));
      Serial.println(ICCID);

    } else {
      Serial.println(F("Not connected to network: restarting modem"));
      modem.begin();
      delay(1000);
    }
  }
}

void postData() {
  NBSSLClient client;

  // https://arduinojson.org/v6/example/http-server/ (for sending json)
  // Allocate a temporary JsonDocument
  // One could use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> doc;
  doc["imei_string"] = IMEI;
  doc["uptime"] = getUptime();

  if (client.connect(server, port)) {
    Serial.println(F("Connected to REST API"));
    client.print(F("POST "));
    client.print(path);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);

    // https://forum.arduino.cc/t/how-to-make-a-post-request-with-mkrgsm-gsmsslclient-library/501250/13
    // https://forum.arduino.cc/t/tomcat-arduino-gsm-shield-post-json/387640
    client.println(F("Content-Type: application/json;charset=UTF-8"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(measureJsonPretty(doc));

    // Terminate headers
    client.println();

    // Send body
    serializeJsonPretty(doc, client);

    // pause waiting for response
    // delay(750);
    delay(2000);

    // https://arduinojson.org/v6/example/http-client/
    // Check HTTP status
    // char status[32] = {0}; // Good for "HTTP/1.1 200 OK"

    /*
     * Debugging
     */
    bool force_client_stop = false;
    bool read_on = true;
    while(read_on) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }

      // SSL hack: if there is just one byte left, force a stop
      // For SSL client.connected() will not go to false - bug?
      // "A client is considered connected if the connection
      // has been closed but there is still unread data"
      if (client.available() == 1 && client.connected()) {
        force_client_stop = true;
      }
      if (force_client_stop && client.available() == 0) {
        client.stop();
      }
    
      // if the server's disconnected, stop the client:
      if (!client.available() && !client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop(); // is this needed, as it's above as well ??
        read_on = false;
      }
    }
    //End Debugging

    /* START Refer to debugging above
    char status[96] = {0};
    client.readBytesUntil('\r', status, sizeof(status));

    client.stop();

    if (strcmp(status, "HTTP/1.1 201 Created") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.print(F("Status: "));
      Serial.println(status);
    } else {
      Serial.print(F("Status: "));
      Serial.println(status);
    }
    // END Refer to debugging above
  */
  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("REST API connection failed"));
    client.stop();
  }

  serializeJsonPretty(doc, Serial);
  Serial.println(F(""));

  Serial.println(F("Blinking  5 sec ... web request finished"));
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(5000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}

String getUptime() {
  return "uptime: "+uptime_as_string()+" or "+uptime()+"s";
}

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  delay(2000);

  Serial.println(F("Sketch: NBWebClient_hourly_SSL_Post_retro"));
}

void loop() {
  startModem();
  postData();

  Serial.println(getUptime());
  Serial.print(F("Sleeping for "));
  Serial.print(sleeping_ms);
  Serial.println(F(" ms Note: 3600000 is 1 hour"));
  delay(sleeping_ms);

  //while (true);
}

/*
# vim: ai et ts=2 sw=2 sts=2 nu
*/
