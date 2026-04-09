#include <WiFi.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak API
String apiKey = "YOUR_THINGSPEAK_API_KEY";
const char* server = "http://api.thingspeak.com/update";

// Sensor Pins
#define TDS_PIN 34
#define TURBIDITY_PIN 35
#define PH_PIN 32

// ADC settings
const float VREF = 3.3;
const int ADC_RESOLUTION = 4095;

// Moving average filter
#define NUM_SAMPLES 10
float tdsSamples[NUM_SAMPLES];
float turbSamples[NUM_SAMPLES];
float phSamples[NUM_SAMPLES];

// Thresholds
#define TDS_SAFE 500
#define PH_LOW 6.5
#define PH_HIGH 8.5
#define TURBIDITY_SAFE 5

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

// Moving Average Function
float movingAverage(float *arr, float newVal) {
  static int index = 0;
  arr[index] = newVal;
  index = (index + 1) % NUM_SAMPLES;

  float sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += arr[i];
  }
  return sum / NUM_SAMPLES;
}

// Convert ADC to voltage
float readVoltage(int pin) {
  int adcValue = analogRead(pin);
  return (adcValue * VREF) / ADC_RESOLUTION;
}

// Calibration equations (approximate)
float calculateTDS(float voltage) {
  return (133.42 * voltage * voltage * voltage 
         - 255.86 * voltage * voltage 
         + 857.39 * voltage) * 0.5;
}

float calculateTurbidity(float voltage) {
  return -1120.4 * voltage * voltage + 5742.3 * voltage - 4352.9;
}

float calculatePH(float voltage) {
  return 7 + ((2.5 - voltage) / 0.18);
}

// Rule-based classification
String classifyWater(float tds, float ph, float turbidity) {
  if (tds < TDS_SAFE && ph >= PH_LOW && ph <= PH_HIGH && turbidity < TURBIDITY_SAFE) {
    return "SAFE";
  } else {
    return "CONTAMINATED";
  }
}

void loop() {
  // Read voltages
  float tdsVolt = readVoltage(TDS_PIN);
  float turbVolt = readVoltage(TURBIDITY_PIN);
  float phVolt = readVoltage(PH_PIN);

  // Apply filtering
  float tdsFiltered = movingAverage(tdsSamples, tdsVolt);
  float turbFiltered = movingAverage(turbSamples, turbVolt);
  float phFiltered = movingAverage(phSamples, phVolt);

  // Convert to parameters
  float tds = calculateTDS(tdsFiltered);
  float turbidity = calculateTurbidity(turbFiltered);
  float ph = calculatePH(phFiltered);

  // Classification
  String status = classifyWater(tds, ph, turbidity);

  // Print values
  Serial.println("---- Water Quality ----");
  Serial.print("TDS: "); Serial.println(tds);
  Serial.print("Turbidity: "); Serial.println(turbidity);
  Serial.print("pH: "); Serial.println(ph);
  Serial.print("Status: "); Serial.println(status);

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = server;
    url += "?api_key=" + apiKey;
    url += "&field1=" + String(tds);
    url += "&field2=" + String(turbidity);
    url += "&field3=" + String(ph);
    url += "&field4=" + status;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Error sending data");
    }

    http.end();
  }

  delay(15000); // 15 seconds interval
}
