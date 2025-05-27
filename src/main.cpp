#include <Arduino.h>
#include <WiFi.h>

/*****************************
 *     TimeLib Library       *
 *         start             *
 ****************************/
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <time.h>
/*****************************
 *     TimeLib Library       *
 *         end               *
 ****************************/

/******************************
 *     Firebase Library       *
 *         start              *
 *****************************/
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
/*****************************
 *     Firebase Library      *
 *         end               *
 *****************************/

/*****************************
 *    MQTT Library           *
 *        start              *
 ****************************/
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
/*****************************
 *    MQTT Library           *
 *        end                *
 ****************************/

/*****************************
 *    DS18B20 Library        *
 *        start              *
 ****************************/
#include <OneWire.h>
#include <DallasTemperature.h>
/*****************************
 *    DS18B20 Library        *
 *        end                *
 ****************************/

/*******************************
 *   ESP Mail Client Library   *
 *       start                 *
 ******************************/
#include <ESP_Mail_Client.h>
/*******************************
 *   ESP Mail Client Library   *
 *       end                   *
 ******************************/

/*****************************
 *    Debugging Macros       *
 *        start              *
 ****************************/
#define DEBUG_AMTIME 1
#define DEBUG_AMTIME_RETRIEVE 1
#define DEBUG_PMTime 1
#define DEBUG_PMTIME_RETRIEVE 1
#define DEBUG_SENSOR_READINGS 1
#define DEBUG_STORE_COUNT 1
#define DEBUG_CONNECT_TO_FIREBASE 1
#define DEBUG_DATE_TO_FIREBASE 1
#define DEBUG_RECIEVED_DATE_COUNT 1
#define DEBUG_SENSOR_READINGS 1
#define DEBUG_SENSOR_CHANGE 1
#define DEBUG_RELAY_CONTROL 1
#define DEBUG_RSSI 1
#define DEBUG_RMS_CURRENT 1
#define DEBUG_RELAY_STATE 1
/*****************************
 *    Debugging Macros       *
 *    definitoins            *
 *        end                *
 ****************************/
#if DEBUG_AMTIME
#define DEBUG_PRINT_AMTIME(x) Serial.print(x)
#define DEBUG_PRINTLN_AMTIME(x) Serial.println(x)
#else
#define DEBUG_PRINT_AMTIME(x)
#define DEBUG_PRINTLN_AMTIME(x)
#endif

#if DEBUG_AMTIME_RETRIEVE
#define DEBUG_PRINT_AMTIME_RETRIEVE(x) Serial.print(x)
#define DEBUG_PRINTLN_AMTIME_RETRIEVE(x) Serial.println(x)
#else
#define DEBUG_PRINT_AMTIME_RETRIEVE(x)
#define DEBUG_PRINTLN_AMTIME_RETRIEVE(x)
#endif

#if DEBUG_PMTIME
#define DEBUG_PRINT_PMTIME(x) Serial.print(x)
#define DEBUG_PRINTLN_PMTIME(x) Serial.println(x)
#else
#define DEBUG_PRINT_PMTIME(x)
#define DEBUG_PRINTLN_PMTIME(x)
#endif

#if DEBUG_PMTIME_RETRIEVE
#define DEBUG_PRINT_PMTIME_RETRIEVE(x) Serial.print(x)
#define DEBUG_PRINTLN_PMTIME_RETRIEVE(x) Serial.println(x)
#else
#define DEBUG_PRINT_PMTIME_RETRIEVE(x)
#define DEBUG_PRINTLN_PMTIME_RETRIEVE(x)
#endif

#if DEBUG_EXTRACTED_SENSOR_READINGS
#define DEBUG_PRINT_EXTRACTED_SENSOR_READINGS(x) Serial.print(x)
#define DEBUG_PRINTLN_EXTRACTED_SENSOR_READINGS(x) Serial.println(x)
#else
#define DEBUG_PRINT_EXTRACTED_SENSOR_READINGS(x)
#define DEBUG_PRINTLN_EXTRACTED_SENSOR_READINGS(x)
#endif

#if DEBUG_STORE_COUNT
#define DEBUG_PRINT_STORE_COUNT(x) Serial.print(x)
#define DEBUG_PRINTLN_STORE_COUNT(x) Serial.println(x)
#else
#define DEBUG_PRINT_STORE_COUNT(x)
#define DEBUG_PRINTLN_STORE_COUNT(x)
#endif

#if DEBUG_CONNECT_TO_FIREBASE
#define DEBUG_PRINT_CONNECT_TO_FIREBASE(x) Serial.print(x)
#define DEBUG_PRINTLN_CONNECT_TO_FIREBASE(x) Serial.println(x)
#else
#define DEBUG_PRINT_CONNECT_TO_FIREBASE(x)
#define DEBUG_PRINTLN_CONNECT_TO_FIREBASE(x)
#endif

#if DEBUG_DATE_TO_FIREBASE
#define DEBUG_PRINT_DATE_TO_FIREBASE(x) Serial.print(x)
#define DEBUG_PRINTLN_DATE_TO_FIREBASE(x) Serial.println(x)
#else
#define DEBUG_PRINT_DATE_TO_FIREBASE(x)
#define DEBUG_PRINTLN_DATE_TO_FIREBASE(x)
#endif

#if DEBUG_RECIEVED_DATE_COUNT
#define DEBUG_PRINT_RECIEVED_DATE_COUNT(x) Serial.print(x)
#define DEBUG_PRINTLN_RECIEVED_DATE_COUNT(x) Serial.println(x)
#else
#define DEBUG_PRINT_RECIEVED_DATE_COUNT(x)
#define DEBUG_PRINTLN_RECIEVED_DATE_COUNT(x)
#endif

#if DEBUG_SENSOR_READINGS
#define DEBUG_PRINT_SENSOR_READINGS(x) Serial.print(x)
#define DEBUG_PRINTLN_SENSOR_READINGS(x) Serial.println(x)
#define DEBUG_PRINT_SENSOR_READINGS_RETRIEVE(x) Serial.print(x)
#else
#define DEBUG_PRINT_SENSOR_READINGS(x)
#define DEBUG_PRINTLN_SENSOR_READINGS(x)
#define DEBUG_PRINT_SENSOR_READINGS_RETRIEVE(x)
#endif

#if DEBUG_SENSOR_CHANGE
#define DEBUG_PRINT_SENSOR_CHANGE(x) Serial.print(x)
#define DEBUG_PRINTLN_SENSOR_CHANGE(x) Serial.println(x)
#else
#define DEBUG_PRINT_SENSOR_CHANGE(x)
#define DEBUG_PRINTLN_SENSOR_CHANGE(x)
#endif

#if DEBUG_RELAY_CONTROL
#define DEBUG_PRINT_RELAY_CONTROL(x) Serial.print(x)
#define DEBUG_PRINTLN_RELAY_CONTROL(x) Serial.println(x)
#else
#define DEBUG_PRINT_RELAY_CONTROL(x)
#define DEBUG_PRINTLN_RELAY_CONTROL(x)
#endif

#if DEBUG_PRINT_RSSI
#define DEBUG_PRINT_RSSI(x) Serial.print(x)
#define DEBUG_PRINTLN_RSSI(x) Serial.println(x)
#else
#define DEBUG_PRINT_RSSI(x)
#define DEBUG_PRINTLN_RSSI(x)
#endif

#if DEBUG_RMS_CURRENT
#define DEBUG_PRINT_RMS_CURRENT(x) Serial.print(x)
#define DEBUG_PRINTLN_RMS_CURRENT(x, y) Serial.println(x, y)
#else
#define DEBUG_PRINT_RMS_CURRENT(x)
#define DEBUG_PRINTLN_RMS_CURRENT(x, y)
#endif

#if DEBUG_RELAY_STATE
#define DEBUG_PRINT_RELAY_STATE(x) Serial.print(x)
#define DEBUG_PRINTLN_RELAY_STATE(x) Serial.println(x)
#else
#define DEBUG_PRINTLN_RELAY_STATE(x)
#define DEBUG_PRINTLN_RELAY_STATE(x)
#endif

/*****************************
 *    Debugging Macros       *
 *        end                *
 ****************************/
/*****************************
 *     Function Prototypes    *
 *         start              *
 ****************************/
void getTime();
void ConnectToWiFi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void publishTimeToMQTT();
void publishSensorValues();
void upDateSensors();
void checkMemory();
void retrieveLastStoredData();
void storeDataToFirebase();
void connectToFirebase();
void storeDateToFirebase();
void tokenStatusCallback(TokenInfo info);
void relay_Control();
void retrieveStoreCount();
void retrieveDateCount();
void setTargetTemperature();
void checkCurrent();
void sendEmail(const String &subject, const String &message);
void setSystemTimeFromNTP();
/*****************************
 *     Function Prototypes    *
 *         end                *
 ****************************/

/******************************
 *      WiFi Credentials      *
 *           start            *
 * ****************************/

const char *ssid = "Gimp";
const char *password = "FC7KUNPX";

/******************************
 *      WiFi Credentials      *
 *           end              *
 * ****************************/

/******************************
 *     MQTT Credentials       *
 *         start              *
 ******************************/

const char *mqtt_server = "ea53fbd1c1a54682b81526905851077b.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "ESP32FireBaseTortoise";
const char *mqtt_password = "ESP32FireBaseHea1951Ter";
const char *red_topic = "heater";
const char *green_topic = "coolSide";
const char *blue_topic = "outSide";
const char *time_topic = "test/time";        // not used
const char *storeCount_topic = "storeCount"; // Counter for MQTT
// Define new MQTT topics for hour and minute
const char *hour_topic = "time/hour";     // not used
const char *minute_topic = "time/minute"; // not used
/******************************
 *     MQTT Credentials       *
 *         end                *
 ******************************/
/******************************
 *    Email Credentials       *
 *         start              *
 *****************************/
#define SENDER_EMAIL "esp8266heaterapp@gmail.com";
#define SENDER_PASSWORD "uuyd ifav mpzd vuoj";
#define RECIPIENT_EMAIL "mac5y4@talktalk.net"
#define SMTP_HOST "smtp.gmail.com";
#define SMTP_PORT 587;
/******************************
 *    Email Credentials       *
 *         end                *
 *****************************/

SMTPSession smtp; // Define the SMTP session object globaly

/******************************
 *    Create a secure client  *
 *        for MQTT            *
 *         start              *
 ******************************/
WiFiClientSecure espClient;
PubSubClient client(espClient);

/******************************
 *    Create a secure client  *
 *        for MQTT            *
 *         end                *
 ******************************/

#define ONE_WIRE_BUS 4 // Data wire is plugged into port 2 on the Wemos D1 Mini

/*************************
 *   Pin Definitions     *
 *       start           *
 ************************/
#define Relay_Pin 5
#define LED_Pin 2 // Built in LED
/*****************************
 * This line is readt to be  *
 * used for the LED pin's    *
 **************************************************************************************************/
// Add more LED's LED-1,LED_2,LED_3,LED_4 LED = Power on/off,LED_1 = Conetted to WiFi,LED_2 = Connected to MQTT,LED_3 = Connected to Firebase,LED_4 = Heater on/off
OneWire ds(4);
/*************************
 *   Pin Definitions     *
 *       end             *
 ************************/

/*************************
 *    Time Variables     *
 *       start           *
 ************************/
unsigned long presentTime = millis();
unsigned long previousTime = millis();
unsigned long interval = 3600000; // change to 1200000 - 20 minutes
bool firstRunE_Mail = true;    // Flag to check if it's the first run for email sending
int currentDay;
int currentMonth = 1;
/******************************************
 *      remove once proven to work        */

int nextDay = 1; // Set to 2 for testing

int timeCount = 0; // timeCount is for testing only
/****************************************/
int asBeenSaved = false;
uint_fast8_t amTemperature; // is set by the sliders
uint_fast8_t pmTemperature; // is set by the sliders
uint_fast8_t amTemp = 0;    // is set by the sliders
uint_fast8_t pmTemp = 0;    // is set by the sliders
uint_fast8_t amTime;
uint_fast8_t pmTime;
uint_fast8_t Day;
uint_fast8_t Hours;
uint_fast8_t Minutes;
uint_fast8_t seconds;
uint_fast8_t amHours;
uint_fast8_t amMinutes;
uint_fast8_t pmHours;
uint_fast8_t pmMinutes;
bool Am;
bool AmFlag;

/*************************
 *    Time Variables     *
 *       end             *
 ************************/

/*************************
 *   ACS712 Variables    *
 *       start           *
 ************************/
const int ACS712_PIN = 34;               // Analog pin
const float VREF = 3.3;                  // Reference voltage
const int ADC_RES = 4095;                // 12-bit ADC
const float ZERO_CURRENT_VOLTAGE = 2.39; // Midpoint when no current
const float SENSITIVITY = 0.185;         // Volts per amp (ACS712-5A = 185mV/A)
const float CURRENT_THRESHOLD = 0.2;     // Amps — adjust for your system

float lastVoltage;
/*************************
 *   ACS712 Variables    *
 *       end             *
 ************************/

/*************************
 *   global Variables   *
 *       start           *
 ************************/

bool heaterStatus = false;
bool StartUp = 1; // Set to 1 for testing

bool firstrun = true;
bool pendingDateStorage = false; // Flag to track if date storage is pending
int pendingTargetDay = 0;        // Store the target day for retry
int pendingTestMonth = 0;        // Store the test month for retry

char sensVal[50];
/*************************
 *   global Variables   *
 *       end             *
 ************************/

/*************************
 *   Firebase Variables  *
 *       start           *
 ************************/
#define API_KEY "AIzaSyDkJinA0K6NqBLGR4KYnX8AdDNgXp2-FDI"
#define DATABASE_URL "https://esp32-heater-controler-6d11f-default-rtdb.europe-west1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
/*************************
 *   Firebase Variables  *
 *       end             *
 ************************/
/**************************************************
 *   Setup a oneWire instance to communicate      *
 *  with any OneWire devices (not just Maxim/     *
 *         Dallas temperature ICs)                *
 **************************************************/

OneWire oneWire(ONE_WIRE_BUS);

/**************************************************
 *   Pass our oneWire reference to Dallas          *
 *  Temperature sensor class                      *
 **************************************************/
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor
DeviceAddress red, blue, green;      // arrays to hold device addresses
/**************************************************
 *   Variables to hold the temperature readings    *
 *         and storeCount for MQTT                 *
 *                   start                         *
 **************************************************/
int redTemp;
int greenTemp;
int blueTemp;
float prevRedTemp;
float prevGreenTemp;
float prevBlueTemp;

int storeCount;
int dateCount;
/**************************************************
 *   Variables to hold the temperature readings    *
 *               and storeCount For MQTT           *
 *                   end                           *
 **************************************************/

/**************************************************
 *   Function to check if it is daylight saving   *
 *                 time or not                    *
 *                   start                        *
 **************************************************/
WiFiUDP ntpUDP;
const int UTC_OFFSET_STANDARD = 0 * 3600;
const int UTC_OFFSET_DST = 1 * 3600;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_STANDARD, 60000);

bool isDST(int day, int month, int hour)
{
  if (month < 3 || month > 10)
    return false;
  if (month > 3 && month < 10)
    return true;

  int lastSunday = (31 - (5 + year() * 5 / 4) % 7);
  if (month == 3)
    return (day >= lastSunday);
  if (month == 10)
    return (day < lastSunday);

  return false;
}
/**************************************************
 *   Function to check if it is daylight saving   *
 *                 time or not                    *
 *                  end                           *
 **************************************************/

void setup()
{
  Serial.begin(115200); // Start the Serial communication at 9600 bps
  /************************************
   *  Initialize the LED pin as an    *
   * output and turn it off           *
   *          start                   *
   ***********************************/

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Relay_Pin, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(ACS712_PIN, INPUT);    // Set the ACS712 pin as input
  digitalWrite(ACS712_PIN, LOW); // Set the ACS712 pin to low
  /************************************
   *  Initialize the LED pin as an    *
   * output and turn it off           *
   *          end                     *
   ***********************************/
  /**************************************
   *      Initialize the ACS712         *
   *             start                  *
   * ************************************/

  analogReadResolution(12);       // ESP32 default is 12-bit
  analogSetAttenuation(ADC_11db); // Makes full-scale voltage ~3.3V

  /************************************
   *      Initialize the ACS712       *
   *            end                   *
   ************************************/

  Serial.begin(115200);
  delay(500);

  ConnectToWiFi(); // call the function to connect to WiFi

  config.api_key = API_KEY;              // set the API key
  config.database_url = DATABASE_URL;    // set the database URL
  config.timeout.serverResponse = 15000; // Set server response timeout to 10 seconds

  connectToFirebase(); // call the function to connect to Firebase

  timeClient.begin();  // start the timeClient
  timeClient.update(); // update the timeClient
  setSystemTimeFromNTP(); // Set the system time from NTP
  espClient.setInsecure();                  // set the client to be insecure
  client.setServer(mqtt_server, mqtt_port); // set the server and port for the client
  client.setCallback(callback);             // set the callback function for the client

  reconnect();
  // retrieveTime();      // retrieve the time from Firebase

  sensors.begin(); // start the sensors

  /**************************************
   * Get the addresses of the sensors  *
   *           start                   *
   *************************************/

  if (!sensors.getAddress(red, 0))
  {
    Serial.println("red sensor not found.");
  }
  else
  {
    Serial.println("red sensor detected.");
  }
  if (!sensors.getAddress(green, 1))
  {
    Serial.println("green sensor not found.");
  }
  else
  {
    Serial.println("green sensor detected.");
  }
  if (!sensors.getAddress(blue, 2))
  {
    Serial.println("blue sensor not found.");
  }
  else
  {
    Serial.println("blue sensor detected.");
  }
  /**************************************
   * Get the addresses of the sensors  *
   *           end                     *
   *************************************/

  retrieveStoreCount();     // retrieve the storeCount from Firebase
  retrieveDateCount();      // retrieve the dateCount from Firebase
  retrieveLastStoredData(); // retrieve the last stored data from Firebase

  /***************************
   *    Signal strength      *
   *        start            *
   ***************************/

  long rssi = WiFi.RSSI();
  DEBUG_PRINT_RSSI("Signal strength (RSSI): ");
  DEBUG_PRINT_RSSI(rssi);
  DEBUG_PRINT_RSSI(" dBm");
  /***************************
   *    Signal strength      *
   *        end              *
   ***************************/

  Serial.println("Setup complete.");
}
/***************************************
 *      function setup                 *
 *          end                       *
 **************************************/

/*******************************
 *      function loop          *
 *          start              *
 * *****************************/

void loop()
{
  getTime(); // call the function to get the time
  upDateSensors();
  setTargetTemperature();
  relay_Control();
  if (firstrun == true)
  {
    getTime(); // call the function to get the time
    firstrun = false;
  }
  checkCurrent();
  /***************************************
   * Check if the client is connected    *
   *       to the MQTT broker            *
   *            start                    *
   * *************************************/
  if (!client.connected())
  {
    digitalWrite(LED_BUILTIN, LOW);
    reconnect();
  }
  /***************************************
   * Check if the client is connected    *
   *       to the MQTT broker            *
   *            end                      *
   * *************************************/

  client.loop();
}
/***************************************
 *      function loop                  *
 *          end                        *
 **************************************/

/***************************************
 *    function to connect to WiFi      *
 *            start                    *
 **************************************/
void ConnectToWiFi()
{
  presentTime = millis();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    client.publish("wemos/status", "online", false);
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  long rssi = WiFi.RSSI();
  DEBUG_PRINT_RSSI("Signal strength (RSSI): ");
  DEBUG_PRINT_RSSI(rssi);
  DEBUG_PRINT_RSSI(" dBm");
}

/***************************************
 *    function to connect to WiFi      *
 *            end                      *
 **************************************/

/***************************************
 *   function to get the time          *
 *           start                     *
 ***************************************/
void getTime()
{
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();

  currentDay = day(epochTime);
  currentMonth = month(epochTime);

  int currentYear = year(epochTime);
  Hours = hour(epochTime);
  Minutes = minute(epochTime);
  int currentSecond = second(epochTime);
  int offset = isDST(currentDay, currentMonth, Hours) ? UTC_OFFSET_DST : UTC_OFFSET_STANDARD;
  timeClient.setTimeOffset(offset);
  // Print the complete date and time
  // Serial.printf("Date and Time: %02d-%02d-%04d %02d:%02d:%02d\n", currentDay, currentMonth, currentYear, Hours, Minutes, currentSecond);
  delay(1000);

  if (currentDay == nextDay)
  {
    if (currentMonth == 2) // February
    {
      Serial.println("February");
      // Check for leap year
      if ((year() % 4 == 0 && year() % 100 != 0) || (year() % 400 == 0)) // Leap year
      {
        if (currentDay > 28)
        {
          Serial.println("line 594");
          // publishTimeToMQTT();
          storeDateToFirebase();
          asBeenSaved = true;
          nextDay = 1; // Reset nextDay to 1 for the next month
        }
      }
      else // Non-leap year
      {
        if (currentDay > 27)
        {
          Serial.println("line 607");
          // publishTimeToMQTT();
          storeDateToFirebase();
          asBeenSaved = true;
          nextDay = 1; // Reset nextDay to 1 for the next month
          Serial.println("Non-leap year February");
        }
      }
    }
    else if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11) // Months with 30 days
    {
      if (currentDay > 29)
      {
        Serial.println("April, June, September, November");
        Serial.println("line 623");
        // publishTimeToMQTT();
        storeDateToFirebase();
        asBeenSaved = true;
        nextDay = 1; // Reset nextDay to 1 for the next month
      }
    }
    else // Months with 31 days
    {
      if (currentDay > 30)
      {
        Serial.println("All other months have 31 days");
        Serial.println("line 637");
        // publishTimeToMQTT();
        storeDateToFirebase();
        asBeenSaved = true;
        nextDay = 1; // Reset nextDay to 1 for the next month
      }
    }

    // Reset to January if the month exceeds December
    if (currentMonth > 12)
    {
      Serial.println("Resetting month to January");
      currentMonth = 1;
    }
    if (asBeenSaved == false)
    {
      // Store the date in Firebase
      storeDateToFirebase();
      // Publish the time to MQTT
      Serial.println("line 658");
      // publishTimeToMQTT();
      asBeenSaved = true;
      nextDay++; // Increment nextDay for the next day
    }
  }
}

/***************************************
 *   function to get the time          *
 *           end                       *
 ***************************************/

/***************************************
 *  function to handle the callback   *
 *           start                     *
 ***************************************/

void callback(char *topic, byte *payload, unsigned int length)
{

  // Copy payload to a local buffer and null-terminate
  char buf[128];
  size_t copyLen = (length < sizeof(buf) - 1) ? length : sizeof(buf) - 1;
  memcpy(buf, payload, copyLen);
  buf[copyLen] = '\0';

  String message(buf);
  Serial.println(message);

  // Check if the message is for the storeCount topic
  if (String(topic) == storeCount_topic)
  {
    storeCount = message.toInt(); // Convert the message to an integer and update storeCount
  }

  if (strstr(topic, "amTemperature"))
  {
    sscanf(buf, "%d", &amTemperature);
    String amTimeStr = String(buf); // Declare and assign amTimeStr from buf
    DEBUG_PRINT_AMTIME("Retrieved AMtime: ");
    DEBUG_PRINTLN_AMTIME(amTemperature);
    DEBUG_PRINT_AMTIME("Parsed amHours: ");
    DEBUG_PRINTLN_AMTIME(amHours);
    DEBUG_PRINT_AMTIME("Parsed amMinutes: ");
    DEBUG_PRINTLN_AMTIME(amMinutes);
    if (StartUp == 1)
    {
      amTemp = amTemperature;
    }
  }
  if (strstr(topic, "pmTemperature"))
  {
    sscanf(buf, "%d", &pmTemperature);
    DEBUG_PRINT_PMTIME("Retrieved AMtime: ");
    DEBUG_PRINTLN_PMTIME(pmTemperature);
    DEBUG_PRINT_PMTIME("Parsed amHours: ");
    DEBUG_PRINTLN_PMTIME(pmHours);
    DEBUG_PRINT_PMTIME("Parsed amMinutes: ");
    DEBUG_PRINTLN_PMTIME(pmMinutes);
    if (StartUp == 1)
    {
      pmTemp = pmTemperature;
    }
  }

  if (strstr(topic, "AMtime"))
  {
    sscanf(buf, "%d:%d", &amHours, &amMinutes);
  }

  if (strstr(topic, "PMtime"))
  {
    sscanf(buf, "%d:%d", &pmHours, &pmMinutes);
  }
  if (amTemp != 0 && pmTemp != 0)
  {
    StartUp = 0;
  }
}

/***************************************
 *  function to handle the callback   *
 *           end                       *
 ***************************************/

/***************************************
 * function to reconnect to MQTT       *
 *              start                  *
 ***************************************/
void reconnect()
{
  int MQTTretryCount = 0; // Add a retry counter
  while (!client.connected())
  {
    if (client.connect("wemosClient", mqtt_user, mqtt_password, "wemos/status", 0, true, "offline"))
    {
      Serial.println("Connected to MQTT broker!");
      // Publish 'online' status with retain flag
      client.publish("wemos/status", "online", true);
      digitalWrite(LED_BUILTIN, HIGH);
      client.subscribe(red_topic);
      client.subscribe(green_topic);
      client.subscribe(blue_topic);
      client.subscribe("Temp_Control/sub");
      client.subscribe("control");
      client.subscribe("amTemperature");
      client.subscribe("pmTemperature");
      client.subscribe("AMtime");
      client.subscribe("PMtime");
      client.subscribe("HeaterStatus");

      break; // Exit the loop if connected successfully
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      digitalWrite(LED_BUILTIN, LOW);
      MQTTretryCount++;
      if (MQTTretryCount >= 5)
      {
        Serial.println("Max retry count reached. Restarting ESP32...");
        ESP.restart();        // Restart the ESP32 after 5 failed attempts
        retrieveStoreCount(); // retrieve the storeCount from Firebase
        retrieveDateCount();  // retrieve the dateCount from Firebase
        // retrieveTime();      // retrieve the time from Firebase
      }
      delay(5000);
    }
  }
}

/***************************************
 *   function to publish time to MQTT  *
 *               start                 *
 ***************************************/

void publishTimeToMQTT()
{
  char timeStr[50];
  timeClient.update();
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());

  // Extract hour and minute values
  int Hours = timeClient.getHours();
  int Minutes = timeClient.getMinutes();
  String hourStr = String(Hours);
  String minuteStr = String(Minutes);
  client.publish(time_topic, timeStr, true);
  client.publish(hour_topic, hourStr.c_str());
  client.publish(minute_topic, minuteStr.c_str());
}
/***************************************
 *   function to publish time to MQTT  *
 *               end                   *
 ***************************************/

/***************************************
 *    and publish the temperature       *
 *           and storeCount             *
 *              start                   *
 * *************************************/
void publishSensorValues()
{
  String redStr = String(redTemp);
  String greenStr = String(greenTemp);
  String blueStr = String(blueTemp);
  String storeCountStr = String(storeCount);

  client.publish(red_topic, redStr.c_str());
  client.publish(green_topic, greenStr.c_str());
  client.publish(blue_topic, blueStr.c_str());
  client.publish(storeCount_topic, storeCountStr.c_str());
  delay(5000);
}

/***************************************
 *   function to publish to MQTT       *
 *      and publish the temperature    *
 *           and storeCount            *
 *               end                   *
 ***************************************/

/***************************************
 *  function to store data to Firebase *
 *           start                     *
 ***************************************/
void storeDataToFirebase()
{
  // Increment and reset storeCount if it exceeds 100
  storeCount++;
  if (storeCount > 100)
  {
    storeCount = 1;
  }

  // Construct Firebase paths
  char sensorPath[50];
  snprintf(sensorPath, sizeof(sensorPath), "/sensorData/%d", storeCount);

  char redPath[60], greenPath[60], bluePath[60], hourPath[60], minutePath[60];
  snprintf(redPath, sizeof(redPath), "%s/red", sensorPath);
  snprintf(greenPath, sizeof(greenPath), "%s/green", sensorPath);
  snprintf(bluePath, sizeof(bluePath), "%s/blue", sensorPath);
  snprintf(hourPath, sizeof(hourPath), "%s/hour", sensorPath);
  snprintf(minutePath, sizeof(minutePath), "%s/minute", sensorPath);

  // Retry logic
  int retryCount = 0;
  const int maxRetries = 3;
  const int retryDelay = 1000; // 1 second delay between retries

  while (retryCount < maxRetries)
  {
    // Attempt to store data in Firebase
    bool success = Firebase.RTDB.setFloat(&fbdo, redPath, redTemp) &&
                   Firebase.RTDB.setFloat(&fbdo, greenPath, greenTemp) &&
                   Firebase.RTDB.setFloat(&fbdo, bluePath, blueTemp) &&
                   Firebase.RTDB.setInt(&fbdo, hourPath, Hours) &&
                   Firebase.RTDB.setInt(&fbdo, minutePath, Minutes);

    if (success)
    {
      // Update storeCount in Firebase
      Firebase.RTDB.setInt(&fbdo, "/storeCount", storeCount);
      Serial.println("Data successfully stored in Firebase.");
      break; // Exit the loop if successful
    }
    else
    {
      // Log the error and retry
      Serial.println("Failed to store data to Firebase, retrying...");
      Serial.printf("Error: %s\n", fbdo.errorReason().c_str());
      retryCount++;

      if (retryCount >= maxRetries)
      {
        Serial.println("Max retries reached. Attempting to reconnect to Firebase...");
        connectToFirebase(); // Attempt to reconnect to Firebase
        break;               // Exit the loop after max retries
      }

      delay(retryDelay); // Wait before retrying
    }
  }

  // Check available memory
  checkMemory();
}

/***************************************
 * this function will need changing    *
 * to retrieve the last stored         *
 * Target temperature values           *
 * and times to change the target      *
 * temperature values                  *
 ***************************************/
void retrieveLastStoredData()
{
  if (Firebase.RTDB.getString(&fbdo, "/setTime/F_B_AMtime"))
  {
    String amTimeStr = fbdo.stringData();
    if (sscanf(amTimeStr.c_str(), "%d:%d", &amHours, &amMinutes) == 2) // Ensure both values are parsed
    {
      DEBUG_PRINT_AMTIME_RETRIEVE("Retrieved AMtime: ");
      DEBUG_PRINTLN_AMTIME_RETRIEVE(amTemperature);
      DEBUG_PRINT_AMTIME_RETRIEVE("Parsed amHours: ");
      DEBUG_PRINTLN_AMTIME_RETRIEVE(amHours);
      DEBUG_PRINT_AMTIME_RETRIEVE("Parsed amMinutes: ");
      DEBUG_PRINTLN_AMTIME_RETRIEVE(amMinutes);
    }
    else
    {
      Serial.println("Failed to parse AMtime.");
    }
  }
  else
  {
    Serial.println("Failed to retrieve AMtime from Firebase");
    Serial.println(fbdo.errorReason());
  }
  // Retrieve pmTime
  Serial.println("Retrieving pmTime from Firebase...");

  if (Firebase.RTDB.getString(&fbdo, "/setTime/F_B_PMtime"))
  {
    String pmTimeStr = fbdo.stringData();
    if (sscanf(pmTimeStr.c_str(), "%d:%d", &pmHours, &pmMinutes) == 2) // Ensure both values are parsed
    {
      DEBUG_PRINT_PMTIME_RETRIEVE("Retrieved AMtime: ");
      DEBUG_PRINTLN_PMTIME_RETRIEVE(pmTemperature);
      DEBUG_PRINT_PMTIME_RETRIEVE("Parsed amHours: ");
      DEBUG_PRINTLN_PMTIME_RETRIEVE(pmHours);
      DEBUG_PRINT_PMTIME_RETRIEVE("Parsed amMinutes: ");
      DEBUG_PRINTLN_PMTIME_RETRIEVE(pmMinutes);
    }
    else
    {
      Serial.println("Failed to parse PMtime.");
    }
  }
  else
  {
    Serial.println("Failed to retrieve PMtime from Firebase");
    Serial.println(fbdo.errorReason());
  }
  // Retrieve amTemperature

  if (Firebase.RTDB.getString(&fbdo, "/setTime/F_B_amTemperature"))
  {
    amTemperature = fbdo.stringData().toInt();
    DEBUG_PRINT_PMTIME("pmTemperature ");
    DEBUG_PRINT_PMTIME(pmTemperature);
    DEBUG_PRINTLN_PMTIME(" c ");
  }
  else
  {
    Serial.println("Failed to retrieve pmTemperature from Firebase");
    Serial.println(fbdo.errorReason());
  }

  // Retrieve pmTemperature

  if (Firebase.RTDB.getString(&fbdo, "/setTime/F_B_pmTemperature"))
  {
    pmTemperature = fbdo.stringData().toInt();
    Serial.print("Retrieved pmTemperature: ");
    Serial.println(pmTemperature);
  }
  else
  {
    Serial.println("Failed to retrieve pmTemperature from Firebase");
    Serial.println(fbdo.errorReason());
  }

  // Retrieve the last stored data from Firebase
  Serial.println("*******Retrieving last stored data from Firebase");
  String path = "/sensorData/";
  path.concat(String(storeCount));

  if (Firebase.RTDB.getJSON(&fbdo, path))
  {
    FirebaseJson &json = fbdo.jsonObject();
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println("Retrieved last stored data from Firebase:");
    Serial.println(jsonStr);

    FirebaseJsonData jsonData;
    if (json.get(jsonData, "red") && jsonData.type == "int")
    {
      redTemp = jsonData.intValue;
    }
    if (json.get(jsonData, "green") && jsonData.type == "int")
    {
      greenTemp = jsonData.intValue;
    }
    if (json.get(jsonData, "blue") && jsonData.type == "int")
    {
      blueTemp = jsonData.intValue;
    }
    DEBUG_PRINT_EXTRACTED_SENSOR_READINGS("********** Extracted redTemp: ");
    DEBUG_PRINTLN_EXTRACTED_SENSOR_READINGS(redTemp);
    DEBUG_PRINT_EXTRACTED_SENSOR_READINGS("Extracted greenTemp: ");
    DEBUG_PRINTLN_EXTRACTED_SENSOR_READINGS(greenTemp);
    DEBUG_PRINT_EXTRACTED_SENSOR_READINGS("Extracted blueTemp: ");
    DEBUG_PRINTLN_EXTRACTED_SENSOR_READINGS(blueTemp);

    // Clear the JSON object to free memory
    json.clear();
  }
  else
  {
    Serial.println("Failed to retrieve last stored data from Firebase");
    Serial.println(fbdo.errorReason());
  }

  // Retrieve dateCount
  Serial.println("Retrieving dateCount from Firebase...");
  if (Firebase.RTDB.getInt(&fbdo, "/dateCount"))
  {
    dateCount = fbdo.intData();

    DEBUG_PRINT_RECIEVED_DATE_COUNT("Received dateCount: ");
    DEBUG_PRINTLN_RECIEVED_DATE_COUNT(dateCount);

    // Check if dateCount is less than 1
  }
  else
  {
    Serial.println("Failed to retrieve dateCount from Firebase");
    Serial.println(fbdo.errorReason());
  }

  // Retrieve storeCount
  Serial.println("Retrieving dateCount from Firebase...");
  if (Firebase.RTDB.getInt(&fbdo, "/storeCount"))
  {
    storeCount = fbdo.intData();
  }
  else
  {
    Serial.println("Failed to retrieve storeCount from Firebase");
    Serial.println(fbdo.errorReason());
  }
}

/***************************************
 * function to retrieve the last stored*
 * data from Firebase on startup       *
 *           end                       *
 ***************************************/

/***************************************
 * function to check the memory        *
 *           start                     *
 ***************************************/
void checkMemory()
{
  Serial.print("Free heap memory: ");
  Serial.println(ESP.getFreeHeap());
}
/*****************************************
 * function to check the memory          *
 *           end                         *
 *****************************************/

/************************************
 *  Attempt to sign up to Firebase  *
 *          start                   *
 ***********************************/
void connectToFirebase()
{
  int DatdBaseretryCount = 0;
  const int maxRetries = 5; // Maximum number of retries
  while (!Firebase.signUp(&config, &auth, "", ""))
  {

    DatdBaseretryCount++;

    DEBUG_PRINT_CONNECT_TO_FIREBASE("Attempting to sign up to Firebase...");
    DEBUG_PRINTLN_CONNECT_TO_FIREBASE("Firebase sign-up failed: ");
    DEBUG_PRINTLN_CONNECT_TO_FIREBASE(config.signer.signupError.message.c_str());
    DEBUG_PRINT_CONNECT_TO_FIREBASE("Error code: ");
    DEBUG_PRINTLN_CONNECT_TO_FIREBASE(config.signer.signupError.code);
    DEBUG_PRINT_CONNECT_TO_FIREBASE("Retrying in 30 seconds...");
    DEBUG_PRINT_CONNECT_TO_FIREBASE("Retry count: ");
    DEBUG_PRINTLN_CONNECT_TO_FIREBASE(DatdBaseretryCount);

    if (DatdBaseretryCount >= maxRetries)
    {
      Serial.println("Max retry count reached. Restarting ESP32...");
      ESP.restart();        // Restart the ESP32 after max retries
      retrieveStoreCount(); // retrieve the storeCount from Firebase
      retrieveDateCount();  // retrieve the dateCount from Firebase
      // retrieveTime();      // retrieve the time from Firebase
      break; // Exit the loop after max retries
    }
    delay(30000); // Wait for 30 seconds before retrying
  }
  Serial.println("Firebase sign-up successful. ok");
  signupOK = true;
  // set the token status callback
  config.token_status_callback = tokenStatusCallback;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Check if there is pending date storage
  if (pendingDateStorage)
  {
    Serial.println("Retrying pending date storage...");
    String datePath = "/dateData/";
    datePath.concat(String(dateCount)); // Change to dateStoreCount
    String dayPath = datePath;
    dayPath.concat("/day");
    String monthPath = datePath;
    monthPath.concat("/month");

    bool success = Firebase.RTDB.setInt(&fbdo, dayPath, pendingTargetDay) &&
                   Firebase.RTDB.setInt(&fbdo, monthPath, pendingTestMonth);

    if (success)
    {
      Serial.println("Pending date successfully stored in Firebase.");
      pendingDateStorage = false; // Reset pending flag
    }
    else
    {
      Serial.println("Failed to store pending date in Firebase.");
      Serial.println(fbdo.errorReason());
      pendingDateStorage = true; // Keep pending flag
    }
  }
}
/************************************
 *  Attempt to sign up to Firebase  *
 *          end                     *
 ***********************************/

/************************************
 *     function to store date       *
 *          in Firebase             *
 *           start                  *
 ***********************************/

void storeDateToFirebase()
{

  // Increment dateCount
  dateCount++;

  // Construct Firebase paths
  String datePath = "/dateData/";
  datePath.concat(String(dateCount)); // Change to dateStoreCount

  String dayPath = datePath;
  dayPath.concat("/day");
  String monthPath = datePath;
  monthPath.concat("/month");

  DEBUG_PRINT_DATE_TO_FIREBASE("currentMonth: ");
  DEBUG_PRINTLN_DATE_TO_FIREBASE(currentMonth);
  DEBUG_PRINT_DATE_TO_FIREBASE("currentDay: ");
  DEBUG_PRINTLN_DATE_TO_FIREBASE(currentDay);
  // Store the current date in Firebase
  // Serial.print("Storing date in Firebase at path: ");
  bool success = Firebase.RTDB.setInt(&fbdo, dayPath, currentDay) && Firebase.RTDB.setInt(&fbdo, monthPath, currentMonth);

  if (success)
  {
    Serial.println("Date successfully stored in Firebase.");
    pendingDateStorage = false; // Reset pending flag
  }
  else
  {
    Serial.println("Failed to store date in Firebase.");
    Serial.print("Error: ");
    Serial.println(fbdo.errorReason());

    // Mark date storage as pending
    pendingDateStorage = true;
    pendingTargetDay = currentDay;
    pendingTestMonth = currentMonth;

    // Attempt to reconnect to Firebase
    connectToFirebase();
  }
}

/******************************************
 * Retrieve the storeCount from Firebase  *
 *            start                       *
 ******************************************/
void retrieveStoreCount()
{
  if (Firebase.RTDB.getInt(&fbdo, "/storeCount"))
  {
    storeCount = fbdo.intData();
    Serial.print("Retrieved storeCount from Firebase: ");
  }
  else
  {
    Serial.println("Failed to retrieve storeCount from Firebase");
    Serial.println(fbdo.errorReason());
  }
}
/******************************************
 * Retrieve the storeCount from Firebase  *
 *            end                         *
 ******************************************/

/******************************************
 * Retrieve the dateCount from Firebase  *
 *            start                       *
 ******************************************/
void retrieveDateCount()
{

  if (Firebase.RTDB.getInt(&fbdo, "/dateCount"))
  {
    dateCount = fbdo.intData();
    Serial.print("Retrieved dateCount from Firebase: ");
    // Serial.println(dateCount);
  }
  else
  {
    Serial.println("Failed to retrieve dateCount from Firebase");
    Serial.println(fbdo.errorReason());
  }
}

/***************************************
 *  function to update the sensors    *
 *           start                     *
 ***************************************/

void upDateSensors()
{
  sensors.requestTemperatures();
  // Serial.println("DONE");

  redTemp = static_cast<int>(sensors.getTempC(red));     // get the temperature for the red sensor
  greenTemp = static_cast<int>(sensors.getTempC(green)); // get the temperature for the green sensor
  blueTemp = static_cast<int>(sensors.getTempC(blue));   //  get the temperature for the blue sensor

  DEBUG_PRINT_SENSOR_READINGS("Red temperature: ");
  DEBUG_PRINT_SENSOR_READINGS(redTemp);
  DEBUG_PRINTLN_SENSOR_READINGS(" C");
  DEBUG_PRINT_SENSOR_READINGS("Green temperature: ");
  DEBUG_PRINT_SENSOR_READINGS(greenTemp);
  DEBUG_PRINTLN_SENSOR_READINGS(" C");
  DEBUG_PRINT_SENSOR_READINGS("Blue temperature: ");
  DEBUG_PRINTLN_SENSOR_READINGS(blueTemp);
  DEBUG_PRINTLN_SENSOR_READINGS(" C");
  /*********************************************
   *   Print the temperature readings to the   *
   *   serial monitor for debugging purposes   *
   *                  start                    *
   * ******************************************/

  // Serial.print("storeCount: ");
  // Serial.println(storeCount);
  /*********************************************
   *  Print the temperature readings to the   *
   * serial monitor for debugging purposes    *
   *                 end                      *
   * ******************************************/

  /*********************************************
   * Check if the temperature readings have    *
   * changed and publish to MQTT and store     *
   * to Firebase if they have changed          *
   *                 start                     *
   * ******************************************/
  if (redTemp != prevRedTemp || greenTemp != prevGreenTemp || blueTemp != prevBlueTemp)
  {

    DEBUG_PRINT_SENSOR_CHANGE("Temperature change detected, publishing to MQTT...");
    DEBUG_PRINTLN_SENSOR_CHANGE("amHours: ");
    DEBUG_PRINTLN_SENSOR_CHANGE(amHours);
    DEBUG_PRINT_SENSOR_CHANGE("amMinutes: ");
    DEBUG_PRINTLN_SENSOR_CHANGE(amMinutes);
    DEBUG_PRINT_SENSOR_CHANGE("Hours: ");
    DEBUG_PRINTLN_SENSOR_CHANGE(Hours);
    DEBUG_PRINT_SENSOR_CHANGE("pmHours: ");
    DEBUG_PRINTLN_SENSOR_CHANGE(pmHours);
    DEBUG_PRINT_SENSOR_CHANGE("pmMinutes: ");
    DEBUG_PRINTLN_SENSOR_CHANGE(pmMinutes);
    DEBUG_PRINTLN_SENSOR_CHANGE("...... ");

    publishSensorValues();
    publishTimeToMQTT();
    storeDataToFirebase();
    prevRedTemp = redTemp;
    prevGreenTemp = greenTemp;
    prevBlueTemp = blueTemp;
    // client.publish("wemos/status", "online", true);
  }
}
/***************************************
 *  function to update the sensors     *
 *           end                       *
 ***************************************/

/*************************************************************
                           Relay Control
                                start
*************************************************************/

void relay_Control()
{

  int targetTemp = AmFlag ? amTemp : pmTemp;

  DEBUG_PRINT_RELAY_CONTROL("targetTemp: ");
  DEBUG_PRINTLN_RELAY_CONTROL(targetTemp);
  DEBUG_PRINT_RELAY_CONTROL("redTemp: ");
  DEBUG_PRINTLN_RELAY_CONTROL(redTemp);

  if (redTemp < targetTemp)
  {
    DEBUG_PRINT_RELAY_CONTROL("Relay on");
    DEBUG_PRINT_RELAY_CONTROL("redTemp: ");
    DEBUG_PRINTLN_RELAY_CONTROL(redTemp);
    DEBUG_PRINT_RELAY_CONTROL("targetTemp: ");
    DEBUG_PRINTLN_RELAY_CONTROL(targetTemp);
    digitalWrite(Relay_Pin, HIGH); // Turn relay ON
    checkCurrent();
    delay(2000); // Wait 2 seconds
    // digitalWrite(Relay_Pin, LOW); // Turn relay OFF
    // delay(2000);
    //   digitalWrite(Relay_Pin, HIGH);
    digitalWrite(LED_Pin, HIGH); // LED_Pin on
    //   digitalWrite(LED_BUILTIN, LOW); // LED_Pin on
    //   heaterStatus = true;
    // if (!heaterOn) {
    //   startHeaterTimer();
    // }
  }
  else if (redTemp > targetTemp)
  {
    DEBUG_PRINT_RELAY_CONTROL("Relay off");
    digitalWrite(Relay_Pin, LOW);
    digitalWrite(LED_Pin, LOW); // LED_Pin off
    // digitalWrite(LED_BUILTIN, HIGH); // LED_Pin off
    heaterStatus = false;
    // heaterOn = false;
  }
}
/*************************************************************
                            Relay Control
                                 End
*************************************************************/

/*************************************************************
                              Heater Control
                                   start
  ************************************************************/
void setTargetTemperature()
{
  if (Am)
  {
    if (amHours == Hours && amMinutes == Minutes)
    { // set amTemp for the Night time setting
      Serial.println("amHours == Hours && amMinutes == Minutes");
      // Serial.println("amHours == Hours && amMinutes == Minutes");
      AmFlag = true;
      amTemp = amTemperature;
      int myTemp = Serial.print("...myTemp: ");
      Serial.print(myTemp);
      ;
      sprintf(sensVal, "%d", myTemp);
      client.publish("targetTemperature", sensVal, true);
      Serial.print("...AM myTemp: ");
      Serial.print(myTemp);
    }
  }
  else
  {
    if (pmHours == Hours && pmMinutes == Minutes)
    { // set pmTemp for the Night time setting
      Serial.println("pmHours == Hours && PmMinutes == Minutes");
      AmFlag = false;
      pmTemp = pmTemperature;
      int myTemp = pmTemp;
      sprintf(sensVal, "%d", myTemp);
      client.publish("targetTemperature", sensVal, true);
      Serial.print("...PM myTemp: ");
      Serial.print(myTemp);
    }
    DEBUG_PRINTLN_AMTIME("Hours minute all =: ");
    DEBUG_PRINT_AMTIME("amHours: ");
    DEBUG_PRINTLN_AMTIME(amHours);

    DEBUG_PRINT_AMTIME("amMinutes: ");
    DEBUG_PRINTLN_AMTIME(amMinutes);

    DEBUG_PRINT_AMTIME("pmHours: ");
    DEBUG_PRINTLN_AMTIME(pmHours);

    DEBUG_PRINT_AMTIME("pmMinutes: ");
    DEBUG_PRINTLN_AMTIME(pmMinutes);
  }
}
/****************************************
 *            Heater Control            *
 *                End                   *
 ***************************************/

/*************************************************************
                              Check Current
                                   start
************************************************************/
void checkCurrent()
{
  const int samples = 200;
  float sumSq = 0;
  for (int i = 0; i < samples; i++)
  {
    int raw = analogRead(ACS712_PIN);
    float voltage = (raw / (float)ADC_RES) * VREF;
    float current = (voltage - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;
    sumSq += current * current;
    lastVoltage = voltage;
    delay(1);
  }
  float rmsCurrent = sqrt(sumSq / samples);

  DEBUG_PRINT_RMS_CURRENT("AC RMS Current: ");
  DEBUG_PRINTLN_RMS_CURRENT(rmsCurrent, 3);
  DEBUG_PRINT_RMS_CURRENT("Measured last sample voltage: ");
  DEBUG_PRINTLN_RMS_CURRENT(lastVoltage, 3);
  // Check if the relay is ON or OFF

  // Print actual relay state
  if (digitalRead(Relay_Pin) == HIGH)
  {
    DEBUG_PRINTLN_RELAY_STATE("✅ Relay is ON  ");
  }
  else
  {
    DEBUG_PRINTLN_RELAY_STATE("  ❌ Relay is OFF");
  }
  Serial.print("CURRENT_THRESHOLD: ");
  Serial.println(CURRENT_THRESHOLD);
  Serial.print("rmsCurrent: ");
  Serial.println(rmsCurrent);
  if (abs(rmsCurrent) < CURRENT_THRESHOLD)
  {

    DEBUG_PRINTLN_RELAY_STATE("  ⚠️ Heater is OFF or not working!  ");
    if (firstRunE_Mail == true)
    {
      firstRunE_Mail = false;
      sendEmail("From ESP32: ", " Heater not working! ");
    }
    presentTime = millis(); // Update presentTime before the check
    if (presentTime - previousTime > interval)
    {
      previousTime = presentTime;
      sendEmail("From ESP32: ", " Heater not working! ");
    }
  }
  else
  {
    DEBUG_PRINTLN_RELAY_STATE("  ✅ Heater is ON and drawing current.  ");
  }
}
/**************************************
 *        Check Current               *
 *            end                     *
 * ************************************/

/*************************************
 * Function to send an email       *
 *            start                 *
 ************************************/
void sendEmail(const String &subject, const String &message)
{
  SMTP_Message msg;

  msg.sender.name = "ESP32";
  msg.sender.email = SENDER_EMAIL;
  msg.subject = subject;
  msg.addRecipient("Recipient", RECIPIENT_EMAIL);

  msg.text.content = message;
  msg.text.charSet = "us-ascii";
  msg.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // Set the SMTP server and login credentials
  smtp.callback([](SMTP_Status status)
                { Serial.println(status.info()); });

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = SENDER_EMAIL;
  session.login.password = SENDER_PASSWORD;
  session.login.user_domain = "";

  if (!smtp.connect(&session))
  {
    Serial.println("Could not connect to mail server");
    return;
  }

  if (!MailClient.sendMail(&smtp, &msg))
  {
    Serial.print("Error sending Email, ");
    Serial.println(smtp.errorReason());
  }
  else
  {
    Serial.println("Email sent successfully!");
  }
  smtp.closeSession();
}
/***************************************
 *  Function to send an email          *
 *            end                      *
 ***************************************/

// Call this after timeClient.update();
void setSystemTimeFromNTP()
{
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  struct timeval tv = {.tv_sec = now};
  settimeofday(&tv, nullptr);
}
