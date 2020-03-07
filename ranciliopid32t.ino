#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>
#include <stdio.h>
#define LVGL_TICK_PERIOD 20
#include "userConfig.h" // needs to be configured by the user
//#include "icon_coffee_40.c"
#include "helper.h"
#include "scale.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ArduinoOTA.h>

#define MY_CHART_SYMBOL "\xEF\x87\xBE"
#define MY_CAL_SYMBOL "\xEF\x81\x9B"
#define MY_SCALE_SYMBOL "\xEF\x89\x8E"
#define MY_PID_SYMBOL "\xEF\x87\x9E"
#define MY_SHOT_SYMBOL "\xEF\x89\x92"
#define MY_WATER_SYMBOL "\xEF\x81\x83"
#define MY_STEAM_SYMBOL "\xEF\x8B\x8C"
#define MY_SYMBOL_STOP "\xEF\x81\x9E"
#define MY_SYMBOL_DOWNLOAD "\xEF\x83\xAD"
#define LV_USE_THEME_ZEN        1
const char* sysVersion PROGMEM  = "Version 1.9.5-c Master";

/********************************************************
  definitions below must be changed in the userConfig.h file
******************************************************/
int Offlinemodus = OFFLINEMODUS;
//const int Display = DISPLAY;
const int OnlyPID = ONLYPID;
const int TempSensor = TEMPSENSOR;
const int Brewdetection = BREWDETECTION;
const int fallback = FALLBACK;
const int triggerType = TRIGGERTYPE;
const int TSICPIN = TPIN;
const boolean ota = OTA;
// create timer
hw_timer_t * timer = NULL;
volatile int interruptCounter;
int totalInterruptCounter;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// Wifi
const char* auth = AUTH;
  const char* ssid = D_SSID;
  const char* pass = PASS;

unsigned long lastWifiConnectionAttempt = millis();
const unsigned long wifiConnectionDelay = 10000; // try to reconnect every 5 seconds
unsigned int wifiReconnects = 0; //number of reconnects



// OTA
const char* OTAhost;
const char* OTApass;

//Blynk
const char* blynkaddress  = BLYNKADDRESS;
#define BLYNK_PRINT Serial
//#include <BlynkSimpleEsp8266.h> old esp
#include <BlynkSimpleEsp32.h>
/**********************
    STATIC PROTOTYPES
 **********************/
static void create_tab1(lv_obj_t * parent);
static void create_tab2(lv_obj_t * parent);
static void create_tab3(lv_obj_t * parent);



Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static lv_color_t buf2[LV_HOR_RES_MAX * 10];

/**********************
    GLOBAL VARIABLES
 **********************/
float soll = 95;
float ist;
int P = 699;
int I = 399;
int D = 100;
bool NOTIFY;
static char p_id[4];
static char pi_d[4];
static char pid_[4];
/********************
  gui global objects
*********************/
lv_obj_t * tv;
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3;
lv_obj_t * btnPlus;
lv_obj_t * btnMinus;
lv_obj_t * label;
lv_obj_t * labelStartInfo;
lv_obj_t * labelPlus;
lv_obj_t * labelMinus;
lv_obj_t * labelSetPoint;
lv_obj_t * labelSet;
lv_obj_t * labelP;
lv_obj_t * labelI;
lv_obj_t * labelD;
lv_obj_t * lmeter;
lv_obj_t * lmeter1;
lv_obj_t * labelW;
lv_obj_t * labelBT;
lv_anim_t a;
lv_obj_t * chart;
lv_chart_series_t * s1;
lv_chart_series_t * s2;
lv_chart_series_t * s3;
lv_obj_t * arc;
lv_obj_t * arc2;
lv_obj_t * arc3;
lv_obj_t * arc4;
lv_obj_t * btnm1;
lv_obj_t * win;
lv_obj_t * list1;
lv_obj_t * txt;
//Define pins for outputs
#define pinRelayVentil    99
#define pinRelayPumpe    99
#define pinRelayHeater    22


/**********************
        MACROS
 **********************/
// Debug mode is active if #define DEBUGMODE is set
#define DEBUGMODE
#define USE_LV_LOG 0
#ifndef DEBUGMODE
#define DEBUG_println(a)
#define DEBUG_print(a)
#define DEBUGSTART(a)
#else
#define DEBUG_println(a) Serial.println(a);
#define DEBUG_print(a) Serial.print(a);
#define DEBUGSTART(a) Serial.begin(a);
#endif
/**********************
     GLOBAL FUNCTIONS
 **********************/

/**
   Create a test screen with a lot objects and apply the given theme on them
   @param th pointer to a theme
*/
void lv_test_theme_1(lv_theme_t * th)
{
  lv_theme_set_current(th);
  th = lv_theme_get_current();    /*If `LV_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
  lv_obj_t * scr = lv_cont_create(NULL, NULL);
  lv_disp_load_scr(scr);

  tv = lv_tabview_create(scr, NULL);
  lv_obj_set_size(tv, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_tabview_set_btns_pos(tv, LV_TABVIEW_BTNS_POS_BOTTOM);
  tab1 = lv_tabview_add_tab(tv, LV_SYMBOL_HOME);
  tab2 = lv_tabview_add_tab(tv, LV_SYMBOL_SHUFFLE);
  tab3 = lv_tabview_add_tab(tv, LV_SYMBOL_SETTINGS);

  create_tab1(tab1);
  create_tab2(tab2);
  create_tab3(tab3);
  // statusbar();

}





/**********************
     STATIC FUNCTIONS
 **********************/




static void event_handler_settings(lv_obj_t * obj, lv_event_t event)
{

}
static void event_handler_hide1(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_RELEASED) {
    lv_obj_set_hidden(win, true);
    lv_win_clean(win);
  }
}
static void event_handler_hide(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_RELEASED) {
    lv_obj_set_hidden(win, true);
    lv_obj_t * scrl = lv_page_get_scrl(win);
    lv_obj_clean(scrl);
    lv_win_clean(win);
  }
}
void list_settings(void)
{
  /*Add control button to the header*/
  lv_obj_t * close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);           /*Add close button and use built-in close action*/
  lv_obj_set_event_cb(close_btn, event_handler_hide);
  lv_win_add_btn(win, LV_SYMBOL_OK);        /*Add a setup button*/
  /*Create a list*/
  list1 = lv_list_create(win, NULL);
  lv_obj_set_size(list1, 160, 200);
  lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);

  /*Add buttons to the list*/

  lv_obj_t * list_btn;

  list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
  lv_obj_set_event_cb(list_btn, event_handler_settings);

  list_btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
  lv_obj_set_event_cb(list_btn, event_handler_settings);

  list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
  lv_obj_set_event_cb(list_btn, event_handler_settings);

  list_btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
  lv_obj_set_event_cb(list_btn, event_handler_settings);

  list_btn = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
  lv_obj_set_event_cb(list_btn, event_handler_settings);
}
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{

  lv_tick_inc(LVGL_TICK_PERIOD);
}


bool my_input_read(lv_indev_drv_t * indev, lv_indev_data_t *data )
{

  // Use TFT_eSPI for touch events
  uint8_t bPressed = 0;
  uint16_t nX = 0;
  uint16_t nY = 0;
  static uint16_t prev_x, prev_y;

  bPressed = tft.getTouch(&nX, &nY);

  if (bPressed > 0) {
    data->point.x = nX;
    data->point.y = nY;
    data->state = LV_INDEV_STATE_PR;
    prev_x = data->point.x;
    prev_y = data->point.y;
  } else {
    data->point.x = prev_x;
    data->point.y = prev_y;
    data->state = LV_INDEV_STATE_REL;
  }

  return false; //No buffering so no more data read/
}

/********************************************************
   Vorab-Konfig
******************************************************/
int pidON = 1 ;                 // 1 = control loop in closed loop
int relayON, relayOFF;          // used for relay trigger type. Do not change!
boolean kaltstart = true;       // true = Rancilio started for first time
boolean emergencyStop = false;  // Notstop bei zu hoher Temperatur

/********************************************************
   moving average - Brüherkennung
*****************************************************/
const int numReadings = 15;             // number of values per Array
float readingstemp[numReadings];        // the readings from Temp
float readingstime[numReadings];        // the readings from time
float readingchangerate[numReadings];

int readIndex = 1;              // the index of the current reading
double total = 0;               // the running
int totaltime = 0 ;             // the running time
double heatrateaverage = 0;     // the average over the numReadings
double changerate = 0;          // local change rate of temprature
double heatrateaveragemin = 0 ;
unsigned long  timeBrewdetection = 0 ;
int timerBrewdetection = 0 ;
int i = 0;
int firstreading = 1 ;          // Ini of the field, also used for sensor check

/********************************************************
   PID - Werte Brüherkennung Offline
*****************************************************/

double aggbKp = AGGBKP;
double aggbTn = AGGBTN;
double aggbTv = AGGBTV;
#if (aggbTn == 0)
double aggbKi = 0;
#else
double aggbKi = aggbKp / aggbTn;
#endif
double aggbKd = aggbTv * aggbKp ;
double brewtimersoftware = 45;    // 20-5 for detection
double brewboarder = 150 ;        // border for the detection,
const int PonE = PONE;
// be carefull: to low: risk of wrong brew detection
// and rising temperature

/********************************************************
   Analog Schalter Read
******************************************************/
const int analogPin = 0; // will be use in case of hardware
int brewcounter = 0;
int brewswitch = 0;


int brewtime = 25000;
long aktuelleZeit = 0;
int totalbrewtime = 0;
int preinfusion = 2000;
int preinfusionpause = 5000;
unsigned long bezugsZeit = 0;
unsigned long startZeit = 0;

/********************************************************
   Sensor check
******************************************************/
boolean sensorError = false;
int error = 0;
int maxErrorCounter = 10 ;  //depends on intervaltempmes* , define max seconds for invalid data

/********************************************************
   PID
******************************************************/
#include "PID_v1.h"

unsigned long previousMillistemp;  // initialisation at the end of init()
const long intervaltempmestsic = 400 ;
const long intervaltempmesds18b20 = 400  ;
int pidMode = 1; //1 = Automatic, 0 = Manual

const unsigned int windowSize = 1000;
volatile  unsigned int isrCounter = 0;  // counter for ISR
unsigned long windowStartTime;
double Input, Output, setPointTemp;  //
double previousInput = 0;

double setPoint = SETPOINT;
double aggKp = AGGKP;
double aggTn = AGGTN;
double aggTv = AGGTV;
double startKp = STARTKP;
double startTn = STARTTN;
#if (startTn == 0)
double startKi = 0;
#else
double startKi = startKp / startTn;
#endif

double starttemp = STARTTEMP;
#if (aggTn == 0)
double aggKi = 0;
#else
double aggKi = aggKp / aggTn;
#endif
double aggKd = aggTv * aggKp ;


PID bPID(&Input, &Output, &setPoint, aggKp, aggKi, aggKd, PonE, DIRECT);

/********************************************************
   DALLAS TEMP
******************************************************/
// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 99
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress sensorDeviceAddress;

/********************************************************
   B+B Sensors TSIC 306
******************************************************/
#include "TSIC.h"       // include the library
TSIC Sensor1(TSICPIN);    // only Signalpin, VCCpin unused by default
uint16_t temperature = 0;
float Temperatur_C = 0;
//Update Intervall zur App
unsigned long previousMillisBlynk;  // initialisation at the end of init()
const long intervalBlynk = 1000;
int blynksendcounter = 1;

//Update für Display
unsigned long previousMillisDisplay;  // initialisation at the end of init()
const long intervalDisplay = 500;

/********************************************************
   BLYNK WERTE EINLESEN und Definition der PINS
******************************************************/



BLYNK_CONNECTED() {
  if (Offlinemodus == 0) {
    Blynk.syncAll();
    //rtc.begin();
  }
}

BLYNK_WRITE(V4) {
  aggKp = param.asDouble();
}

BLYNK_WRITE(V5) {
  aggTn = param.asDouble();
}
BLYNK_WRITE(V6) {
  aggTv =  param.asDouble();
}

BLYNK_WRITE(V7) {
  setPoint = param.asDouble();
}

BLYNK_WRITE(V8) {
  brewtime = param.asDouble() * 1000;
}

BLYNK_WRITE(V9) {
  preinfusion = param.asDouble() * 1000;
}

BLYNK_WRITE(V10) {
  preinfusionpause = param.asDouble() * 1000;
}
BLYNK_WRITE(V11) {
  startKp = param.asDouble();
}
BLYNK_WRITE(V12) {
  starttemp = param.asDouble();
}
BLYNK_WRITE(V13)
{
  pidON = param.asInt();
}
BLYNK_WRITE(V14)
{
  startTn = param.asDouble();
}
BLYNK_WRITE(V30)
{
  aggbKp = param.asDouble();//
}

BLYNK_WRITE(V31) {
  aggbTn = param.asDouble();
}
BLYNK_WRITE(V32) {
  aggbTv =  param.asDouble();
}
BLYNK_WRITE(V33) {
  brewtimersoftware =  param.asDouble();
}
BLYNK_WRITE(V34) {
  brewboarder =  param.asDouble();
}

/********************************************************
  Moving average - brewdetection (SW)
*****************************************************/

void movAvg() {
  if (firstreading == 1) {
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readingstemp[thisReading] = Input;
      readingstime[thisReading] = 0;
      readingchangerate[thisReading] = 0;
    }
    firstreading = 0 ;
  }

  readingstime[readIndex] = millis() ;
  readingstemp[readIndex] = Input ;

  if (readIndex == 0) { // geaendert numReadings - 1 auf 0
    changerate = (readingstemp[0] - readingstemp[numReadings - 1]) / (readingstime[0] - readingstime[numReadings - 1]) * 10000;
  } else {
    changerate = (readingstemp[readIndex] - readingstemp[readIndex - 1]) / (readingstime[readIndex] - readingstime[readIndex - 1]) * 10000;
  }

  readingchangerate[readIndex] = changerate ;
  total = 0 ;
  for (i = 0; i < numReadings; i++)
  {
    total += readingchangerate[i];
  }

  heatrateaverage = total / numReadings * 100 ;
  if (heatrateaveragemin > abs(heatrateaverage)) {
    heatrateaveragemin = abs(heatrateaverage) ;
  }

  if (readIndex >= numReadings - 1) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  else {
    readIndex++;
  }
}
/********************************************************
  check sensor value. If < 0 or difference between old and new >25, then increase error.
  If error is equal to maxErrorCounter, then set sensorError
*****************************************************/
boolean checkSensor(float tempInput) {
  boolean sensorOK = false;
  /********************************************************
    sensor error
  ******************************************************/
  if ((tempInput < 0 || abs(tempInput - previousInput) > 25) && !sensorError) {
    error++;
    sensorOK = false;
    DEBUG_print("Error counter: ");
    DEBUG_println(error);
    DEBUG_print("temp delta: ");
    DEBUG_println(tempInput);
  } else if (tempInput > 0) {
    error = 0;
    sensorOK = true;
  }
  if (error >= maxErrorCounter && !sensorError) {
    sensorError = true ;
    DEBUG_print("Sensor Error");
    DEBUG_println(Input);
  } else if (error == 0) {
    sensorError = false ;
  }

  return sensorOK;
}
/********************************************************
  Refresh temperature.
  Each time checkSensor() is called to verify the value.
  If the value is not valid, new data is not stored.
*****************************************************/
void refreshTemp() {
  /********************************************************
    Temp. Request
  ******************************************************/
  unsigned long currentMillistemp = millis();
  previousInput = Input ;
  if (TempSensor == 1)
  {
    if (currentMillistemp - previousMillistemp >= intervaltempmesds18b20)
    {
      previousMillistemp += intervaltempmesds18b20;
      sensors.requestTemperatures();
      if (!checkSensor(sensors.getTempCByIndex(0)) && firstreading == 0) return;  //if sensor data is not valid, abort function
      Input = sensors.getTempCByIndex(0);
      if (Brewdetection == 1) {
        movAvg();
      } else {
        firstreading = 0;
      }
    }
  }
  if (TempSensor == 2)
  {
    if (currentMillistemp - previousMillistemp >= intervaltempmestsic)
    {
      previousMillistemp += intervaltempmestsic;
      /*  variable "temperature" must be set to zero, before reading new data
            getTemperature only updates if data is valid, otherwise "temperature" will still hold old values
      */
      temperature = 0;
      Sensor1.getTemperature(&temperature);
      Temperatur_C = Sensor1.calc_Celsius(&temperature);
      if (!checkSensor(Temperatur_C) && firstreading == 0) return;  //if sensor data is not valid, abort function
      Input = Temperatur_C;
      // Input = random(50,70) ;// test value
      if (Brewdetection == 1)
      {
        movAvg();
      } else {
        firstreading = 0;
      }
    }
  }
}
/********************************************************
    PreInfusion, Brew , if not Only PID
******************************************************/
void brew() {
  if (OnlyPID == 0) {
    brewswitch = analogRead(analogPin);
    unsigned long aktuelleZeit = millis();
    if (brewswitch > 1000 && brewcounter == 0) {
      startZeit = millis();
      brewcounter = brewcounter + 1;
    }
    if (brewcounter >= 1) {
      bezugsZeit = aktuelleZeit - startZeit;
    }

    totalbrewtime = preinfusion + preinfusionpause + brewtime;
    if (brewswitch > 1000 && bezugsZeit < totalbrewtime && brewcounter >= 1) {
      if (bezugsZeit < preinfusion) {
        //DEBUG_println("preinfusion");
        digitalWrite(pinRelayVentil, relayON);
        digitalWrite(pinRelayPumpe, relayON);
      }
      if (bezugsZeit > preinfusion && bezugsZeit < preinfusion + preinfusionpause) {
        //DEBUG_println("Pause");
        digitalWrite(pinRelayVentil, relayON);
        digitalWrite(pinRelayPumpe, relayOFF);
      }
      if (bezugsZeit > preinfusion + preinfusionpause) {
        //DEBUG_println("Brew");
        digitalWrite(pinRelayVentil, relayON);
        digitalWrite(pinRelayPumpe, relayON);
      }
    } else {
      //DEBUG_println("aus");
      digitalWrite(pinRelayVentil, relayOFF);
      digitalWrite(pinRelayPumpe, relayOFF);
    }
    if (brewswitch < 1000 && brewcounter >= 1) {
      brewcounter = 0;
      aktuelleZeit = 0;
      bezugsZeit = 0;
    }
  }
}
/********************************************************
  Check if Wifi is connected, if not reconnect
*****************************************************/
void checkWifi() {
  if (Offlinemodus == 1) return;
  int statusTemp = WiFi.status();
  // check WiFi connection:
  if (statusTemp != WL_CONNECTED) {
    // (optional) "offline" part of code

    // check delay:
    if (millis() - lastWifiConnectionAttempt >= wifiConnectionDelay) {
      lastWifiConnectionAttempt = millis();
      // attempt to connect to Wifi network:
      WiFi.begin(ssid, pass);
      delay(5000);    //will not work without delay
      wifiReconnects++;
    }

  }
}
/********************************************************
  send data to Blynk server
*****************************************************/

void sendToBlynk() {
  if (Offlinemodus != 0) return;
  unsigned long currentMillisBlynk = millis();
  if (currentMillisBlynk - previousMillisBlynk >= intervalBlynk) {
    previousMillisBlynk += intervalBlynk;
    if (Blynk.connected()) {
      if (blynksendcounter == 1) {
        Blynk.virtualWrite(V2, Input);
        Blynk.syncVirtual(V2);
      }
      if (blynksendcounter == 2) {
        Blynk.virtualWrite(V23, Output);
        Blynk.syncVirtual(V23);
      }
      if (blynksendcounter == 3) {
        Blynk.virtualWrite(V3, setPoint);
        Blynk.syncVirtual(V3);
      }
      if (blynksendcounter == 4) {
        Blynk.virtualWrite(V35, heatrateaverage);
        Blynk.syncVirtual(V35);
      }
      if (blynksendcounter >= 5) {
        Blynk.virtualWrite(V36, heatrateaveragemin);
        Blynk.syncVirtual(V36);
        blynksendcounter = 0;
      }
      blynksendcounter++;
    }
  }
}
/********************************************************
    Brewdetection
******************************************************/
void brewdetection() {
  if (brewboarder == 0) return; //abort brewdetection if deactivated

  // Brew detecion == 1 software solution , == 2 hardware
  if (Brewdetection == 1 || Brewdetection == 2) {
    if (millis() - timeBrewdetection > brewtimersoftware * 1000) {
      timerBrewdetection = 0 ;
      if (OnlyPID == 1) {
        bezugsZeit = 0 ;
      }
    }
  }

  if (Brewdetection == 1) {
    if (heatrateaverage <= -brewboarder && timerBrewdetection == 0 ) {
      DEBUG_println("SW Brew detected") ;
      timeBrewdetection = millis() ;
      timerBrewdetection = 1 ;
    }
  }
}


/********************************************************
    Timer 1 - ISR für PID Berechnung und Heizrelais-Ausgabe
******************************************************/
//void ICACHE_RAM_ATTR onTimer1ISR() { old esp82
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);

  //timer1_write(50000); // set interrupt time to 10ms
  // timerAlarmWrite(timer, 10000, true);


}


/***********************
      Event Handler
 ***********************/
#include "events.h"
/*********************
      HOME TAB
  *********************/
#include "tab1.h"

/*********************
      GRAPH TAB
 ********************/
#include "tab2.h"
/*******************
    Settings Tab
**********************/
#include "tab3.h"

//#include "status.h"
 /* init filesystem and load config */
#include "extConfig.h"

void setup() {

  Serial.begin(115200); /* prepare for possible serial debug */
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);

  Serial.println("Mounting FS...");

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }


  if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("Config saved");
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
  }




  lv_init();
  if (SCALE == 1) {
    initScale();
  }
#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin(); /* TFT init */
  tft.setRotation(1); /* Landscape orientation */

  lv_disp_buf_init(&disp_buf, buf, buf2, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);




  // calibration touch pad

  uint16_t calData[5] = { 223, 3497, 351, 3465, 1 };
  tft.setTouch(calData);
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_input_read;
  lv_indev_drv_register(&indev_drv);

  /*Initialize the graphics library's tick*/
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  lv_theme_t *th = lv_theme_zen_init(200, NULL);
  lv_theme_set_current(th);

  lv_test_theme_1(th);

  // settings
  /*Create style for the Arcs*/
  static lv_style_t style_loader;
  lv_style_copy(&style_loader, &lv_style_plain);
  style_loader.line.color = LV_COLOR_NAVY;           /*Arc color*/
  style_loader.line.width = 10;                       /*Arc width*/

  /*Create an Arc*/
  arc4 = lv_arc_create(lv_scr_act(), NULL);
  lv_arc_set_angles(arc4, 180, 180);
  lv_arc_set_style(arc4, LV_ARC_STYLE_MAIN, &style_loader);
  lv_obj_align(arc4, NULL, LV_ALIGN_CENTER, 0, 0);

  /* Create an `lv_task` to update the arc.
     Store the `arc` in the user data*/
  lv_task_create(arc_loader, 20, LV_TASK_PRIO_LOWEST, arc4);


  labelStartInfo = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_align(labelStartInfo, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_obj_align(labelStartInfo, arc4, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_label_set_text(labelStartInfo, "Startup...");
  lv_task_handler();

  /********************************************************
     Define trigger type
   ******************************************************/
  if (triggerType)
  {
    relayON = HIGH;
    relayOFF = LOW;
  } else {
    relayON = LOW;
    relayOFF = HIGH;
  }

  /********************************************************
    Ini Pins
  ******************************************************/
  pinMode(pinRelayVentil, OUTPUT);
  pinMode(pinRelayPumpe, OUTPUT);
  pinMode(pinRelayHeater, OUTPUT);
  digitalWrite(pinRelayVentil, relayOFF);
  digitalWrite(pinRelayPumpe, relayOFF);
  digitalWrite(pinRelayHeater, LOW);

  /********************************************************
       BLYNK & Fallback offline
    ******************************************************/
  if (Offlinemodus == 0) {

    if (fallback == 0) {
      lv_label_set_text(labelStartInfo, "Connect to Blynk, no Fallback");
      lv_task_handler();

      //      displaymessage("Connect to Blynk", "no Fallback");
      Blynk.begin(auth, ssid, pass, blynkaddress, 8080);
    }

    if (fallback == 1) {
      unsigned long started = millis();
      lv_label_set_text(labelStartInfo, "Connect Wifi...");
      lv_task_handler();

      //      displaymessage("1: Connect Wifi to:", ssid);
      // wait 10 seconds for connection:
      /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
        would try to act as both a client and an access-point and could cause
        network-issues with your other WiFi-devices on your WiFi-network. */
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass);
      DEBUG_print("Connecting to ");
      DEBUG_print(ssid);
      DEBUG_println(" ...");

      while ((WiFi.status() != WL_CONNECTED) && (millis() - started < 20000))
      {
        yield();    //Prevent Watchdog trigger
      }

      if (WiFi.status() == WL_CONNECTED) {
        DEBUG_println("WiFi connected");
        DEBUG_println("IP address: ");
        DEBUG_println(WiFi.localIP());
        // start the check update task
        xTaskCreate(&check_update_task, "check_update_task", 8192, NULL, 5, NULL);
        lv_label_set_text(labelStartInfo, "Wifi connected, try Blynk...");
        lv_task_handler();

        //      displaymessage("2: Wifi connected, ", "try Blynk   ");
        DEBUG_println("Wifi works, now try Blynk connection");
        delay(2000);
        Blynk.config(auth, blynkaddress, 8080) ;
        Blynk.connect(30000);

        // Blnky works:
        if (Blynk.connected() == true) {
          lv_label_set_text(labelStartInfo, "Blynk connected, sync variables...");
          lv_task_handler();

          //       displaymessage("3: Blynk connected", "sync all variables...");
          DEBUG_println("Blynk is online, new values to eeprom");
          // Blynk.run() ;
          Blynk.syncVirtual(V4);
          Blynk.syncVirtual(V5);
          Blynk.syncVirtual(V6);
          Blynk.syncVirtual(V7);
          Blynk.syncVirtual(V8);
          Blynk.syncVirtual(V9);
          Blynk.syncVirtual(V10);
          Blynk.syncVirtual(V11);
          Blynk.syncVirtual(V12);
          Blynk.syncVirtual(V13);
          Blynk.syncVirtual(V14);
          Blynk.syncVirtual(V30);
          Blynk.syncVirtual(V31);
          Blynk.syncVirtual(V32);
          Blynk.syncVirtual(V33);
          Blynk.syncVirtual(V34);
          // Blynk.syncAll();  //sync all values from Blynk server
          // Werte in den eeprom schreiben
          // ini eeprom mit begin
          EEPROM.begin(1024);
          EEPROM.put(0, aggKp);
          EEPROM.put(10, aggTn);
          EEPROM.put(20, aggTv);
          EEPROM.put(30, setPoint);
          EEPROM.put(40, brewtime);
          EEPROM.put(50, preinfusion);
          EEPROM.put(60, preinfusionpause);
          EEPROM.put(70, startKp);
          EEPROM.put(80, starttemp);
          EEPROM.put(90, aggbKp);
          EEPROM.put(100, aggbTn);
          EEPROM.put(110, aggbTv);
          EEPROM.put(120, brewtimersoftware);
          EEPROM.put(130, brewboarder);
          // eeprom schließen
          EEPROM.commit();
          //          display.fillScreen(0);
        }
      }
      if (WiFi.status() != WL_CONNECTED || Blynk.connected() != true) {
        //      displaymessage("Begin Fallback,", "No Blynk/Wifi");
        lv_label_set_text(labelStartInfo, "Begin Fallback, Blynk/Wifi failed...");
        lv_task_handler();

        delay(2000);
        DEBUG_println("Start offline mode with eeprom values, no wifi or blynk :(");
        lv_label_set_text(labelStartInfo, "Start offline mode with eeprom values...");
        lv_task_handler();
        Offlinemodus = 1 ;
        // eeprom öffnen
        EEPROM.begin(1024);
        // eeprom werte prüfen, ob numerisch
        double dummy;
        EEPROM.get(0, dummy);
        DEBUG_print("check eeprom 0x00 in dummy: ");
        DEBUG_println(dummy);
        if (!isnan(aggKp)) {
          EEPROM.get(0, aggKp);
          EEPROM.get(10, aggTn);
          EEPROM.get(20, aggTv);
          EEPROM.get(30, setPoint);
          EEPROM.get(40, brewtime);
          EEPROM.get(50, preinfusion);
          EEPROM.get(60, preinfusionpause);
          EEPROM.get(70, startKp);
          EEPROM.get(80, starttemp);
          EEPROM.get(90, aggbKp);
          EEPROM.get(100, aggbTn);
          EEPROM.get(110, aggbTv);
          EEPROM.get(120, brewtimersoftware);
          EEPROM.get(130, brewboarder);
        }
        else
        {
          //       displaymessage("No eeprom,", "Value");
          lv_label_set_text(labelStartInfo, "No eeprom values, use default offline values...");
          lv_task_handler();
          DEBUG_println("No working eeprom value, I am sorry, but use default offline value  :)");
          delay(2000);
        }
        // eeeprom schließen
        EEPROM.commit();
      }
    }
  }

  /********************************************************
     OTA
  ******************************************************/
  if (ota && Offlinemodus == 0 ) {
    //wifi connection is done during blynk connection

    ArduinoOTA.setHostname(OTAhost);  //  Device name for OTA
    ArduinoOTA.setPassword(OTApass);  //  Password for OTA
    ArduinoOTA.begin();
  }


  /********************************************************
     Ini PID
  ******************************************************/

  setPointTemp = setPoint;
  bPID.SetSampleTime(windowSize);
  bPID.SetOutputLimits(0, windowSize);
  bPID.SetMode(AUTOMATIC);


  /********************************************************
     TEMP SENSOR
  ******************************************************/
  if (TempSensor == 1) {
    sensors.begin();
    sensors.getAddress(sensorDeviceAddress, 0);
    sensors.setResolution(sensorDeviceAddress, 10) ;
    sensors.requestTemperatures();
  }

  /********************************************************
    movingaverage ini array
  ******************************************************/
  if (Brewdetection == 1) {
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readingstemp[thisReading] = 0;
      readingstime[thisReading] = 0;
      readingchangerate[thisReading] = 0;
    }
  }

  //Initialisation MUST be at the very end of the init(), otherwise the time comparision in loop() will have a big offset
  unsigned long currentTime = millis();
  previousMillistemp = currentTime;
  windowStartTime = currentTime;
  previousMillisDisplay = currentTime;
  previousMillisBlynk = currentTime;

  /********************************************************
    Timer1 ISR - Initialisierung
    TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
    TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
    TIM_DIV256 = 3  //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
  ******************************************************/
  //  timer1_isr_init();
  //  timer1_attachInterrupt(onTimer1ISR);
  //  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);   // ALTE TIMER INITIALISIERUNG
  // timer1_write(50000); // set interrupt time to 10ms */
  lv_label_set_text(labelStartInfo, "startup successfull, loading tabs...");
  lv_task_handler();
  lv_obj_del(arc4);
  lv_obj_del(labelStartInfo);

}
void loop() {
  ArduinoOTA.handle();  // For OTA
  // Disable interrupt it OTA is starting, otherwise it will not work
  ArduinoOTA.onStart([]() {
    //timer1_disable();
    timerAlarmDisable(timer);

    digitalWrite(pinRelayHeater, LOW); //Stop heating
  });
  ArduinoOTA.onError([](ota_error_t error) {
    timerAlarmEnable(timer);
    //timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  });
  // Enable interrupts if OTA is finished
  ArduinoOTA.onEnd([]() {
    //timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timerAlarmEnable(timer);

  });

  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run(); //Do Blynk magic stuff
    wifiReconnects = 0;
  } else {
    checkWifi();
  }
  unsigned long startT;
  unsigned long stopT;

  refreshTemp();   //read new temperature values
  //  testEmergencyStop();  // test if Temp is to high
  brew();   //start brewing if button pressed

  //check if PID should run or not. If not, set to manuel and force output to zero
  if (pidON == 0 && pidMode == 1) {
    pidMode = 0;
    bPID.SetMode(pidMode);
    Output = 0 ;
  } else if (pidON == 1 && pidMode == 0) {
    pidMode = 1;
    bPID.SetMode(pidMode);
  }



  //Sicherheitsabfrage
  if (!sensorError && Input > 0 && !emergencyStop) {

    //Set PID if first start of machine detected
    if (Input < starttemp && kaltstart) {
      if (startTn != 0) {
        startKi = startKp / startTn;
      } else {
        startKi = 0 ;
      }
      bPID.SetTunings(startKp, startKi, 0);
    } else {
      // calc ki, kd
      if (aggTn != 0) {
        aggKi = aggKp / aggTn ;
      } else {
        aggKi = 0 ;
      }
      aggKd = aggTv * aggKp ;
      bPID.SetTunings(aggKp, aggKi, aggKd);
      kaltstart = false;
    }

    //if brew detected, set PID values
    brewdetection();
    if ( millis() - timeBrewdetection  < brewtimersoftware * 1000 && timerBrewdetection == 1) {
      // calc ki, kd
      if (aggbTn != 0) {
        aggbKi = aggbKp / aggbTn ;
      } else {
        aggbKi = 0 ;
      }
      aggbKd = aggbTv * aggbKp ;
      bPID.SetTunings(aggbKp, aggbKi, aggbKd) ;
      if (OnlyPID == 1) {
        bezugsZeit = millis() - timeBrewdetection ;
      }
    }

    sendToBlynk();

    //update display if time interval xpired

    unsigned long currentMillisDisplay = millis();
    if (currentMillisDisplay - previousMillisDisplay >= intervalDisplay) {
      previousMillisDisplay += intervalDisplay;
      /****************************
               CALL EVENT
       ***************************/

      int cur_tab_id = lv_tabview_get_tab_act(tv);
      if (cur_tab_id == 0 ) {
        lv_event_send(lmeter, LV_EVENT_REFRESH, NULL);
        lv_event_send(label, LV_EVENT_REFRESH, NULL);
        lv_event_send(lmeter1, LV_EVENT_REFRESH, NULL);
        lv_event_send(arc, LV_EVENT_REFRESH, NULL);
      } else if (cur_tab_id == 1) {
        lv_event_send(chart, LV_EVENT_REFRESH, NULL);

      }

    } else if (sensorError) {

      //Deactivate PID
      if (pidMode == 1) {
        pidMode = 1;
        bPID.SetMode(pidMode);
        Output = 1 ;
      }

      digitalWrite(pinRelayHeater, LOW); //Stop heating


    } else if (emergencyStop) {

      //Deactivate PID
      if (pidMode == 1) {
        pidMode = 0;
        bPID.SetMode(pidMode);
        Output = 0 ;
      }
      digitalWrite(pinRelayHeater, LOW); //Stop heating

    }
  }
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    totalInterruptCounter++;
    if (Output != isrCounter) { // führt dazu, dass der PWR immer sicher an oder aus ist (kein unnötiger Impuls bei Output=isrCounter)
      if (Output < isrCounter) {
        digitalWrite(pinRelayHeater, relayOFF);
        //DEBUG_println("Power off!");
      } else {
        digitalWrite(pinRelayHeater, relayON);
        //DEBUG_println("Power on!");
      }
    }
    isrCounter += 10; // += 10 because one tick = 10ms
    //set PID output as relais commands
    if (isrCounter > windowSize) {
      isrCounter = 0;
    }


    //run PID calculation
    bPID.Compute();

  }

  lv_task_handler();
}
