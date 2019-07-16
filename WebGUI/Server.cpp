#include "Server.h"
#include "GlobalVariables.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define hostName       "SelfBalancingRobot"
#define localSsid      "V1RU$"       // My Local WiFi SSID
#define localPassword  "@mikochu123"  // My Local WiFi password

#define esp32SSID		"Self Balancing Robot"
#define esp32Password	"@mikochu123"

AsyncWebServer  server(80); // define web server port 80
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
	if (type == WS_EVT_CONNECT) {
		Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
		client->printf("Hello Client %u :)", client->id());
		client->ping();
	}
	else if (type == WS_EVT_DISCONNECT) {
		Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
	}
	else if (type == WS_EVT_ERROR) {
		Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	}
	else if (type == WS_EVT_PONG) {
		Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
	}
	else if (type == WS_EVT_DATA) {
		AwsFrameInfo * info = (AwsFrameInfo*)arg;
		String msg = "";
		if (info->final && info->index == 0 && info->len == len) {
			//the whole message is in a single frame and we got all of it's data
			Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)data[i];
				}
			}
			else {
				char buff[3];
				for (size_t i = 0; i < info->len; i++) {
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					msg += buff;
				}
			}
			Serial.printf("%s\n", msg.c_str());
			if (info->opcode == WS_TEXT) {
				if (msg == "YPR") {
					client->binary((uint8_t*)&YPR, 12);
				}
				else if (msg = "GYRO") {
					client->binary((uint8_t*)&imuData.gyro, 12);
				}
				else if (msg = "ACC") {
					client->binary((uint8_t*)&imuData.accel, 12);
				}
				else if (msg = "MOT") {
					long mot[2] = { M1Counter, M2Counter };
					client->binary((uint8_t*)&mot, 8);
				}
				else {
					client->text("I got your text message");
				}
			}
			else
				client->binary("I got your binary message");
		}
		else {
			//message is comprised of multiple frames or the frame is split into multiple packets
			if (info->index == 0) {
				if (info->num == 0)
					Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
				Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}

			Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < len; i++) {
					msg += (char)data[i];
				}
			}
			else {
				char buff[3];
				for (size_t i = 0; i < len; i++) {
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					msg += buff;
				}
			}
			Serial.printf("%s\n", msg.c_str());

			if ((info->index + len) == info->len) {
				Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
				if (info->final) {
					Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
					if (info->message_opcode == WS_TEXT)
						client->text("I got your text message 2");
					else
						client->binary("I got your binary message 2");
				}
			}
		}
	}

}
void registerServer() {
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("max-age=600");
	// API
	server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["ChipRevision"] = String(ESP.getChipRevision());
		doc["CpuFreqMHz"] = String(ESP.getCpuFreqMHz());
		doc["SketchMD5"] = String(ESP.getSketchMD5());
		doc["SketchSize"] = String(ESP.getSketchSize());
		doc["CycleCount"] = String(ESP.getCycleCount());
		doc["SdkVersion"] = String(ESP.getSdkVersion());
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/ypr", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["yaw"] = YPR[0];
		doc["pitch"] = YPR[1];
		doc["roll"] = YPR[2];
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/gyro", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["x"] = imuData.gyro[0];
		doc["y"] = imuData.gyro[1];
		doc["z"] = imuData.gyro[2];
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/gyroOffset", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["x"] = gyroOffset[0];
		doc["y"] = gyroOffset[1];
		doc["z"] = gyroOffset[2];
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/accMinMax", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["xMin"] = accelMinMax[0];
		doc["yMin"] = accelMinMax[1];
		doc["zMin"] = accelMinMax[2];
		doc["xMax"] = accelMinMax[3];
		doc["yMax"] = accelMinMax[4];
		doc["zMax"] = accelMinMax[5];
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/motor", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["M1"] = M1Counter;
		doc["M2"] = M2Counter;
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
	server.on("/api/acc", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument doc(1024);
		String jsonValue;
		doc["x"] = imuData.accel[0];
		doc["y"] = imuData.accel[1];
		doc["z"] = imuData.accel[2];
		serializeJson(doc, jsonValue);
		request->send(200, "application/json", jsonValue);
	});
}

void initServer() {
	Serial.begin(115200);
	if (!SPIFFS.begin()) {
		//Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPdisconnect(true);
	WiFi.setHostname(hostName);
	WiFi.begin(localSsid, localPassword);

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) {
	}

	WiFi.softAPsetHostname(hostName);
	WiFi.softAPdisconnect(false);
	WiFi.softAP(esp32SSID, esp32Password, 1, 0, 1);
	MDNS.addService("http", "tcp", 80);
	registerServer();

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	events.onConnect([](AsyncEventSourceClient *client) {
		client->send("hello!", NULL, millis(), 1000);
	});
	server.begin();

}
