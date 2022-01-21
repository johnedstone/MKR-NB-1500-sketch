### Notes
* SIM: Hologram
* Device: MKR NB 1500
* Firmware:
```
  AT+GMM
  SARA-R410M-02B
  AT+CGMR
  L0.0.00.00.05.06 [Feb 03 2018 13:00:41]
```

### Sketches
* NBWebClient_hourly_SSL_Post
     * Purpose: Do a POST with JSON data to a REST API (in this case, [Django Rest Framework](https://www.django-rest-framework.org/))
     * Development based on these sketches: 
         * libraries/MKRNB/examples/NBSSLWebClient
         * [GSMWebClient_hourly_SSL_Post](https://github.com/johnedstone/mkrgsm1400-post-json-ssl)
     * Library modifications:
         * First: remove libraries/MKRNB and\
         replace with libraries/MKRNB-master from https://github.com/arduino-libraries/MKRNB
         * Second: update libraries/MKRNB-master/src/Modem.cpp\
         as noted at https://forum.arduino.cc/t/mkr-1500-nb-hangs-on-nbaccess-begin/636736 and\
         proposed by hakondahle update `int ModemClass::begin(bool restart)` and `void ModemClass::end()`
         * Third (optionally): while waiting on modem firmware updating and/or getting Root certs loaded disable cert validation:
             * See this file: libraries/MKRNB-master/src/NBClient.cpp
             ```
             125 //MODEM.send("AT+USECPRF=0,0,1");
             126  MODEM.sendf("AT+USECPRF=0");
             ```
     * Currently:
         * Running successfully with 60 min iterations.  See comments in sketch for previous tests

<!---
# vim: ai et ts=4 sw=4 sts=4 nu
->
