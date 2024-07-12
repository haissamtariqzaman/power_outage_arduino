#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.c"
#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80);
MDNSResponder mdns;

String webPage = "";

int led_pin = 13;

void print_board_info()
{
  Serial.println("");
  Serial.println("ESP8266 board info:");
  Serial.print("\tChip ID: ");
  Serial.println(ESP.getFlashChipId());
  Serial.print("\tCore Version: ");
  Serial.println(ESP.getCoreVersion());
  Serial.print("\tChip Real Size: ");
  Serial.println(ESP.getFlashChipRealSize());
  Serial.print("\tChip Flash Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("\tChip Flash Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("\tChip Speed: ");
  Serial.println(ESP.getCpuFreqMHz());
  Serial.print("\tChip Mode: ");
  Serial.println(ESP.getFlashChipMode());
  Serial.print("\tSketch Size: ");
  Serial.println(ESP.getSketchSize());
  Serial.print("\tSketch Free Space: ");
  Serial.println(ESP.getFreeSketchSpace());
}

void create_web_page()
{
  webPage += "<h1>ESP8266 Web Server</h1>";
  webPage += "<p>Chip ID: ";
  webPage += ESP.getFlashChipId();
  webPage += "</p>";
  webPage += "<p>Core Version: ";
  webPage += ESP.getCoreVersion();
  webPage += "</p>";
  webPage += "<p>Chip Real Size: ";
  webPage += ESP.getFlashChipRealSize() / 1024;
  webPage += " Kbit</p>";
  webPage += "<p>Chip Size: ";
  webPage += ESP.getFlashChipSize() / 1024;
  webPage += " Kbit</p>";
  webPage += "<p>Chip Flash Speed: ";
  webPage += ESP.getFlashChipSpeed() / 1000000;
  webPage += " MHz</p>";
  webPage += "<p>Chip Work Speed: ";
  webPage += ESP.getCpuFreqMHz();
  webPage += " MHz</p>";
  webPage += "<p>Chip Mode: ";
  webPage += ESP.getFlashChipMode();
  webPage += "</p>";
  webPage += "<p>LED state <a href=\"LedON\"><button>ON</button></a>&nbsp;<a href=\"LedOFF\"><button>OFF</button></a></p>";
}

void connect_to_wifi()
{
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to "); //
  Serial.println(ssid);
  Serial.print("IP address: "); //
  Serial.println(WiFi.localIP());
}

void define_APIs()
{
  // APIS:
  server.on("/", []()
            { server.send(200, "text/html", webPage); });

  server.on("/LedON", []()
            {
    server.send(200, "text/html", webPage);
    digitalWrite(led_pin, HIGH);
    Serial.println("[ON]");
    delay(1000); });

  server.on("/LedOFF", []()
            {
    server.send(200, "text/html", webPage);
    digitalWrite(led_pin, LOW);
    Serial.println("[OFF]");
    delay(1000); });
}

void hit_api()
{
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://www.google.com/"); // Specify the URL
  int httpCode = http.GET();                    // Make the request

  if (httpCode > 0)
  {
    String payload = http.getString(); // Get the response payload
    Serial.println(payload);
    Serial.println("http code is: " + httpCode); // Print the response payload
  }
  else
  {
    Serial.println("Error on HTTP request");
  }

  http.end(); // Free resources
}

void setup(void)
{
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  print_board_info();

  create_web_page();

  connect_to_wifi();

  // Begin MDNS Responder
  if (mdns.begin("esp8266", WiFi.localIP()))
  {
    Serial.println("MDNS responder started");
  }

  define_APIs();

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("before hit_api");
  hit_api();
  Serial.println("after hit_api");
}

void loop(void)
{
  server.handleClient();
}
