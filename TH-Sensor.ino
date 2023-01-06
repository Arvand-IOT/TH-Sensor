#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHTesp.h"
#include "env.h"

#define DHT_pin 14
#define DHT_type DHTesp::DHT22

ESP8266WebServer server(80);

WiFiClient espClient;
DHTesp dht;
float humidity;
float temperature;

void setup()
{
    Serial.begin(115200);

    // Configure DHT
    delay(100);
    dht.setup(DHT_pin, DHT_type);

    // Connect to WiFi
    setup_wifi();

    // Setup a HTTP server
    setup_server();

    // Show initial data
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    Serial.print("Sensor Status : ");
    Serial.println(dht.getStatusString());
    Serial.print("T : ");
    Serial.println(temperature);
    Serial.print("H : ");
    Serial.println(humidity);
}

void setup_wifi()
{
    delay(10);

    Serial.print("Attempting WiFi connection ...");

    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println(" Connected");
    Serial.print("IP address : ");
    Serial.println(WiFi.localIP());
}

void setup_server()
{
    server.on("/data", handleData);
    server.begin();
    Serial.println("Server listening");
}

void loop()
{
    server.handleClient();
}

void handleData()
{
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    server.send(200, "application/json", SendJson(temperature, humidity));
}

String SendJson(float Temperature, float Humidity)
{
    String answer = "{\"temperature\":\"" + String(Temperature) + "\",\"humidity\":\"" + String(Humidity) + "\"}";
    return answer;
}
