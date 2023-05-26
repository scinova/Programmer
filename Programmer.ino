//#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPAsyncWebServer.h>
#include <ESP_AVRISP.h>
#include "UpdateServer.h"

AsyncWebServer webServer(80);
#define ISP_PORT 328
#define ISP_CLK_PIN 25
#define ISP_MISO_PIN 26
#define ISP_MOSI_PIN 27
#define ISP_RESET_PIN 33

ESP_AVRISP avrprog(ISP_PORT, ISP_CLK_PIN, ISP_MISO_PIN, ISP_MOSI_PIN, ISP_RESET_PIN);
WiFiServer tcpserver;

WiFiClient client;
#define BUFFER_SIZE 1024
uint8_t buff[BUFFER_SIZE];

void serveNotFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "not found");
}

void setup() {
	Serial2.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	avrprog.setReset(false);
	IPAddress address(192, 168, 1, 1);
	IPAddress gateway(192, 168, 1, 1);
	IPAddress subnet(255, 255, 255, 0);
	WiFi.softAP("Tesla", "");
	WiFi.softAPConfig(address, gateway, subnet);
	webServer.onNotFound(serveNotFound);
	webServer.begin();
	UpdateServer.begin(&webServer);
	avrprog.begin();
	tcpserver = WiFiServer(400);
	tcpserver.begin();
}

void loop() {
	static AVRISPState_t last_state = AVRISP_STATE_IDLE;
	AVRISPState_t new_state = avrprog.update();
	if (last_state != new_state) {
		switch (new_state) {
			case AVRISP_STATE_IDLE: {
				//Serial.printf("[AVRISP] now idle\r\n");
				// Use the SPI bus for other purposes
				break;
			}
			case AVRISP_STATE_PENDING: {
				//Serial.printf("[AVRISP] connection pending\r\n");
				// Clean up your other purposes and prepare for programming mode
				break;
			}
			case AVRISP_STATE_ACTIVE: {
				//Serial.printf("[AVRISP] programming mode\r\n");
				// Stand by for completion
				break;
			}
		}
		last_state = new_state;
	}
	// Serve the client
	if (last_state != AVRISP_STATE_IDLE) {
		avrprog.serve();
	}
	digitalWrite(LED_BUILTIN, millis() % 1000 < 100 ? HIGH : LOW);


	if (tcpserver.hasClient()) {
		if (!client || !client.connected()) {
			if (client)
				client.stop();
			client = tcpserver.available();
		} else
			tcpserver.available().stop();
	}
	if (!client)
		return;
	int size = 0;
	while ((size = client.available())) {
		size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
		client.read(buff, size);
		//client.write(buff, size);
		Serial2.write(buff, size);
		Serial2.flush();
	}
	while ((size = Serial2.available())) {
		size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
		Serial2.readBytes(buff, size);
		client.write(buff, size);
		client.flush();
	}
	delay(10);
}
