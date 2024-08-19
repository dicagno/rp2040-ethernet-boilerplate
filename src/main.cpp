#include <SPI.h>
#include <Ethernet.h>
#include <aWOT.h>
#include <ArduinoJson.h>

const int CS_PIN = 17;
const int RESET_PIN = 20;

byte mac[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

EthernetClient client;
EthernetServer server(80);
Application app;

void handleApi(Request &req, Response &res)
{
  JsonDocument doc;
  char jsonOutput[128];

  doc["source"] = "gnss";
  doc["time"] = 1351824920;
  doc["lng"][0] = 0;
  doc["lat"][1] = 0;

  serializeJson(doc, jsonOutput, sizeof(jsonOutput));

  res.set("Content-Type", "application/json");
  res.print(jsonOutput);
}

void setup()
{
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);

  Serial.begin(115200);
  while (!Serial)
  {
    ; // Needed for native USB port
  }

  Ethernet.init(CS_PIN);
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    IPAddress ip(192, 168, 1, 254); // fallback
    Ethernet.begin(mac, ip);
  }

  server.begin();

  app.get("/api", handleApi);

  delay(1000);

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  // non-blocking code
  EthernetClient cli = server.available();

  if (cli.connected())
  {
    app.process(&cli);
    cli.stop();
  }
}
