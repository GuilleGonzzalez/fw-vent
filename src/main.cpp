#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <Servo.h> 

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
HANumber vent(VENT_ID);
Servo servo;

/* Global variables ***********************************************************/
/* Function prototypes ********************************************************/

static void servo_cmd_cb(HANumeric number, HANumber* sender);

/* Callbacks ******************************************************************/

static void servo_cmd_cb(HANumeric number, HANumber* sender)
{

	if (!number.isSet()) {
		// The reset command was send by Home Assistant
	} else {
			int8_t num = number.toInt8();
			int angle = map(num, -100, 100, 0, 120); 
			servo.write(angle);
			Serial.print("Number: ");
			Serial.print(num);
			Serial.print(", angle: ");
			Serial.println(angle);
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

	servo.attach(PIN_SERVO);

	vent.setName(VENT_NAME);
	vent.onCommand(servo_cmd_cb);
	vent.setIcon("mdi:hvac");
	vent.setMin(-100);
	vent.setMax(100);
	vent.setStep(10);
	vent.setUnitOfMeasurement("%");

	mqtt.begin(MQTT_IP);
}

void loop()
{
	mqtt.loop();
}