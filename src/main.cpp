#include <Arduino.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>

#define T1 15
#define T2 5

OneWire t1(T1);
OneWire t2(T2);
DallasTemperature sensor1(&t1);
DallasTemperature sensor2(&t2);

const char* ssid = "netis";
const char* pass = "11111111";

AsyncWebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Температура</title>
<style>
body {font-family:Arial;background:#f4f6f8;text-align:center;padding:20px;}
.container {display:flex;justify-content:center;gap:20px;flex-wrap:wrap;}
.card {background:white;padding:20px;border-radius:12px;
box-shadow:0 4px 10px rgba(0,0,0,0.1);width:200px;}
.temp {font-size:32px;font-weight:bold;margin-top:10px;}
</style>
</head>
<body>
<div class="container">
<div class="card">
<h3>Котел</h3>
<div id="t1" class="temp">--</div>
</div>
<div class="card">
<h3>Теплиця</h3>
<div id="t2" class="temp">--</div>
</div>
<button onclick="fetch('/reset').catch(() => {})">Перезагрузка</button>
</div>
<script>

let loading = false;

async function response() {
    if (!document.hidden && !loading) {
        loading = true;

        try {
            let response = await fetch("/temps").catch(() => {});
            let r = await response.json();

            document.getElementById("t1").innerText = r.t1 + " °C";
            document.getElementById("t2").innerText = r.t2 + " °C";
        } finally {
            loading = false;
        }
    }
}
setInterval(response,2000);
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.clearWriteError();
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    digitalWrite(2, HIGH);
    delay(499);
    digitalWrite(2, LOW);
    delay(499);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200,"text/html",htmlPage);
  });

  server.on("/temps", HTTP_GET, [](AsyncWebServerRequest *request){

    String json="{";
    json+="\"t1\":"+String(sensor1.getTempCByIndex(0)) + ",";
    json+="\"t2\":"+String(sensor2.getTempCByIndex(0));
    json+="}";

    request->send(200,"application/json",json);
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){

    request->send(200, "text/plian", "resetted");
    ESP.restart();
  });
  server.begin();
  sensor1.begin();
  sensor2.begin();
  digitalWrite(2, HIGH);
}
  

void loop() {
  sensor1.requestTemperatures();
  sensor2.requestTemperatures();
  Serial.println(String(sensor1.getTempCByIndex(0)) + ", " + String(sensor2.getTempCByIndex(0)));
  delay(2000);
}