#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include "Adafruit_SHT4x.h"

#include "vent_boardv1.h"

#define WIFI_SSID   "Gonmar-Livebox"
#define WIFI_PASS   "618995151609549464"
#define MQTT_IP     "192.168.1.100"
#define FW_VERSION  "0.1.0"
#define DEVICE_NAME "Vent Guille Arduino"
#define VENT_NAME   "Vent Guille"
#define VENT_ID     "vent_guille"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
HANumber servo(VENT_ID);

/* Global variables ***********************************************************/
/* Function prototypes ********************************************************/

static void servo_cmd_cb(HANumeric number, HANumber* sender);

/* Callbacks ******************************************************************/

static void servo_cmd_cb(HANumeric number, HANumber* sender)
{

	if (!number.isSet()) {
		// the reset command was send by Home Assistant
	} else {
			int8_t num_int8 = number.toInt8();
			Serial.print("Number: ");
			Serial.println(num_int8);
			// Map from 0-100 to 0-255?
			analogWrite(PIN_SERVO, num_int8);
	}
	
	sender->setState(number); // report the selected option back to the HA panel
}

/* Function definitions *******************************************************/
/* Setup and loop functions ***************************************************/

void setup()
{
	Serial.begin(9600);
	while (!Serial) {
		delay(10);
	}

	Serial.println("Home Assistant Arduino Vent");

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500); // Waiting for the connection
	}
	Serial.println("Connected!");
	
	byte mac[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(mac);
	device.setUniqueId(mac, sizeof(mac));
	device.setName(DEVICE_NAME);
	device.setSoftwareVersion(FW_VERSION);

	pinMode(PIN_SERVO, OUTPUT);

	servo.setName(VENT_NAME);
	servo.onCommand(servo_cmd_cb);
	servo.setIcon("mdi:hvac");

	mqtt.begin(MQTT_IP);
}

void loop()
{
	mqtt.loop();
}