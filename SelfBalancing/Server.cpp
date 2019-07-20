#include "Server.h"
#include "GlobalVariables.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Configuration.h"
#include "Sensor.h"
#include "Communication.h"

#define hostName       "SelfBalancingRobot"
#define localSsid      "V1RU$"       // My Local WiFi SSID
#define localPassword  "@mikochu123"  // My Local WiFi password

#define esp32SSID		"Self Balancing Robot"
#define esp32Password	"@mikochu123"
rc_cmd_t rcCmd;
AsyncWebServer  server(80); // define web server port 80
AsyncWebSocket ws("/ws");

//AsyncEventSource events("/events");

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
			if (info->opcode == WS_TEXT) {
				for (size_t i = 0; i < info->len; i++) {
					msg += (char)data[i];
				}
				if (msg == "YPR") {
					client->binary((uint8_t*)&allData.YPR, 12);
				}
				else if (msg == "GYRO") {
					client->binary((uint8_t*)&allData.imuData.gyro, 12);
				}
				else if (msg == "ACC") {
					client->binary((uint8_t*)&allData.imuData.accel, 12);
				}
				else if (msg == "MOT") {
					allData.motor[0] = M1Counter;
					allData.motor[1] = M2Counter;
					client->binary((uint8_t*)&allData.motor, 8);
				}
				else if (msg == "ALL") {
					allData.motor[0] = M1Counter;
					allData.motor[1] = M2Counter;
					client->binary((uint8_t*)&allData, 44);
				}
				else {
					client->text("I got your text message");
				}
			}
			else {
				if (info->len > 0) {
					executeCommunication(client, data, len);
				}
			}
		}
		else {
			// not implemented yet ..... =)

			//message is comprised of multiple frames or the frame is split into multiple packets
			/*if (info->index == 0) {
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
			}*/
		}
	}

}
void registerServer() {
	server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("max-age=600");


	// REST API
	server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["ChipRevision"] = String(ESP.getChipRevision());
		resultDoc["CpuFreqMHz"] = String(ESP.getCpuFreqMHz());
		resultDoc["SketchMD5"] = String(ESP.getSketchMD5());
		resultDoc["SketchSize"] = String(ESP.getSketchSize());
		resultDoc["CycleCount"] = String(ESP.getCycleCount());
		resultDoc["SdkVersion"] = String(ESP.getSdkVersion());
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/ypr", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["yaw"] = allData.YPR[0];
		resultDoc["pitch"] = allData.YPR[1];
		resultDoc["roll"] = allData.YPR[2];
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/gyro", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["x"] = allData.imuData.gyro[0];
		resultDoc["y"] = allData.imuData.gyro[1];
		resultDoc["z"] = allData.imuData.gyro[2];
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/gyroOffset", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/accMinMax", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["xMin"] = accelMinMax[0];
		resultDoc["yMin"] = accelMinMax[1];
		resultDoc["zMin"] = accelMinMax[2];
		resultDoc["xMax"] = accelMinMax[3];
		resultDoc["yMax"] = accelMinMax[4];
		resultDoc["zMax"] = accelMinMax[5];
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/motor", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["M1"] = M1Counter;
		resultDoc["M2"] = M2Counter;
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/acc", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["x"] = allData.imuData.accel[0];
		resultDoc["y"] = allData.imuData.accel[1];
		resultDoc["z"] = allData.imuData.accel[2];
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/cal", HTTP_GET, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		JsonObject  jsonGyro = resultDoc.createNestedObject("gyro");
		jsonGyro["x"] = gyroOffset[0];
		jsonGyro["y"] = gyroOffset[1];
		jsonGyro["z"] = gyroOffset[2];

		JsonObject  jsonAcc = resultDoc.createNestedObject("acc");
		jsonAcc["xMin"] = accelMinMax[0];
		jsonAcc["yMin"] = accelMinMax[1];
		jsonAcc["zMin"] = accelMinMax[2];
		jsonAcc["xMax"] = accelMinMax[3];
		jsonAcc["yMax"] = accelMinMax[4];
		jsonAcc["zMax"] = accelMinMax[5];

		JsonObject jsonPID = resultDoc.createNestedObject("pid");
		JsonObject jsonYawPID = jsonPID.createNestedObject("yaw");
		jsonYawPID["P"] = stabilizerPID.yaw[0];
		jsonYawPID["I"] = stabilizerPID.yaw[1];
		jsonYawPID["D"] = stabilizerPID.yaw[2];
		JsonObject jsonPitchPID = jsonPID.createNestedObject("pitch");
		jsonPitchPID["P"] = stabilizerPID.pitch[0];
		jsonPitchPID["I"] = stabilizerPID.pitch[1];
		jsonPitchPID["D"] = stabilizerPID.pitch[2];
		JsonObject jsonRollPID = jsonPID.createNestedObject("angle");
		jsonRollPID["P"] = stabilizerPID.angle[0];
		jsonRollPID["I"] = stabilizerPID.angle[1];
		jsonRollPID["D"] = stabilizerPID.angle[2];

		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/savePID", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		if (request->hasParam("pid", true)) {
			String msg = request->getParam("pid", true)->value();
			Serial.println(msg);
			DynamicJsonDocument postDoc(1024);
			DeserializationError err = deserializeJson(postDoc, msg);
			if (err != DeserializationError::Ok) {
				resultDoc["success"] = false;
				resultDoc["error"] = "error deserialize Json";
			}
			else {
				stabilizerPID.yaw[0] = postDoc["yaw"]["P"];
				stabilizerPID.yaw[1] = postDoc["yaw"]["I"];
				stabilizerPID.yaw[2] = postDoc["yaw"]["D"];
				stabilizerPID.pitch[0] = postDoc["pitch"]["P"];
				stabilizerPID.pitch[1] = postDoc["pitch"]["I"];
				stabilizerPID.pitch[2] = postDoc["pitch"]["D"];
				stabilizerPID.angle[0] = postDoc["angle"]["P"];
				stabilizerPID.angle[1] = postDoc["angle"]["I"];
				stabilizerPID.angle[2] = postDoc["angle"]["D"];

				Serial.println("PID Saved");
				resultDoc["success"] = true;
				saveConfig();
			}
		}
		else {
			resultDoc["success"] = false;
			resultDoc["error"] = "No Data";
		}
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/saveGyroOffset", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		if (request->hasParam("gyroOffset", true)) {
			String msg = request->getParam("gyroOffset", true)->value();
			Serial.println(msg);
			DynamicJsonDocument postDoc(1024);
			DeserializationError err = deserializeJson(postDoc, msg);
			if (err != DeserializationError::Ok) {
				resultDoc["success"] = false;
				resultDoc["error"] = "error deserialize Json";
			}
			else {
				gyroOffset[0] = postDoc["x"];
				gyroOffset[1] = postDoc["y"];
				gyroOffset[2] = postDoc["z"];

				Serial.println("Gyro Offset Saved");
				resultDoc["success"] = true;
				saveConfig();
			}
		}
		else {
			resultDoc["success"] = false;
			resultDoc["error"] = "No Data";
		}
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/saveAccMinMax", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		if (request->hasParam("accMinMax", true)) {
			String msg = request->getParam("accMinMax", true)->value();
			Serial.println(msg);
			DynamicJsonDocument postDoc(1024);
			DeserializationError err = deserializeJson(postDoc, msg);
			if (err != DeserializationError::Ok) {
				resultDoc["success"] = false;
				resultDoc["error"] = "error deserialize Json";
			}
			else {
				accelMinMax[0] = postDoc["min"]["x"];
				accelMinMax[1] = postDoc["min"]["y"];
				accelMinMax[2] = postDoc["min"]["z"];
				accelMinMax[3] = postDoc["max"]["x"];
				accelMinMax[4] = postDoc["max"]["y"];
				accelMinMax[5] = postDoc["max"]["z"];

				Serial.println("Acc Min Max Saved");
				resultDoc["success"] = true;
				saveConfig();
			}
		}
		else {
			resultDoc["success"] = false;
			resultDoc["error"] = "No Data";
		}
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/resetCfg", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resetConfig();
		resultDoc["success"] = true;
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
		ESP.restart();
	});
	server.on("/api/reloadCfg", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		loadConfig();
		resultDoc["success"] = true;
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
	server.on("/api/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		resultDoc["success"] = true;
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
		ESP.restart();
	});
	server.on("/api/toggleStabilizer", HTTP_POST, [](AsyncWebServerRequest *request) {
		DynamicJsonDocument resultDoc(1024);
		String resultJsonValue;
		stabilzerOn = !stabilzerOn;
		resultDoc["success"] = true;
		resultDoc["state"] = stabilzerOn ? "ON" : "OFF";
		if (stabilzerOn)
			resetSensor();
		serializeJson(resultDoc, resultJsonValue);
		request->send(200, "application/json", resultJsonValue);
	});
}

void initServer() {
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
	WiFi.softAP(esp32SSID, esp32Password, 1, 0, 4);
	MDNS.addService("http", "tcp", 80);
	registerServer();

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);

	//events.onConnect([](AsyncEventSourceClient *client) {
	//	client->send("hello!", NULL, millis(), 1000);
	//});
	server.begin();

}
