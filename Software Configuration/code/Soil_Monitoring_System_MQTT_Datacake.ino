// include the required libraries
#include <ModbusMaster.h> // for reading the RS485 protocol
#include <SoftwareSerial.h> // for virtual serial communication using digital pins
#include <WiFiClientSecure.h> // WiFi library
#include <PubSubClient.h> // MQTT publish-subscribe client
#include "secrets.h" // Contains all the WiFi and MQTT credentials
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// define starting register addresses for the sensors
#define SENSOR3IN1 0x00
#define NPKSENSOR  0x1e
#define TEMPSENSOR 0x00

// define pins of Resistive and Capacitive Sensors
#define RSENSOR 1
#define CSENSOR 2

// define RE and DE pins of MAX485 converter
#define RE 6
#define DE 7

// define SSD_1306 OLED Display
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C

// define conversion factor for microseconds to seconds
#define uS_TO_S_FACTOR 1000000ULL
// define time ESP32 will go to sleep (in seconds)
#define TIME_TO_SLEEP  1800

// initialize boot count of the ESP32
RTC_DATA_ATTR int bootCount = 0;

//define constant value for max and minimum R and C moisture sensor reading
const int maxRsensorValue =3248 ;
const int minRsensorValue =1773 ;
const int maxCsensorValue =2560 ;
const int minCsensorValue =1184 ;

//function call
EspSoftwareSerial::UART swSerial;
ModbusMaster node;
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);

void setup() 
{
  // Hardware serial at 115200 baud rate
  Serial.begin(115200);

  delay(500);

  // Configure the wake up source - timer wakeup
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  // configure the MAX485 RE & DE control signals and enable receive mode
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  digitalWrite(RE, 0);
  digitalWrite(RE, 0);

  // Callbacks to allow us to set the RS485 direction
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  delay(100);

  // Setup WiFi
  WiFi.setHostname(hostname);
  WiFi.mode(WIFI_AP_STA);
  wifiConnect();
  wifiClient.setCACert(root_ca);  // Verify CA certificate

  delay(100);

  // Setup MQTT
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttConnect();

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Welcome");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 17);
  display.println("This is");
  display.println("a Splash");
  display.println("Screen");

  display.display();
}

void loop()
{
  uint8_t result1, result2, result3;
  int rSensorValue, cSensorValue;
  int16_t currentCursorX;
  uint16_t EC;
  float Moisture, Temperature, Nitrogen, Phosphorus, Potassium, Ambi_Temp, Humidity, rSensorMoisture, cSensorMoisture;

  // Modbus communication runs at 4800/9600 baud; 8 bits, no parity and 1 stop bit; RO/RX pin, DI/TX pin
  swSerial.begin(4800, SWSERIAL_8N1, 17, 16);

  delay(5);

  // Initialize communication with Modbus at device id 1
  node.begin(1, swSerial);
  // Read 3 registers at starting address SENSOR3IN1
  result1 = node.readHoldingRegisters(SENSOR3IN1, 3);

  if (result1 == node.ku8MBSuccess)
  {
    Serial.print("Moisture: ");
    Moisture = node.getResponseBuffer(0x00)/10.0f;
    Serial.print(Moisture);
    Serial.println(" %"); 

    Serial.print("Temperature: ");
    Temperature = node.getResponseBuffer(0x01)/10.0f;
    Serial.print(Temperature);
    Serial.println(" °C");

    Serial.print("EC: ");
    EC = node.getResponseBuffer(0x02);
    Serial.print(EC);
    Serial.println(" us/cm");
  } 
  else
  {
    printModbusError(result1);
  }
  Serial.println();
  swSerial.end();
  delay(200);

  // Modbus communication runs at 4800/9600 baud; 8 bits, no parity and 1 stop bit; RO/RX pin, DI/TX pin
  swSerial.begin(9600, SWSERIAL_8N1, 17, 16);

  delay(5);

  // Initialize communication with Modbus at device id 2
  node.begin(2, swSerial);
  // Read 3 registers at starting address NPKSENSOR
  result2 = node.readHoldingRegisters(NPKSENSOR, 3);

  if (result2 == node.ku8MBSuccess)
  {
    Serial.print("Nitrogen: ");
    Nitrogen = node.getResponseBuffer(0x00);
    Serial.print(Nitrogen);
    Serial.println(" mg/kg"); 

    Serial.print("Phosphorus: ");
    Phosphorus = node.getResponseBuffer(0x01);
    Serial.print({Phosphorus});
    Serial.println(" mg/kg");

    Serial.print("Potassium: ");
    Potassium = node.getResponseBuffer(0x02);
    Serial.print(Potassium);
    Serial.println(" mg/kg");
  } 
  else
  {
    printModbusError(result2);
  }
  Serial.println();

  swSerial.end();
  delay(200);

  // Modbus communication runs at 4800/9600 baud; 8 bits, no parity and 1 stop bit; RO/RX pin, DI/TX pin
  swSerial.begin(9600, SWSERIAL_8N1, 17, 16);

  delay(5);

  // Initialize communication with Modbus at device id 3
  node.begin(3, swSerial);
  // Read 2 registers at starting address TEMPSENSOR
  result3 = node.readHoldingRegisters(TEMPSENSOR, 2);

  if (result3 == node.ku8MBSuccess)
  {
    Serial.print("Humidity: ");
    Humidity = node.getResponseBuffer(0x00)/10.0f;
    Serial.print(Humidity);
    Serial.println(" %"); 

    Serial.print("Ambient Temperature: ");
    Ambi_Temp = node.getResponseBuffer(0x01)/10.0f;
    Serial.print(Ambi_Temp);
    Serial.println(" °C"); 
  } 
  else {
    printModbusError(result3);
  }
  Serial.println();

  swSerial.end();
  delay(200);


  // // Taking capacitive and resistive soil moisture sensors
  // rSensorValue = analogRead(RSENSOR);
  // cSensorValue = analogRead(CSENSOR);

  // rSensorMoisture = map(rSensorValue, maxRsensorValue, minRsensorValue, 0, 100);
  // rSensorMoisture = rSensorMoisture/1.0f;
  // cSensorMoisture = map(cSensorValue, maxCsensorValue, minCsensorValue, 0, 100);
  // cSensorMoisture = cSensorMoisture/1.0f;

  // Serial.print("Resistive Sensor Moisture: ");
  // Serial.print(rSensorMoisture);
  // Serial.println(" %"); 
  // Serial.print("Capacitive Sensor Moisture: ");
  // Serial.print(cSensorMoisture);
  // Serial.println(" %"); 
  // delay(200);

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("S");
  currentCursorX = display.getCursorX();
  display.setCursor(currentCursorX + 1, 8);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("oil ");
  currentCursorX = display.getCursorX();
  display.setCursor(currentCursorX, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("P");
  currentCursorX = display.getCursorX();
  display.setCursor(currentCursorX + 1, 8);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("arameters");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 17);
  display.print("SM:");
  display.print(Moisture, 1);
  display.print("% ");
  display.print("EC:");
  display.print(EC);
  display.println("us/cm");
  display.print("T:");
  display.print(Temperature, 1);
  display.print("C ");
  display.print("H:");
  display.print(Humidity, 1);
  display.println("%");
  display.print("N:");
  display.print(Nitrogen, 1);
  display.println("mg/kg");
  display.print("P:");
  display.print(Phosphorus, 1);
  display.println("mg/kg");
  display.print("K:");
  display.print(Potassium, 1);
  display.println("mg/kg");

  display.display();

  // Make sure WiFi is connected
  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnect();
  }

  // Make sure MQTT client is connected
  if (!mqttClient.connected())
  {
    mqttConnect();
  }

  mqttClient.loop();

  // Parse data in JSON format
  String payload =  "{\"Moisture\": " + String(Moisture) + ", \"Temperature\": " + String(Temperature) + ", \"EC\": " + String(EC) + 
                    ", \"Nitrogen\": " + String(Nitrogen) + ", \"Phosphorus\": " + String(Phosphorus) + ", \"Potassium\": " + String(Potassium) + 
                    ", \"Humidity\": " + String(Humidity) + ", \"Ambi_Temp\": " + String(Ambi_Temp) + ", \"R_Moisture\": " + String(rSensorMoisture) + ", \"C_Moisture\": " + String(cSensorMoisture) + "}";

  if (result1 != node.ku8MBResponseTimedOut && result2 != node.ku8MBResponseTimedOut && result3 != node.ku8MBResponseTimedOut)
  {
  mqttClient.publish(MQTT_PUB_TOPIC, payload.c_str());
  Serial.println("Payload published");
  Serial.println();
  delay(100);
  }
  else
  {
    Serial.println("Payload not published");
    Serial.println();
    delay(1000);
  }
  // Putting ESP32 into sleep mode

  Serial.println("Going to sleep now");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_deep_sleep_start();
}

// Put the MAX485 into transmit mode
void preTransmission()
{
  digitalWrite(RE, 1);
  digitalWrite(DE, 1);
}

// Put the MAX485 into receive mode
void postTransmission()
{
  digitalWrite(RE, 0);
  digitalWrite(DE, 0);
}

void wifiConnect()
{
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi...");

  uint8_t retryCounter = 10;
  while (WiFi.status() != WL_CONNECTED && retryCounter > 0)
  {
    Serial.print(".");
    delay(500);
    retryCounter--;
  }

  if (retryCounter > 0)
  {
    Serial.println("connected");
  }
  else
  {
    Serial.print("failed, status code =");
    Serial.println(WiFi.status());
    Serial.println("Restarting in 5 sec");
    delay(5000);
    ESP.restart();
  }
}

// print out the error received from the Modbus library
void printModbusError(uint8_t errNum)
{
  switch (errNum)
  {
    case node.ku8MBSuccess:
      Serial.println(F("Success"));
      break;
    case node.ku8MBIllegalFunction:
      Serial.println(F("Illegal Function Exception"));
      break;
    case node.ku8MBIllegalDataAddress:
      Serial.println(F("Illegal Data Address Exception"));
      break;
    case node.ku8MBIllegalDataValue:
      Serial.println(F("Illegal Data Value Exception"));
      break;
    case node.ku8MBSlaveDeviceFailure:
      Serial.println(F("Slave Device Failure"));
      break;
    case node.ku8MBInvalidSlaveID:
      Serial.println(F("Invalid Slave ID"));
      break;
    case node.ku8MBInvalidFunction:
      Serial.println(F("Invalid Function"));
      break;
    case node.ku8MBResponseTimedOut:
      Serial.println(F("Response Timed Out"));
      break;
    case node.ku8MBInvalidCRC:
      Serial.println(F("Invalid CRC"));
      break;
    default:
      Serial.println(F("Unknown Error"));
      break;
  }
}

// Method to print the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void mqttConnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("MQTT connecting...");
    String clientId = "ESP32Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
      Serial.println("connected");
      mqttClient.subscribe(MQTT_SUB_TOPIC);
    }
    else
    {
      Serial.print("failed, status code =");
      Serial.print(mqttClient.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  Serial.printf("MQTT Received [%s]: ", topic);
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.printf(" (len=%d)", length);
  Serial.println();
}