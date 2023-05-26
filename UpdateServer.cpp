#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "UpdateServer.h"

UpdateServerClass UpdateServer;

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
	//Serial.println("handleUpload");
	//Serial.println(index);
	//Serial.println(len);
	//Serial.println(final);
	if (!index) {
		if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
			Serial.println("can't start");
			Serial.flush();
			request->send(400, "text/plain", "can't start");
		}
	}
	if (len) {
		if (Update.write(data, len) != len) {
			Serial.println("can't write");
			Serial.flush();
			request->send(400, "text/plain", "can't write");
		}
	}
	if (final) {
		if (!Update.end(true)) {
			Serial.println("can't end");
			Serial.flush();
			request->send(400, "text/plain", "can't end");
		}
	}
	return;
}

void handlePost(AsyncWebServerRequest *request) {
	//Serial.println("handlePost");
	AsyncWebServerResponse *response;
	if (Update.hasError())
		response = request->beginResponse(500, "text/plain", "error\n");
	else
		response = request->beginResponse(200, "text/plain", "ok\n");
	response->addHeader("Connection", "close");
	request->send(response);
	Serial.println(Update.hasError() ? "error" : "ok");
	Serial.println("restarting ..");
	Serial.flush();
	delay(3000);
	ESP.restart();
}

#define ESP_RESET_PIN 14
#define ESP_D0_PIN 12

void handleEspProgram(AsyncWebServerRequest *request) {
	digitalWrite(ESP_D0_PIN, LOW);
	digitalWrite(ESP_RESET_PIN, HIGH);
	delay(10);
	digitalWrite(ESP_RESET_PIN, LOW);
	delay(50);
	digitalWrite(ESP_RESET_PIN, HIGH);
	request->send(200, "text/plain", "ok\n");
}

void handleEspReset(AsyncWebServerRequest *request) {
	digitalWrite(ESP_RESET_PIN, HIGH);
	digitalWrite(ESP_D0_PIN, HIGH);
	delay(10);
	digitalWrite(ESP_RESET_PIN, LOW);
	delay(50);
	digitalWrite(ESP_RESET_PIN, HIGH);
	request->send(200, "text/plain", "ok\n");
}

void UpdateServerClass::begin(AsyncWebServer *server) {
	//self
	server->on("/update", HTTP_POST, handlePost, handleUpload);
	//esp
	server->on("/esp/program", HTTP_GET, handleEspProgram);
	server->on("/esp/reset", HTTP_GET, handleEspReset);

	pinMode(ESP_RESET_PIN, OUTPUT);
	pinMode(ESP_D0_PIN, OUTPUT);
	digitalWrite(ESP_RESET_PIN, HIGH);
	digitalWrite(ESP_D0_PIN, HIGH);
}
