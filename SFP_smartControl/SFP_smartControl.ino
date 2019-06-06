/*
*	SMART FlowerPot
*	smartControl
*	__________________
*
*	Code by Andrew Cabalin
*/

/* ----- Libraries ----- */
#include <SFE_BMP180.h>
#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_SHT31.h>
#include <ArduinoJson.h>

/* ---- Pins ----- */
#define VCC_MOISTURE	A4
#define GND_MOISTURE	A5
#define MOISTURE		A7

#define VCC_LIGHT		A9
#define GND_LIGHT		A10
#define LIGHT			A8

#define VCC_CLOCK		19
#define GND_CLOCK		18

#define WiFi			Serial3

/* ----- Sensors ----- */
Adafruit_SHT31 sht31 = Adafruit_SHT31();
SFE_BMP180 pressure;
DS3231 clock;
RTCDateTime dt;

/* ----- Sysytem vars ----- */
boolean canTransmitt = false;
unsigned long WiFitimer = 0;
void setup()
{
	Serial.begin(9600);
	WiFi.begin(115200);

	PinsInit();
	delay(1000);

	pressure.begin();
	clock.begin();
	sht31.begin(0x44);
}

void loop()
{
	TWaction();
}

void PinsInit() {
	pinMode(VCC_CLOCK, OUTPUT);
	pinMode(GND_CLOCK, OUTPUT);
	pinMode(VCC_MOISTURE, OUTPUT);
	pinMode(GND_MOISTURE, OUTPUT);
	pinMode(VCC_LIGHT, OUTPUT);
	pinMode(GND_LIGHT, OUTPUT);

	digitalWrite(VCC_CLOCK, 1);
	digitalWrite(GND_CLOCK, 0);
	digitalWrite(VCC_MOISTURE, 1);
	digitalWrite(GND_MOISTURE, 0);
	digitalWrite(VCC_LIGHT, 1);
	digitalWrite(GND_LIGHT, 0);
}

/* -------- Data exchange with Thingworx Server  -------- */
void TWaction() {
	if (canTransmitt && millis() - WiFitimer >= 5000) {
		float temp = sht31.readTemperature();					//Read temperature and humid
		float humid = sht31.readHumidity();

		dt = clock.getDateTime();								//Read time
		double T, P;

		pressure.startTemperature();							//Read pressure
		pressure.getTemperature(T);
		pressure.startPressure(3);
		pressure.getPressure(P, T);

		WiFi.print("Temp=");
		WiFi.print(temp, 1);
		WiFi.print("&Humid=");
		WiFi.print(humid, 1);
		WiFi.print("&Pressure=");
		WiFi.print(P, 1);
		WiFi.print("&Light=");
		WiFi.print(map(analogRead(LIGHT), 0, 1024, 0, 100));
		WiFi.print("&Moisture=");
		WiFi.print(map(analogRead(MOISTURE), 0, 1024, 0, 100));
		WiFi.print("&Hours=");
		WiFi.print(dt.hour);
		WiFi.print("&Minutes=");
		WiFi.print(dt.minute);
		WiFi.print("&Seconds=");
		WiFi.print(dt.second);

		Serial.println("Sended!");
		canTransmitt = false;
		WiFitimer = millis();
	}

	String message = "";
	if (WiFi.available()) {						//Waiting for '>' symbol
		message = WiFi.readStringUntil('\n');

		if (message.startsWith(">"))
			canTransmitt = true;
		else
			Serial.println(message);
	}
}