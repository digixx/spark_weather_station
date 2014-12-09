/*  Weather base
    (c) 2014 makerparts.ch
*/
#include <math.h>
#include "bmp085.h"
#include "dht22.h"
#include "tcs34725.h"
#include "ina219.h"
#include "debug.h"
#include "eeprom.h"
#include "sensors.h"
#include "config.h"

//*********************************************************
const char prog_mode = 36; // "$"

//*********************************************************

char compiledDateTime[22] = COMPILED_DATE;
unsigned long lastSync = 0;
unsigned long startpoint = 0;
unsigned long waitTime = 0;
char szData[16];
int stat =  0;

String HTTPreq;
bool doNotSleep = false;
bool inProgMode = false;


//*********************************************************
TCPClient client;
byte server[] = { 192,168,55,150 }; // Raspberry Pi
byte sparkfun[] = { 54,86,132,254 };

//*********************************************************

void sendDataToSparkfun(void){
    RGB.color(128,255,0);
    if(client.connect(sparkfun, 80)){
        HTTPreq = "GET /input/AJJVVRE0VYfjpx6Z0o1Y?private_key=rzz22gRj2KTbwDym0EzN";

        sprintf(szData, "%.2f", dhtd.t); HTTPreq = HTTPreq + "&temp=" + szData;
        sprintf(szData, "%.2f", dhtd.h); HTTPreq = HTTPreq + "&humidity=" + szData;

        sprintf(szData, "%d", tcsd.colortemp); HTTPreq = HTTPreq + "&colortemp=" + szData;
        sprintf(szData, "%d", tcsd.lux); HTTPreq = HTTPreq + "&brightness=" + szData;
        sprintf(szData, "%d", tcsd.r); HTTPreq = HTTPreq + "&red=" + szData;
        sprintf(szData, "%d", tcsd.g); HTTPreq = HTTPreq + "&green=" + szData;
        sprintf(szData, "%d", tcsd.b); HTTPreq = HTTPreq + "&blue=" + szData;
        sprintf(szData, "%d", tcsd.c); HTTPreq = HTTPreq + "&clear=" + szData;

        sprintf(szData, "%d", bmpd.pressure / 100); HTTPreq = HTTPreq + "&pressure=" + szData;

        sprintf(szData, "%.2f", ina219d.voltage); HTTPreq = HTTPreq + "&voltage=" + szData;
        sprintf(szData, "%.2f", ina219d.current); HTTPreq = HTTPreq + "&current=" + szData;

        sprintf(szData, "%d", rssi); HTTPreq = HTTPreq + "&rssi=" + szData;

        HTTPreq = HTTPreq + "\n";
        HTTPreq = HTTPreq + "Host: " + SPARKFUN_DATA_URL + "\n";
        HTTPreq = HTTPreq + "User-Agent: spark.io\nContent-Length: 0\nConnection: close\n";

        client.println(HTTPreq);
        delay(100);
        if (client.available())
        {
            char c = client.read();
        }
        client.flush();
        client.stop();
        RGB.color(0,128,0);
    }else{
        RGB.color(128,0,0);
    }
}

void updateDATA() {
    debugPrintln("\nContact server..");
    inProgMode = false;
    if (client.connect(server, 80))
    {
        HTTPreq = "GET /data.php";
        sprintf(szData, "%d", stat); HTTPreq = HTTPreq + "?stat=" + szData;

        sprintf(szData, "%.2f", ina219d.voltage); HTTPreq = HTTPreq + "&volt=" + szData;
        sprintf(szData, "%.2f", ina219d.current); HTTPreq = HTTPreq + "&chrg=" + szData;

        sprintf(szData, "%.2f", dhtd.t); HTTPreq = HTTPreq + "&temp=" + szData;
        sprintf(szData, "%.2f", dhtd.h); HTTPreq = HTTPreq + "&hum=" + szData;

        sprintf(szData, "%.2f", bmpd.pressure); HTTPreq = HTTPreq + "&baro=" + szData;

        sprintf(szData, "%d", tcsd.colortemp); HTTPreq = HTTPreq + "&ctemp=" + szData;
        sprintf(szData, "%d", tcsd.lux); HTTPreq = HTTPreq + "&cbright=" + szData;
        sprintf(szData, "%d", tcsd.r); HTTPreq = HTTPreq + "&cred=" + szData;
        sprintf(szData, "%d", tcsd.g); HTTPreq = HTTPreq + "&cgreen=" + szData;
        sprintf(szData, "%d", tcsd.b); HTTPreq = HTTPreq + "&cblue=" + szData;
        sprintf(szData, "%d", tcsd.c); HTTPreq = HTTPreq + "&cclear=" + szData;

        sprintf(szData, "%d", rssi); HTTPreq = HTTPreq + "&rssi=" + szData;

        HTTPreq = HTTPreq + "&version=" + compiledDateTime;
        HTTPreq = HTTPreq + "\n";
        HTTPreq = HTTPreq + "Host: 192.168.55.150\n";
        HTTPreq = HTTPreq + "User-Agent: spark.io\nContent-Length: 0\nConnection: close\n";

        client.println(HTTPreq);
        delay(250);

        while (client.available())
        {
            char c = client.read();
            if (c == prog_mode) {
                inProgMode = true;
            }
        }
        if (inProgMode) {
            debugPrintln("ProgMode=true");
        } else {
            debugPrintln("ProgMode=false");
        }
        client.flush();
        client.stop();
        debugPrintln("\nsending data successfully");
    }else{
        debugPrintln("\nsending data failed");
    }
}

void debugPrintData(void){
    if (debugging) {
        debugPrintln("--------------------");
        sprintf(szData, "%.2f", dhtd.t); debugPrint("temp="); debugPrintln(szData);
        sprintf(szData, "%.2f", dhtd.h); debugPrint("humidity="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.colortemp); debugPrint("colortemp="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.lux); debugPrint("brightness="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.r); debugPrint("red="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.g); debugPrint("green="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.b); debugPrint("blue="); debugPrintln(szData);
        sprintf(szData, "%d", tcsd.c); debugPrint("clear="); debugPrintln(szData);
        sprintf(szData, "%d", bmpd.pressure / 100); debugPrint("pressure="); debugPrintln(szData);
        sprintf(szData, "%.2f", ina219d.voltage); debugPrint("voltage="); debugPrintln(szData);
        sprintf(szData, "%.2f", ina219d.current); debugPrint("current="); debugPrintln(szData);
        sprintf(szData, "%d", rssi); debugPrint("rssi="); debugPrintln(szData);
        debugPrintln("--------------------");
    }
}

//*********************************************************
void setup() {
    pinMode(HIDE_RGB, INPUT_PULLDOWN);
    debugPrintln("********************\n");
    initDebugPrint();

    strcat(compiledDateTime, " - "); strcat(compiledDateTime, COMPILED_TIME);

	  initBARO();
	  initCOLOR();
    initPOWER();
    lastSync = millis();
    debugPrintln("delay 1..2..3..4");
    while ((millis() - lastSync) < START_DELAY * 1000) SPARK_WLAN_Loop();
}

void loop() {
    // Disable RGBled to avoid wrong data from color sensor
    if (digitalRead(HIDE_RGB) == HIGH) {
        RGB.control(true);
        RGB.color(0,0,0);
    }
    debugPrintln("++++++++++++++++++++");
    debugPrint("code: "); debugPrintln(compiledDateTime);

    readTEMP_HUM();
    readBARO();
    readCOLOR();
    readPOWER();
    readRSSI();

    updateDATA();

    RGB.control(false);

    switch (statusPOWER()) {
        case PowerHigh:
            doNotSleep = true;
            if (inProgMode) {
                waitTime = PROGMODE_DELAY;
            } else {
                waitTime = IDLE_DELAY;
            }
            break;

        case PowerLow:
            doNotSleep = false;
            waitTime = SURVIVE_DELAY;
            break;

        default:
            if (inProgMode) {
                doNotSleep = true;
                waitTime = PROGMODE_DELAY;
            } else {
                doNotSleep = false;
                waitTime = SLEEP_DELAY;
            }
    }

    // Send daylight value into cloud for Torch lamp
    sprintf(szData, "%d", tcsd.lux);
    Spark.publish("weather_lux", szData);

    debugPrintData();
    //debugPrintDataTransmitted();
    lastSync = millis();
    if (doNotSleep) {
        debugPrintln("no sleep, idle ..\n\r####################\n\n");
        stat = 1;
        while ((millis() - lastSync) < (waitTime * 1000)) SPARK_WLAN_Loop();
    } else {
        debugPrintln("sleeping..\n\r####################\n\n");
        closeDebugPrint();
        stat = 0;
        Spark.sleep(SLEEP_MODE_DEEP, waitTime);
    }
}
// END
