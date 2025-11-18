#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define RELAY_PIN D0 

const char* ssid = "YourWifiSSID";
const char* password = "YourWifiPASSWORD";


// --- Time UTC+7 ---
const long utcOffsetInSeconds = 7 * 3600; 
const char* ntpServer = "pool.ntp.org";

const int ON_HOUR = 12;
const int ON_MINUTE = 0;

const int OFF_HOUR = 11;
const int OFF_MINUTE = 0;

// Init Server & NTP
ESP8266WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds, 60000);

bool ledState = false; 

const int HTML_BUFFER_SIZE = 1500; 
char htmlBuffer[HTML_BUFFER_SIZE];

String getWebPage() {
    String stateText = ledState ? "ON" : "OFF";
    String stateClass = ledState ? "status-on" : "status-off";
    String currentTime = timeClient.getFormattedTime();
    
    String html = ""; 
    html += R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="10"> 
    <title>Smart Scheduler</title>
    <style>
        :root {
            --primary-color: #007bff;
            --on-color: #28a745;
            --off-color: #dc3545;
            --bg-light: #f4f7f9;
            --text-dark: #343a40;
        }

        body { 
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; 
            display: flex; 
            justify-content: center; 
            align-items: center; 
            min-height: 100vh; 
            margin: 0; 
            background-color: var(--bg-light); 
            color: var(--text-dark); 
        }

        .container { 
            background-color: #ffffff; 
            padding: 35px; 
            border-radius: 15px; 
            box-shadow: 0 10px 30px rgba(0,0,0,0.1), 0 0 0 1px rgba(0,0,0,0.05); /* Shadow dan border halus */
            text-align: center; 
            max-width: 400px; 
            width: 90%; 
            transition: transform 0.3s ease-in-out;
        }
        .container:hover {
            transform: translateY(-3px);
        }

        h1 { 
            color: var(--primary-color); 
            margin-bottom: 5px; 
            font-size: 2em; 
            font-weight: 600;
        }
        .schedule { 
            color: #6c757d; 
            margin-bottom: 25px; 
            font-size: 0.9em; 
            border-bottom: 1px dashed #eee;
            padding-bottom: 15px;
        }

        /* Status Display */
        .status-display { 
            padding: 30px 20px; 
            border-radius: 15px; 
            font-size: 1.8em; 
            font-weight: 700; 
            margin-bottom: 30px; 
            transition: all 0.5s ease;
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        
        /* Status ON Style */
        .status-on { 
            background: linear-gradient(135deg, #d4edda, var(--on-color)); 
            color: white; 
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);
        }
        
        /* Status OFF Style */
        .status-off { 
            background: linear-gradient(135deg, #f8d7da, var(--off-color)); 
            color: white; 
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);
        }

        /* Time Box */
        .time-box { 
            background-color: var(--bg-light); 
            padding: 12px; 
            border-radius: 8px; 
            margin-top: 20px; 
            font-size: 1em; 
            border: 1px solid #dee2e6; 
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .time-box span { 
            font-weight: bold; 
            color: var(--primary-color); 
            font-size: 1.2em;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Lamp Scheduler</h1>
)rawliteral";
    sprintf(htmlBuffer, "<div class=\"schedule\">OFF at %02d:%02d --- ON at %02d:%02d</div>", 
            OFF_HOUR, OFF_MINUTE, ON_HOUR, ON_MINUTE);
    html += String(htmlBuffer);

    sprintf(htmlBuffer, "<div class=\"status-display %s\">%s</div>", 
            stateClass.c_str(), stateText.c_str());
    html += String(htmlBuffer); 
    
    sprintf(htmlBuffer, "<div class=\"time-box\"><span>Server Time</span><span>%s</span></div>", 
            currentTime.c_str());
    html += String(htmlBuffer);

    html += R"rawliteral(
        <div class="footer" style="margin-top: 25px; font-size: 0.8em; color: #adb5bd;">
            <p>Refreshed every 10s.</p>
        </div>
    </div>
</body>
</html>
)rawliteral";
    
    return html;
}

void handleRoot() {
  server.send(200, "text/html", getWebPage());
}

void checkScheduler() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  long currentTimeInMinutes = currentHour * 60 + currentMinute;
  long onTimeInMinutes = ON_HOUR * 60 + ON_MINUTE;
  long offTimeInMinutes = OFF_HOUR * 60 + OFF_MINUTE;
  bool shouldBeOn = false;
  if (ON_HOUR < OFF_HOUR) {
    if (currentTimeInMinutes >= onTimeInMinutes && currentTimeInMinutes < offTimeInMinutes) {
        shouldBeOn = true;
    } else {
        shouldBeOn = false;
    }
  }
  else {
    if (currentHour == ON_HOUR && currentMinute == ON_MINUTE) {
        shouldBeOn = true;
    }
    else if (currentHour == OFF_HOUR && currentMinute == OFF_MINUTE) {
        shouldBeOn = false;
    }
    else {
        shouldBeOn = ledState; 
    }
  }
  Serial.print(timeClient.getFormattedTime());
  Serial.print(" WIB | ShouldBeOn: "); Serial.print(shouldBeOn ? "TRUE" : "FALSE");
  Serial.print(" | CurrentState: "); Serial.println(ledState ? "ON" : "OFF");

  if (shouldBeOn != ledState) {
    if (shouldBeOn) {
      digitalWrite(RELAY_PIN, LOW);
      ledState = true;
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      ledState = false;
    }
  }
  
  if (timeClient.getSeconds() < 2 && currentMinute % 2 != 0) {
      Serial.print("Time Check (WIB): ");
      Serial.println(timeClient.getFormattedTime());
  }
  delay(1000); 
}


void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  ledState = false;

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke "); Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.update(); 
  Serial.print("Time UTC+7: "); Serial.println(timeClient.getFormattedTime());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Server HTTP dimulai.");
}

void loop() {
  server.handleClient();
  checkScheduler();
  delay(100); 
}