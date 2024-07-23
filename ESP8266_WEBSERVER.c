#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define DHT11 Sensor
#define DHTPIN D7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define Motor Pins
#define MOTOR_PIN1 D5
#define MOTOR_PIN2 D6

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Wi-Fi Credentials
const char* ssid = "Redmi8";
const char* password = "7415603507n";

WiFiServer server(80);

float temperatureThreshold = 25.0; // Default threshold value
float lastTemperature = -999.0;
float lastHumidity = -999.0;
unsigned long lastLcdUpdate = 0;
int lcdPage = 0;

void setup() {
  Serial.begin(9600);
  
  dht.begin();
  
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi..");

  
  connectWiFi();
  
  server.begin();
  Serial.println("Server started");
  
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  static unsigned long lastSensorReadTime = 0;
  static unsigned long lastMotorControlTime = 0;
  static unsigned long lastClientHandleTime = 0;

  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorReadTime >= 2000) { // Read sensor data every 2 seconds
    readAndDisplaySensorData();
    lastSensorReadTime = currentMillis;
  }

  if (currentMillis - lastMotorControlTime >= 5000) { // Control motor every 5 seconds
    controlMotor();
    lastMotorControlTime = currentMillis;
  }

  if (currentMillis - lastClientHandleTime >= 1000) { // Handle client requests every 1 second
    handleClient();
    lastClientHandleTime = currentMillis;
  }

  updateLcdDisplay(); // Update LCD display without delay
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > 10000) { // 10 seconds timeout
      Serial.println("Failed to connect to WiFi. Retrying...");
      WiFi.begin(ssid, password); // Retry connection
      startTime = millis(); // Reset start time
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/motor/on") != -1) {
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
    } else if (request.indexOf("/motor/off") != -1) {
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
    } else if (request.indexOf("/setThreshold?value=") != -1) {
      int pos = request.indexOf('=');
      String val = request.substring(pos + 1);
      temperatureThreshold = val.toFloat();
    } else if (request.indexOf("/data") != -1) {
      sendSensorData(client);
    } else if (request.indexOf("/details") != -1) {
      sendDetailsPage(client);
    } else {
      sendWebPage(client);
    }

    delay(1);
  }
}

void sendWebPage(WiFiClient& client) {
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head>";
  s += "<title>Addas Agriculture Automation Project</title>";
  s += "<style>";
  s += "body { font-family: Arial, sans-serif; text-align: center; background: linear-gradient(45deg, #FF6347, #4682B4, #00FF7F, #FFD700); color: #fff; }";
  s += ".container { display: flex; justify-content: center; flex-direction: column; align-items: center; }";
  s += ".card { background: #fff; padding: 20px; margin: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); border-radius: 10px; }";
  s += ".box { background-color: #4682b4; color: #fff; padding: 10px; border-radius: 5px; margin-bottom: 10px; }";
  s += "#thresholdForm { margin-top: 20px; }";
  s += ".button { padding: 10px 20px; margin: 10px; background: #4682b4; color: #fff; border: none; border-radius: 5px; cursor: pointer; }";
  s += "#chartContainer { width: 90%; max-width: 700px; margin-top: 20px; padding: 20px; background: #fff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  s += "canvas { background: #f9f9f9; border-radius: 10px; }";
  s += "</style>";
  s += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  s += "</head>";
  s += "<body>";
  s += "<div class='container'>";
  s += "<h1>Addas Agriculture Automation Project</h1>";
  s += "<div class='card'>";
  s += "<div class='box'>Temperature: <span id='temperature'></span> &#8451;</div>";
  s += "<div class='box'>Humidity: <span id='humidity'></span> %</div>";
  s += "<div class='box'>Threshold Temperature: <span id='threshold'></span> &#8451;</div>";
  s += "<form id='thresholdForm' action=\"/setThreshold\">";
  s += "<input type=\"text\" name=\"value\" placeholder='Set Threshold'>";
  s += "<input type=\"submit\" value=\"Set Threshold\" class='button'>";
  s += "</form>";
  s += "<p><a href=\"/\"><a href=\"/motor/on\" class='button'>Motor ON</a></a></p>";
  s += "<p><a href=\"/\"><a href=\"/motor/off\" class='button'>Motor OFF</a></a></p>";
  s += "</div>";
  s += "<div id='chartContainer'>";
  s += "<canvas id='tempChart'></canvas>";
  s += "</div>";
  s += "<a href=\"/details\" class='button'>Project Details</a>";
  s += "</div>";
  s += "<script>";
  s += "let tempData = [];";
  s += "let humidityData = [];";
  s += "let myChart;";
  s += "setInterval(function() {";
  s += "fetch('/data').then(response => response.json()).then(data => {";
  s += "document.getElementById('temperature').innerText = data.temperature;";
  s += "document.getElementById('humidity').innerText = data.humidity;";
  s += "document.getElementById('threshold').innerText = data.threshold;";
  s += "tempData.push(data.temperature);";
  s += "humidityData.push(data.humidity);";
  s += "if (tempData.length > 10) tempData.shift();";
  s += "if (humidityData.length > 10) humidityData.shift();";
  s += "updateChart();";
  s += "});";
  s += "}, 1000);";
  s += "function updateChart() {";
  s += "let ctx = document.getElementById('tempChart').getContext('2d');";
  s += "if (myChart) myChart.destroy();";
  s += "myChart = new Chart(ctx, { type: 'line', data: { labels: Array.from({ length: tempData.length }, (_, i) => i + 1), datasets: [{ label: 'Temperature', data: tempData, borderColor: '#FF6347', backgroundColor: 'rgba(255, 99, 71, 0.2)', fill: true }, { label: 'Humidity', data: humidityData, borderColor: '#4682B4', backgroundColor: 'rgba(70, 130, 180, 0.2)', fill: true }] }, options: { scales: { x: { title: { display: true, text: 'Time (s)' } }, y: { title: { display: true, text: 'Value' } } } } });";
  s += "}";
  s += "</script>";
  s += "</body>";
  s += "</html>";

  client.print(s);
}

void sendDetailsPage(WiFiClient& client) {
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head>";
  s += "<title>Project Details</title>";
  s += "<style>";
  s += "body { font-family: Arial, sans-serif; text-align: center; background: linear-gradient(45deg, #FF6347, #4682B4, #00FF7F, #FFD700); color: #fff; }";
  s += ".container { max-width: 800px; margin: auto; padding: 20px; background: rgba(255, 255, 255, 0.9); color: #000; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  s += "h1 { margin-top: 0; }";
  s += "ul { text-align: left; }";
  s += "</style>";
  s += "</head>";
  s += "<body>";
  s += "<div class='container'>";
  s += "<h1>Project Details</h1>";
  s += "<h2>Procedure</h2>";
  s += "<p>This project involves the development of an IoT-based agriculture automation system. The steps followed are:</p>";
  s += "<ul>";
  s += "<li><strong>Requirement Analysis:</strong> Identifying the needs and defining the scope of the project.</li>";
  s += "<li><strong>Design and Development:</strong> Creating schematics and writing the firmware for sensor and motor control.</li>";
  s += "<li><strong>Integration:</strong> Connecting the hardware components and integrating the software.</li>";
  s += "<li><strong>Testing:</strong> Conducting rigorous tests to ensure system reliability and performance.</li>";
  s += "<li><strong>Deployment:</strong> Installing the system in a real-world environment.</li>";
  s += "</ul>";
  s += "<h2>Function</h2>";
  s += "<p>The primary function of this system is to monitor and control agricultural parameters. It uses a various sensor to collect data, and it controls a motor based on the sensor  threshold value. The system also provides a web interface for real-time monitoring and control.</p>";
  s += "<h2>Benefits</h2>";
  s += "<p>The benefits of this project include:</p>";
  s += "<ul>";
  s += "<li><strong>Improved Efficiency:</strong> Automated control reduces manual labor and increases efficiency.</li>";
  s += "<li><strong>Real-time Monitoring:</strong> Provides real-time data on environmental conditions, allowing for timely interventions.</li>";
  s += "<li><strong>Remote Access:</strong> The web interface enables remote monitoring and control, providing convenience and flexibility.</li>";
  s += "<li><strong>Resource Management:</strong> Helps in better management of water and other resources by automating irrigation based on environmental conditions.</li>";
  s += "</ul>";
  s += "<h2>Team Members</h2>";
  s += "<p>Our dedicated team comprises the following members:</p>";
  s += "<ul>";
  s += "<li><strong> Nitin Lodhi </strong>  </li>";
  s += "<li><strong> Sunny Sarovar </strong>  </li>";
  s += "<li><strong> Akash Rai </strong>  </li>";
  s += "<li><strong> Kiran Thakur </strong> </li>";
  s += "</ul>";
  s += "<h2>College</h2>";
  s += "<p>This Minor project  is undertaken at Oriental College of Technology, Bhopal.</p>";
  s += "<a href=\"/\" class='button' style='display: inline-block; padding: 10px 20px; background: #4682b4; color: #fff; border-radius: 5px; text-decoration: none;'>Back to Main Page</a>";
  s += "</div>";
  s += "</body>";
  s += "</html>";

  client.print(s);
}

void sendSensorData(WiFiClient& client) {
  String json = "{";
  json += "\"temperature\":" + String(lastTemperature ) + ",";
  json += "\"humidity\":" + String(lastHumidity) + ",";
  json += "\"threshold\":" + String(temperatureThreshold);
  json += "}";

  client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
  client.print(json);
}

void readAndDisplaySensorData() {
  float temperature = dht.readTemperature() *2 + 20 ;
  float humidity = dht.readHumidity() *2 + 20;

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lastTemperature = temperature;
  lastHumidity = humidity;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %");
}

void controlMotor() {
  if (lastTemperature > temperatureThreshold) {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
  } else {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
  }
}

void updateLcdDisplay() {
  if (millis() - lastLcdUpdate >= 5000) {
    lcdPage = (lcdPage + 1) % 2;
    lastLcdUpdate = millis();
    switch (lcdPage) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(lastTemperature);
        lcd.print(" C   ");
        lcd.setCursor(0, 1);
        lcd.print("Hum: ");
        lcd.print(lastHumidity);
        lcd.print(" %   ");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Threshold: ");
        lcd.print(temperatureThreshold);
        lcd.print(" C   ");
        lcd.setCursor(0, 1);
        lcd.print("Motor: ");
        lcd.print((lastTemperature > temperatureThreshold) ? "ON " : "OFF");
        lcd.print("    ");
        break;
    }
  }
}


