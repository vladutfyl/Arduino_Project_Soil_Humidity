#include<ESP8266WebServer.h>

const char* ssid = "TP-LINK_E93A78"; // wifi network name
const char* password = "filypvasile27"; // password

ESP8266WebServer server(80);

int waterPump = 13;
int sensorPin = A0;
int sensorValue;
int waterPumpState = LOW;
int limit = 600;
void setup() {

  Serial.begin(115200);
  delay(10);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, LOW);

  Serial.println("");
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/waterPumpOn", handle_waterPumpOn);
  server.on("/waterPumpOff", handle_waterPumpOff);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  
  server.handleClient();
  if (waterPumpState)
  {
    digitalWrite(waterPump, HIGH);
  }
  else
  {
    digitalWrite(waterPump, LOW);
  }
}
void handle_OnConnect() {
  sensorValue = analogRead(sensorPin);
  server.send(200, "text/html", SendHTML(sensorValue, waterPumpState));
}
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
void handle_waterPumpOff() {
  sensorValue = analogRead(sensorPin);
  waterPumpState = LOW;
  server.send(200, "text/html", SendHTML(sensorValue, waterPumpState));
}
void handle_waterPumpOn() {
  sensorValue = analogRead(sensorPin);
  waterPumpState = HIGH;
  server.send(200, "text/html", SendHTML(sensorValue, waterPumpState));
}
String SendHTML (int sensorValue, int waterPumpState) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>LED Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,500);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  if (waterPumpState)
  {
    ptr += "<p>Water Pump Status: ON</p><a class=\"button button-off\" href=\"/waterPumpOff\">OFF</a>\n";
  }
  else
  {
    ptr += "<p>Water Pump Status: OFF</p><a class=\"button button-on\" href=\"/waterPumpOn\">ON</a>\n";
  }
  ptr += "<p>Soil Moisture value: ";
  ptr += sensorValue;
  ptr += " </p>";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
