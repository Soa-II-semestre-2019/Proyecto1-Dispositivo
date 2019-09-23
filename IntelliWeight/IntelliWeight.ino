#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <PubSubClient.h>

#include <HX711.h>

//Sensor 1
HX711 scale_1;

#define calibration_factor_1 -19000.0 //This value is obtained using the HX711_Calibration sketch
#define DOUT_1  3
#define CLK_1  2

//Sensor 2
HX711 scale_2;

#define calibration_factor_2 -19000.0 //This value is obtained using the HX711_Calibration sketch
#define DOUT_2  5
#define CLK_2  4

//Configuration for WiFi
#define WIFI_AP "<SSID>"  //Add SSID
#define WIFI_PASSWORD "<PASS>" //Add PASSWORD

//Configuration for MQTT
WiFiEspClient espClient;
#define mqtt_server "3.83.223.148" //soldier.cloudmqtt.com
#define mqtt_port 13115
#define mqtt_user "<USER>"
#define mqtt_password "<PASS>"
#define weight_topic_1 "/board_1/weight_1"
#define weight_topic_2 "/board_1/weight_2"

PubSubClient client(mqtt_server, mqtt_port, espClient);

#define soft Serial1 //Serial for communication with ESP8266_01

int status = WL_IDLE_STATUS;


void setup()
{

  Serial.begin(115200);

  InitSensors();
  InitWiFi();

  client.setServer(mqtt_server, mqtt_port);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float weight_1 = scale_1.get_units(10);
  client.publish(weight_topic_1, String(weight_1).c_str(), true);   // Publish weight on weight_topic

  //float weight_2 = scale_2.get_units(10);
  //client.publish(weight_topic_2, String(weight_2).c_str(), true);   // Publish weight on weight_topic

  delay(5000);
}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(115200);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    InitWiFi();
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void InitSensors()
{

  scale_1.begin(DOUT_1, CLK_1);
  scale_1.set_scale(calibration_factor_1);
  scale_1.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  scale_2.begin(DOUT_2, CLK_2);
  scale_2.set_scale(calibration_factor_2);
  scale_2.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
    
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    String clientId = "IntelliWeight";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}
