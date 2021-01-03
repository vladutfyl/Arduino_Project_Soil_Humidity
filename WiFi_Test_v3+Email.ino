#include<ESP8266WebServer.h>


const char* ssid = "TP-LINK_E93A78"; // wifi network name
const char* password = "filypvasile27"; // password
char serverAd[] = "smtp.gmail.com"; // The SMTP Server 

ESP8266WebServer server(80);
WiFiClientSecure espClient;

int waterPump = 13;
int sensorPin = A0;
int sensorValue;
int waterPumpState = LOW;
int limit = 600;
int ok=0;//verifica daca a fost trimis deja email
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
  if(limit<sensorValue and ok==0){
    sendEmail();
    ok=1;
  }
  server.send(200, "text/html", SendHTML(sensorValue, waterPumpState));
}
void handle_waterPumpOn() {
  sensorValue = analogRead(sensorPin);
  waterPumpState = HIGH;
  ok=0;
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
byte sendEmail()
{
  if (espClient.connect(serverAd, 465) == 1) 
  {
    Serial.println(F("connected"));
  } 
  else 
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp()) 
    return 0;
  
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO google");
  if (!emailResp()) 
    return 0;
  
  
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp()) 
    return 0;

  Serial.println(F("Sending User"));
  espClient.println("dGVzdHBtcGZpbGlwQGdtYWlsLmNvbQ=="); //base64, ASCII encoded Username/Email Address
  if (!emailResp()) 
    return 0;
 
  Serial.println(F("Sending Password"));
  espClient.println("dGVzdC5wbXAx");//base64, ASCII encoded Password
  if (!emailResp()) 
    return 0;
    
  Serial.println(F("Sending From"));
  // you can change to sender email address
  espClient.println(F("MAIL FROM: <testpmpfilip@gmail.com>"));// necesita <> in jurul email ului conform sintaxei
  if (!emailResp()) 
    return 0;
  
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: <testpmpfilip@gmail.com>"));
  if (!emailResp()) 
    return 0;
  
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp()) 
    return 0;
    
  Serial.println(F("Sending email"));
  // change to recipient address
  espClient.println(F("To: <testpmpfilip@gmail.com>"));
  // change to your address
  espClient.println(F("From: testpmpfilip@gmail.com"));
  espClient.println(F("Subject: Water your plant\r\n"));
  espClient.println(F("Your plants need water.\n"));
  
  espClient.println(F("."));// necesar pentru terminare
  if (!emailResp()) 
    return 0;
  
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp()) 
    return 0;
  
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte emailResp()//raspunsul server-ului la comenzile trimise in sendmail()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available()) 
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000) 
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    return 0;
  }
  return 1;
}
