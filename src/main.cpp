#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TP-Link_24AD";
const char* pass = "11111111";

WebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP32 Server</title>
</head>
<body>
  <h1>Привіт з ESP32 </h1>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}


void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.clearWriteError();
  pinMode(2, OUTPUT);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    digitalWrite(2, HIGH);
    delay(499);
    digitalWrite(2, LOW);
    delay(499);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
  digitalWrite(2, HIGH);
}

void loop() {
  server.handleClient();
}