#include <WiFi.h>
#include <HttpClient.h>
// Including Wire.h and Adafruit_AHTX0.h libraries to implement in our code
#include <Wire.h>
#include <Adafruit_AHTX0.h>


char ssid[] = "NETGEAR34";
char pass[] = "Cost4Mes4,S4nt44n4,S4nDiego";

const char kHostname[] = "18.218.147.252";
const int kPort = 5000;
const char kPath[] = "/";

const int kNetworkTimeout = 30 * 1000;
const int kNetworkDelay = 1000;

// Creating object for DHT20 Sensor
Adafruit_AHTX0 tempSensor;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());


  // Debugging code to ensure connection to DHT20 is found
  Serial.println("Connecting to DHT20 device.");
  if (!tempSensor.begin()) {
    Serial.println("The DHT20 device is not found.");
    while (1); // stop program if the sensor is not found
  }

  Serial.println("The DHT20 was found. Initializing...");
}

void loop() {
  // Debugging to get desired output
  // Not necessarily needed, we can probably just comment this out
  Serial.println("Start of Loop."); 


  // Holds sensors event data for humidity and temperature from the DHT20 device
  sensors_event_t humidityValue;
  sensors_event_t temperatureValue;
  // Retrieve temperature and humidity data from DHT20 device
  tempSensor.getEvent(&humidityValue, &temperatureValue);

  // Assign float variables to hold the temperature and humidity data (using float to be able to hold decimal values)
  float temp = temperatureValue.temperature;
  float humidity = humidityValue.relative_humidity;


  // Send the data to the cloud using new variable dataPath, which holds the path to the data values
  String data = "tempVal=" + String(temp) + "&humVal=" + String(humidity);
  String dataPath = String(kPath) + "?" + data;

  // Print temperature and humidity values to the local terminal
  Serial.print("Current temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Current humidity: ");
  Serial.print(humidity);
  Serial.println(" %");


  // From here below, this is all the same code from the given code in the instructions
  // Commented out some of the debugging print statements for if the connection was found (for the sake of cleanliness) 
  int err = 0;
  WiFiClient c;
  HttpClient http(c);

  //Serial.println("Sending HTTP request..."); // Debugging output

  // Changed the kpath to datapath, to get the values from the temperature and humidity.
  // dataPath.c_str() to convert dataPath to const char* since that is what http.get expects to receive
  err = http.get(kHostname, kPort, dataPath.c_str());
  if (err == 0) {
    err = http.responseStatusCode();
    if (err >= 0) {
      //Serial.print("Got status code: ");
      //Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        //Serial.println("Application>  HTTP Response Body : ");

        unsigned long timeoutStart = millis();
        char character;
        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            character = http.read();
            //Serial.print(character);
            bodyLen--;
            timeoutStart = millis();
          } else {
            delay(kNetworkDelay);
          }
        }
        Serial.println();
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

  // Added delay of 2.5sec
  delay(2500);
}