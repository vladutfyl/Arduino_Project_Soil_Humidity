#include <ESP8266WiFi.h>

const char* ssid = "TP-LINK_E93A78";
const char* password = "filypvasile27";
 
int pumpPin = 13; // GPIO13
int analogIn = A0; // GPIO13
int pumpState = LOW;
int sensorValue=600;
const int waterLimit=600;

WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);

  //Init pump state
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, pumpState);

 
  // Connectarea la wifi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Pornirea Serverului
  server.begin();
  Serial.println("Server started");
 
  // afisarea adresei IP
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {
  // verificarea conectarii unui client
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // asteptarea datelor de la client
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // citirea primei linii de comanda trimisa de client
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //verificarea comenzii
  if (request.indexOf("cmd=TURN_ON_WATER") != -1 )
  {
    //starea pentru pornirea pompei
    pumpState = HIGH;
  }
  else if (request.indexOf("cmd=TURN_OFF_WATER") != -1  )
  {
     // starea pentru stingerea pompei
    pumpState = LOW;    
  }else if (waterLimit>sensorValue){
    // starea pentru stingerea pompei
    pumpState = LOW;
  }else if (waterLimit<sensorValue){
    //starea pentru pornirea pompei
    pumpState = HIGH;
  }
  
  // Update the LED based on ledState
  digitalWrite(pumpPin, pumpState);
  

  //sensorValue se actualizeaza la fiecare request, dar avem si comanda
  // RELOAD_SENSOR_VALUE pentru a putea vedea valoare fara a modifica starea pompei
  sensorValue = analogRead(analogIn);
  
  // Raspunsul
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Water pump is now: ");
 
  if(pumpState == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"?cmd=TURN_ON_WATER\"><button>Turn On </button></a>");
  client.println("<a href=\"?cmd=TURN_OFF_WATER\"><button>Turn Off </button></a>");

  client.println("<p>The moisture sensor's value is:<b> ");
  client.println(sensorValue); 
  client.println("</b></p>");
  client.println("<a href=\"?cmd=RELOAD_SENSOR_VALUE\"><button>Check for new moisture sensor's value</button></a>");
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
