//*********************************************************
struct {
  float pressure;
  float temperature;
} bmpd;

Adafruit_BMP085 bmp;

void initBARO (void) {
    if (!bmp.begin()) {
	    while(1) SPARK_WLAN_Loop(); // Die, with the possibility of reincarnation
	}
	debugPrintln("BARO init");
}

void readBARO(void) {
  bmpd.temperature = bmp.readTemperature();
  bmpd.pressure = bmp.readPressure() / 96.2; //96.2% Druck in 300m H.ü.M
  debugPrintln("BARO readed");
}

//*********************************************************
struct {
  uint16_t  r;
  uint16_t  g;
  uint16_t  b;
  uint16_t  c;
  uint16_t  colortemp;
  uint16_t  lux;
} tcsd;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_16X);

void initCOLOR (void) {
    if (!tcs.begin()) {
        while(1) SPARK_WLAN_Loop(); // Die, with the possibility of reincarnation
    }
    debugPrintln("COLOR init");
}

void readCOLOR(void) {
  // Read Color Sensor
  tcs.getRawData(&tcsd.r, &tcsd.g, &tcsd.b, &tcsd.c);
  tcsd.colortemp = tcs.calculateColorTemperature(tcsd.r, tcsd.g, tcsd.b);
  tcsd.lux = tcs.calculateLux(tcsd.r, tcsd.g, tcsd.b);
  debugPrintln("COLOR readed");
}

void normalizeCOLOR(void) {
// Normalise color measurement
    float resolution = 10000 / (tcsd.r + tcsd.g + tcsd.b); // = 100%
    tcsd.r = tcsd.r * resolution;
    tcsd.g = tcsd.g * resolution;
    tcsd.b = tcsd.b * resolution;
}

//*********************************************************
struct {
  float h;
  float t;
} dhtd;

int idDHT22pin = A7; //Analog pin for comunications (no power on glitch)
void dht22_wrapper(); // must be declared before the lib initialization
// DHT instantiate
idDHT22 DHT22(idDHT22pin, dht22_wrapper);

void dht22_wrapper() {
	DHT22.isrCallback();
}

void readTEMP_HUM(void) {
// Read Humidity/Temps Sensor
    if(DHT22.acquireAndWait()==IDDHTLIB_OK){
        dhtd.h = DHT22.getHumidity();
        dhtd.t = DHT22.getCelsius();
    }
    debugPrintln("TEMP_HUM readed");
}

//*********************************************************
struct {
  float voltage;
  float current;
} ina219d  ;

const float maxVoltageLimit = 6.0; // Volt
const float minVoltageLimit = 4.3; // Volt
const int PowerOk = 0;
const int PowerHigh = 1;
const int PowerLow = 2;

Adafruit_INA219 ina219;

void initPOWER(void) {
    ina219.begin();
    debugPrintln("POWER init");
}

void readPOWER(void) {
    int i = 0;
    ina219d.current = 0;
    ina219d.voltage = 0;
    /* Check if voltage is in valuable range */
    while ((ina219d.voltage < 2) || (ina219d.voltage > 10)) {
        ina219d.voltage = ina219.getBusVoltage_V();
        if (i<3) {
            i++;
        } else {
            break;
        }
    }
    ina219d.current = ina219.getCurrent_mA();
}

int statusPOWER(void) {
    if (ina219d.voltage > maxVoltageLimit) {
        return PowerHigh;
    } else if (ina219d.voltage < minVoltageLimit) {
        return PowerLow;
    } else {
        return PowerOk;
    }
}

//*********************************************************
uint16_t rssi;
void readRSSI(void) {
    rssi = uint16_t (abs(WiFi.RSSI()));
}
//*********************************************************
