#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include "env.h"

#define DHT_pin 14
#define DHT_type DHTesp::DHT22

String sensor_id = "4";
String client_id = "NodeMCU_DHT22_";

#define humidity_topic "humidity"
#define temperature_topic "temperature"

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;

void setup()
{
    Serial.begin(115200);

    // Configure DHT
    delay(100);
    dht.setup(DHT_pin, DHT_type);

    // Connect to WiFi
    setup_wifi();

    // Connect to to Broker
    client.setServer(mqtt_server, 1883);
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
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection ...");

        if (client.connect((client_id + sensor_id).c_str()))
        {
            Serial.println(" Connected");
            Serial.println("-----------------------------------------------");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Try again in 5 seconds");
            delay(5000);
        }
    }
}

// Check for obvious changes ( Bigger than 1.0 )
bool checkBound(float newValue, float prevValue, float maxDiff)
{
    return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}

long last_msg = 0;
long interval = 30000;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    unsigned long now = millis();

    if (now - last_msg > interval)
    {
        last_msg = now;

        // Read sensor data
        float newTemp = dht.getTemperature();
        float newHum = dht.getHumidity();
        Serial.print("Sensor Status : ");
        Serial.println(dht.getStatusString());

        if (checkBound(newTemp, temp, diff))
        {
            temp = newTemp;
            String tmp_msg;
            tmp_msg = sensor_id + ":" + String(temp);
            Serial.print("New Temperature : ");
            Serial.println(temp);
            client.publish(temperature_topic, tmp_msg.c_str(), true);
        }

        if (checkBound(newHum, hum, diff))
        {
            hum = newHum;
            String hum_msg;
            hum_msg = sensor_id + ":" + String(hum);
            Serial.print("New Humidity : ");
            Serial.println(hum);
            client.publish(humidity_topic, hum_msg.c_str(), true);
        }
    }
}
