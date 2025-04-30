#include <Arduino.h>
#include <WiFi.h>

/*****************************
 *     TimeLib Library       *
 *         start             *
 ****************************/
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
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
void connecttoFirebase();
void storeDateToFirebase();
void tokenStatusCallback(TokenInfo info);
void relay_Control();
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

#define ONE_WIRE_BUS 23 // Data wire is plugged into port 2 on the Wemos D1 Mini

/*************************
 *   Pin Definitions     *
 *       start           *
 ************************/
#define Relay_Pin D5
#define LED_Pin D6
// #define LED_BUILTIN 2
OneWire ds(D7);

/*************************
 *   Pin Definitions     *
 *       end             *
 ************************/

/*************************
 *    Time Variables     *
 *       start           *
 ************************/
unsigned long presentTime = millis();
unsigned long previousMillis = millis();
unsigned long interval = 5000;
int targetMinutes;
int currentDay;
int currentMonth = 1;
// remove once proven to work
int nextDay = 1; // Set to 2 for testing
// timeCount is for testing only
int timeCount = 0;

uint_fast8_t amTemperature; // is set by the sliders
uint_fast8_t pmTemperature; // is set by the sliders
uint_fast8_t amTemp = 0;    // is set by the sliders
uint_fast8_t pmTemp = 0;    // is set by the sliders
// uint_fast8_t AMtime;
// uint_fast8_t PMtime;
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
 *   global Variables   *
 *       start           *
 ************************/

bool heaterStatus = false;
bool StartUp = 1; // Set to 1 for testing

bool firstrun = true;
bool pendingDateStorage = false; // Flag to track if date storage is pending
int pendingTargetDay = 0;        // Store the target day for retry
int pendingTestMonth = 0;        // Store the test month for retry
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
  /************************************
   *  Initialize the LED pin as an    *
   * output and turn it off           *
   *          start                   *
   ***********************************/
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  /************************************
   *  Initialize the LED pin as an    *
   * output and turn it off           *
   *          end                     *
   ***********************************/

  Serial.begin(115200);
  delay(500);

  ConnectToWiFi(); // call the function to connect to WiFi

  config.api_key = API_KEY;              // set the API key
  config.database_url = DATABASE_URL;    // set the database URL
  config.timeout.serverResponse = 15000; // Set server response timeout to 10 seconds

  connecttoFirebase(); // call the function to connect to Firebase

  timeClient.begin();  // start the timeClient
  timeClient.update(); // update the timeClient

  espClient.setInsecure();                  // set the client to be insecure
  client.setServer(mqtt_server, mqtt_port); // set the server and port for the client
  client.setCallback(callback);             // set the callback function for the client

  reconnect();

  /******************************************
   * Retrieve the storeCount from Firebase  *
   *            start                       *
   ******************************************/

  if (Firebase.RTDB.getInt(&fbdo, "/storeCount"))
  {
    storeCount = fbdo.intData();
    Serial.print("Retrieved storeCount from Firebase: ");
    // Serial.println(storeCount);
  }
  else
  {
    Serial.println("Failed to retrieve storeCount from Firebase");
    Serial.println(fbdo.errorReason());
  }
  /******************************************
   * Retrieve the storeCount from Firebase  *
   *            end                         *
   ******************************************/
  /******************************************
   * Retrieve the dateCount from Firebase  *
   *            start                       *
   ******************************************/

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
  /******************************************
   * Retrieve the dateCount from Firebase  *
   *            end                         *
   ******************************************/
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

  retrieveLastStoredData(); // retrieve the last stored data from Firebase

  /***************************
   *    Signal strength      *
   *        start            *
   ***************************/

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
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
  // publishTimeToMQTT();
  // storeDateToFirebase();
  upDateSensors();
  // Serial.println("...");
  // Serial.print("352 targetMinutes: ");
  // Serial.print(targetMinutes);
  // Serial.print(" Minutes: ");
  // Serial.println(Minutes);
  // Serial.println("...");
  // Serial.print("357 firestrun: ");
  // Serial.println(firstrun);
  // Serial.println("...");
  if (firstrun == true)
  {
    getTime(); // call the function to get the time
    // Serial.println("First run");
    // targetMinutes = Minutes + 1;
    // Serial.print("361 targetMinutes: ");
    // Serial.print(targetMinutes);
    // Serial.print("...Minutes: ");
    // Serial.println(Minutes);
    firstrun = false;
  }
  /******************************
   *    remove only want to     *
   *    publish when sensor     *
   *    values change           *
   *        start               *
   *******************************/

  if (Minutes == targetMinutes)
  {
    Serial.println("Time to publish sensor values");
    // publishTimeToMQTT();
    targetMinutes++;
    if (targetMinutes == 60)
    {
      targetMinutes = 2;
    }
    // Serial.print("372 targetMinutes: ");
    // Serial.print(targetMinutes);
    // Serial.print("...Minutes: ");
    // Serial.println(Minutes);
    // Serial.println("...");
  }
  /******************************
   *    remove only want to     *
   *    publish when sensor     *
   *    values change           *
   *        end                 *
   *******************************/

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
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
  /***************************************
   * Check if the client is connected    *
   *       to the MQTT broker            *
   *            end                      *
   * *************************************/

  client.loop();

  /***************************************
   * Check if the client is connected    *
   *       to the Firebase                *
   *            start                    *
   * *************************************/
  // if (Firebase.ready())
  // {
  //   Serial.println("Firebase is connected and ready.");
  //   // upDateSensors();       // call the function to update the sensors
  //   // storeDataToFirebase(); // call the function to store data to Firebase
  // }
  // else
  // {
  //   Serial.println("Firebase is not connected.");
  //   ESP.restart();
  // }
  /***************************************
   * Check if the client is connected    *
   *       to the Firebase               *
   *            end                      *
   * *************************************/
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
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("*********** Connected to WiFi");
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
  // currrentDay and currentMonth comment out for testing
  // currentDay = day(epochTime);
  // currentMonth = month(epochTime);
  // currentDay and currentMonth comment out for testing
  int currentYear = year(epochTime);
  Hours = hour(epochTime);
  Minutes = minute(epochTime);
  int currentSecond = second(epochTime);
  int offset = isDST(currentDay, currentMonth, Hours) ? UTC_OFFSET_DST : UTC_OFFSET_STANDARD;
  timeClient.setTimeOffset(offset);
  // Print the complete date and time
  // Serial.printf("Date and Time: %02d-%02d-%04d %02d:%02d:%02d\n", currentDay, currentMonth, currentYear, Hours, Minutes, currentSecond);
  delay(1000);

  timeCount++;
  Serial.println("Before currentDay == nextDay");
  Serial.println("..... ");
  Serial.print("currentDay: ");
  Serial.println(currentDay);
  Serial.print("nextDay: ");
  Serial.println(nextDay);
  Serial.print("currentMonth: ");
  Serial.println(currentMonth);
  Serial.print("timeCount: ");
  Serial.println(timeCount);
  Serial.println(("..... "));
  if (timeCount == 10)
  {
    timeCount = 0;
    currentDay++; // Increment currentDay for testing
  }
  int asBeenSaved = false;
  // Check if nextDay matches currentDay
  Serial.println("Checking if nextDay matches currentDay");
  Serial.println("..... ");
  Serial.print("currentDay: ");
  Serial.println(currentDay);
  Serial.print("nextDay: ");
  Serial.println(nextDay);
  Serial.print("currentMonth: ");
  Serial.println(currentMonth);
  Serial.print("timeCount: ");
  Serial.println(timeCount);
  Serial.println(("..... "));
  if (currentDay == nextDay)
  {
    Serial.println("nextDay matches currentDay, incrementing dateCount");
    Serial.println("..... ");
    Serial.print("currentDay: ");
    Serial.println(currentDay);
    Serial.print("nextDay: ");
    Serial.println(nextDay);
    Serial.print("currentMonth: ");
    Serial.println(currentMonth);
    Serial.print("timeCount: ");
    Serial.println(timeCount);
    Serial.println(("..... "));
    // Check if nextDay exceeds the maximum possible days in the current month
    if (currentMonth == 2) // February
    {
      Serial.println("February");
      // Check for leap year
      if ((year() % 4 == 0 && year() % 100 != 0) || (year() % 400 == 0)) // Leap year
      {
        if (currentDay > 28)
        {
          publishTimeToMQTT();
          storeDateToFirebase();
          asBeenSaved = true;
          currentDay = 1;
          nextDay = 1;
          currentMonth++;
        }
      }
      else // Non-leap year
      {
        if (currentDay > 27)
        {
          publishTimeToMQTT();
          storeDateToFirebase();
          asBeenSaved = true;
          currentDay = 1;
          nextDay = 1;
          currentMonth++;
          Serial.println("Non-leap year February");
        }
      }
    }
    else if (currentMonth == 4 || currentMonth == 6 || currentMonth == 9 || currentMonth == 11) // Months with 30 days
    {
      if (currentDay > 29)
      {
        Serial.println("April, June, September, November");
        publishTimeToMQTT();
        storeDateToFirebase();
        asBeenSaved = true;
        currentDay = 1;
        nextDay = 1;
        currentMonth++;
      }
    }
    else // Months with 31 days
    {
      if (currentDay > 30)
      {
        Serial.println("All other months have 31 days");
        publishTimeToMQTT();
        storeDateToFirebase();
        asBeenSaved = true;
        currentDay = 1;
        nextDay = 1;
        currentMonth++;
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
      publishTimeToMQTT();
    }
    // Increment nextDay for the next iteration
    nextDay++;
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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is for the storeCount topic
  if (String(topic) == storeCount_topic)
  {
    storeCount = message.toInt(); // Convert the message to an integer and update storeCount
    Serial.print("......");
    Serial.print("Updated storeCount: ");
    Serial.println(storeCount);
    Serial.print("....... ");
  }
  payload[length] = '\0';

  if (strstr(topic, "amTemperature"))
  {
    sscanf((char *)payload, "%d", &amTemperature);
    Serial.print("......");
    Serial.print("amTemperature: ");
    Serial.println(amTemperature);
    Serial.print("......");
    if (StartUp == 1)
    {
      amTemp = amTemperature;
    }
  }
  if (strstr(topic, "pmTemperature"))
  {
    sscanf((char *)payload, "%d", &pmTemperature);
    Serial.print("......");
    Serial.print("pmTemperature: ");
    Serial.println(pmTemperature);
    Serial.print("......");
    if (StartUp == 1)
    {
      pmTemp = pmTemperature;
    }
  }

  if (strstr(topic, "AMtime"))
  {
    sscanf((char *)payload, "%d:%d", &amHours, &amMinutes);
    Serial.print("......");
    Serial.print("amHours: ");
    Serial.println(amHours);
    Serial.print("......");
  }

  if (strstr(topic, "PMtime"))
  {
    sscanf((char *)payload, "%d:%d", &pmHours, &pmMinutes);
    Serial.print("......");
    Serial.print("pmHours: ");
    Serial.println(pmHours);
    Serial.print("......");
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
  Serial.println("**** Starting Reconnecting to MQTT broker...");
  int MQTTretryCount = 0; // Add a retry counter
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");
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
        ESP.restart(); // Restart the ESP32 after 5 failed attempts
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
  Serial.println("Publishing time to MQTT");
  char timeStr[50];
  timeClient.update();
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());

  // Extract hour and minute values
  int Hours = timeClient.getHours();
  int Minutes = timeClient.getMinutes();
  String hourStr = String(Hours);
  String minuteStr = String(Minutes);
  // Serial.print("Publishing temperature start: ");
  // Serial.println(".. ");
  // Serial.println(".. ");
  // Serial.println(".. ");
  // Serial.println(redStr);
  // Serial.println(greenStr);
  // Serial.println(blueStr);
  // Serial.println(storeCountStr);
  // Serial.println(".. ");
  // Serial.println(".. ");
  // Serial.println(".. ");
  // Serial.println("Publishing temperature end: ");

  client.publish(time_topic, timeStr, true);
  Serial.print("Publishing time: ");
  Serial.println(timeStr);
  // Publish hour and minute values
  client.publish(hour_topic, hourStr.c_str());
  Serial.print("hour; ");
  Serial.printf(hour_topic, hourStr.c_str());
  Serial.println("");
  client.publish(minute_topic, minuteStr.c_str());
  Serial.print("minuteStr; ");
  Serial.printf(minute_topic, minuteStr.c_str());
  Serial.println("");
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
  Serial.print("redStr; ");
  Serial.printf(red_topic, redStr.c_str());
  Serial.println("");

  client.publish(green_topic, greenStr.c_str());
  Serial.print("greenStr; ");
  Serial.printf(green_topic, greenStr.c_str());
  Serial.println("");

  client.publish(blue_topic, blueStr.c_str());
  Serial.print("blueStr; ");
  Serial.printf(blue_topic, blueStr.c_str());
  Serial.println("");

  client.publish(storeCount_topic, storeCountStr.c_str());
  Serial.print("storeCountStr; ");
  Serial.printf(storeCount_topic, storeCountStr.c_str());
  Serial.println("");

  // add publish for hour and minute

  delay(5000);
}

/***************************************
 *   function to publish to MQTT       *
 *      and publish the temperature    *
 *           and storeCount            *
 *               end                   *
 ***************************************/

/***************************************
 *  function to update the sensors    *
 *           start                     *
 ***************************************/

void upDateSensors()
{
  // Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  // Serial.println("DONE");

  redTemp = static_cast<int>(sensors.getTempC(red));     // get the temperature for the red sensor
  greenTemp = static_cast<int>(sensors.getTempC(green)); // get the temperature for the green sensor
  blueTemp = static_cast<int>(sensors.getTempC(blue));   //  get the temperature for the blue sensor
  Serial.print("...");
  Serial.print("...");
  Serial.print("Red temperature: ");
  Serial.print(redTemp);
  Serial.println(" C");
  Serial.print("Green temperature: ");
  Serial.print(greenTemp);
  Serial.println(" C");
  Serial.print("Blue temperature: ");
  Serial.print(blueTemp);
  Serial.println(" C");
  Serial.println("...");
  Serial.println("...");
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
    Serial.println("...");
    Serial.println("Temperature change detected, publishing to MQTT...");
    Serial.println("...");
    publishSensorValues();
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
                connecttoFirebase(); // Attempt to reconnect to Firebase
                break; // Exit the loop after max retries
            }

            delay(retryDelay); // Wait before retrying
        }
    }

    // Check available memory
    checkMemory();
}



// void storeDataToFirebase()
// {
//   storeCount++;
//   if (storeCount > 100)
//   {
//     storeCount = 1; // Reset storeCount if it exceeds 100
//   }
//   String sensorPath = "/sensorData/";
//   sensorPath.concat(String(storeCount));
//   String redPath = sensorPath;
//   redPath.concat("/red");
//   String greenPath = sensorPath;
//   greenPath.concat("/green");
//   String bluePath = sensorPath;
//   bluePath.concat("/blue");

//   //String timePath = "/timeData/";
//   //timePath.concat(String(storeCount));
//   String hourPath = sensorPath;
//   hourPath.concat("/hour");
//   String minutePath = sensorPath;
//   minutePath.concat("/minute");

//   int retryCount = 0;
//   const int maxRetries = 3;    // Maximum number of retries
//   const int retryDelay = 1000; // Delay between retries in milliseconds
//   while (retryCount < maxRetries)
//   {
//     bool success = Firebase.RTDB.setFloat(&fbdo, redPath, redTemp) &&
//                    Firebase.RTDB.setFloat(&fbdo, greenPath, greenTemp) &&
//                    Firebase.RTDB.setFloat(&fbdo, bluePath, blueTemp) &&
//                    Firebase.RTDB.setInt(&fbdo, hourPath, Hours) &&
//                    Firebase.RTDB.setInt(&fbdo, minutePath, Minutes);

//     // }
//     if (success)
//     {
//       Firebase.RTDB.setInt(&fbdo, "/storeCount", storeCount);
//       Serial.println("Data successfully stored in Firebase.");
//       break; // Exit the loop if data is successfully stored
//     }
//     else
//     {
//       Serial.println("Failed to store data to Firebase, retrying...");
//       retryCount++;
//       if (retryCount >= maxRetries)
//       {
//         Serial.println("Failed to store data to Firebase");
//         Serial.println(fbdo.errorReason());
//         connecttoFirebase(); // Attempt to reconnect to Firebase
//         break;               // Exit the loop after max retries
//       }
//       delay(retryDelay); // Wait before retrying
//     }
//   }

//   checkMemory();
// }
/***************************************
 *  function to store data to Firebase *
 *           end                       *
 ***************************************/

/***************************************
 * function to retrieve the last stored*
 * data from Firebase on startup       *
 *           start                     *
 ***************************************/
/***************************************
 * this function will need changing    *
 * to retrieve the last stored         *
 * Target temperature values           *
 * and times to change the target      *
 * temperature values                  *
 ***************************************/
void retrieveLastStoredData()
{
  Serial.println("*******Retrieving last stored data from Firebase");
  String path = "/sensorData/";
  path.concat(String(storeCount));
  // Serial.print("Path: ");
  // Serial.println(path);
  // Serial.print("storeCount: ");
  // Serial.println(storeCount);

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

    Serial.println("************** Extracted redTemp: ");
    Serial.println(redTemp);
    Serial.print("Extracted greenTemp: ");
    Serial.println(greenTemp);
    Serial.print("Extracted blueTemp: ");
    Serial.println(blueTemp);

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
    Serial.print("Retrieved dateCount from Firebase: ");
    Serial.println(dateCount);
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
    // Serial.print("Retrieved storeCount from Firebase: ");
    // Serial.println(storeCount);
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
void connecttoFirebase()
{
  int DatdBaseretryCount = 0;
  const int maxRetries = 5; // Maximum number of retries
  while (!Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Attempting to sign up to Firebase...");
    Serial.printf("Firebase sign-up failed: %s\n", config.signer.signupError.message.c_str());
    Serial.printf("Error code: %d\n", config.signer.signupError.code);
    Serial.println("Retrying in 30 seconds...");
    DatdBaseretryCount++;
    Serial.print("Retry count: ");
    Serial.println(DatdBaseretryCount);
    if (DatdBaseretryCount >= maxRetries)
    {
      Serial.println("Max retry count reached. Restarting ESP32...");
      ESP.restart(); // Restart the ESP32 after max retries
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
  // incriment dateStoreCount
  Serial.println(" ");
  Serial.println("*******************  ");
  Serial.print("Storing date to Firebase");
  //  Serial.print("nextDay: ");
  //  Serial.println(nextDay);
  //  Serial.print("currentDay: ");
  //  Serial.println(currentDay);
  Serial.print("  ********************");
  Serial.print("");
  dateCount++;
  String datePath = "/dateData/";
  datePath.concat(String(dateCount)); // Change to dateStoreCount
  String dayPath = datePath;
  dayPath.concat("/day");
  String monthPath = datePath;
  monthPath.concat("/month");

  bool success = Firebase.RTDB.setInt(&fbdo, dayPath, currentDay) &&
                 Firebase.RTDB.setInt(&fbdo, monthPath, currentMonth);
  Serial.print("currentMonth: ");
  Serial.println(currentMonth);
  Serial.print("currentDay: ");
  Serial.println(currentDay);
  // bool success = Firebase.RTDB.setInt(&fbdo, dayPath, currentDay) &&
  //                Firebase.RTDB.setInt(&fbdo, monthPath, currentMonth);

  if (!success)
  {
    Serial.println("Failed to store pending date in Firebase.");
    Serial.print("Pending Target Day: ");
    Serial.println(pendingTargetDay);
    Serial.print("Pending Test Month: ");
    Serial.println(pendingTestMonth);
    Serial.print("1119 ******* Failed to store date in Firebase.  ");
    Serial.println(fbdo.errorReason());
    connecttoFirebase(); // Attempt to reconnect to Firebase
    // Mark date storage as pending
    pendingDateStorage = true;
    pendingTargetDay = currentDay;
    pendingTestMonth = currentMonth;
  }

  if (success)
  {
    Serial.println("Date successfully stored in Firebase.");
  }
  else
  {
    Serial.print("1134 ******** Failed to store date in Firebase.  ");
    Serial.println(fbdo.errorReason());
    connecttoFirebase(); // Attempt to reconnect to Firebase
  }
  //}
  // else // can be removed
  // {
  //   Serial.println("nextDay does not match currentDay. Skipping date storage."); // Return true since this is not a failure, just a skip
  // }
}
/************************************
 *     function to store date       *
 *          in Firebase             *
 *           end                    *
 ***********************************/

//  /*************************************************************
//                             Relay Control
//                                  start
// *************************************************************/

// void relay_Control() {
//   int targetTemp = AmFlag ? amTemp : pmTemp;
//   if (s3 < targetTemp) {
//     digitalWrite(Relay_Pin, HIGH);
//     digitalWrite(LED_Pin, HIGH);  // LED_Pin on
//     digitalWrite(LED_BUILTIN, LOW);  // LED_Pin on
//     heaterStatus = true;
//     if (!heaterOn) {
//       startHeaterTimer();
//     }
//   } else if (s3 > targetTemp) {
//     digitalWrite(Relay_Pin, LOW);
//     digitalWrite(LED_Pin, LOW);  // LED_Pin off
//     digitalWrite(LED_BUILTIN, HIGH);  // LED_Pin off
//     heaterStatus = false;
//     heaterOn = false;
//   }
// }
// /*************************************************************
//                             Relay Control
//                                  End
// *************************************************************/

// /*************************************************************
//                               Heater Control
//                                    start
//   ************************************************************/
//   if (Am) {
//     if (amHours == Hours && amMinutes == Minutes) {  // set amTemp for the Night time setting
//       AmFlag = true;
//       amTemp = amTemperature;
//       int myTemp = amTemp;
//       sprintf(sensVal, "%d", myTemp);
//       client.publish("targetTemperature", sensVal, true);
//     }
//   } else {
//     if (pmHours == Hours && pmMinutes == Minutes) {  // set pmTemp for the Night time setting
//       AmFlag = false;
//       pmTemp = pmTemperature;
//       int myTemp = pmTemp;
//       sprintf(sensVal, "%d", myTemp);
//       client.publish("targetTemperature", sensVal, true);
//     }
//   }
// }
// /*************************************************************
//                              Heater Control
//                                     End
//  *************************************************************/