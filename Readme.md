### To Do
* Move [NBWebClient_hourly_SSL_Post sketch](https://github.com/johnedstone/mkrgsm1400-post-json-ssl/tree/main/NBWebClient_hourly_SSL_Post) into this repository
* SIM: Hologram
* Device: MKR NB 1500
* Firmware:

### Notes
* SIM: Hologram
* Device: MKR NB 1500
* Firmware:

### Sketches
* NBWebClient_hourly_SSL_Post
     * Development based on these sketches: 
         * libraries/MKRNB/examples/NBSSLWebClient
         * [GSMWebClient_hourly_SSL_Post](https://github.com/johnedstone/mkrgsm1400-post-json-ssl)
     * Library modifications:
         * First: remove libraries/MKRNB and\
         replace with libraries/MKRNB-master from https://github.com/arduino-libraries/MKRNB
         * Second: update libraries/MKRNB-master/src/Modem.cpp\
         as noted at https://forum.arduino.cc/t/mkr-1500-nb-hangs-on-nbaccess-begin/636736\
         proposed by hakondahle update `int ModemClass::begin(bool restart)` and `void ModemClass::end()`
         * Third (optionally): while waiting on modem firmware updating and/or getting Root certs loaded disable cert validation:
             * See this file: libraries/MKRNB-master/src/NBClient.cpp
             ```
             125 //MODEM.send("AT+USECPRF=0,0,1");
             126  MODEM.sendf("AT+USECPRF=0");
             ```

<!---
# vim: ai et ts=4 sw=4 sts=4 nu
->
