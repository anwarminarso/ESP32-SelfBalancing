#include "GlobalVariables.h"
#include "Sensor.h"
#include "Server.h"
#include "Task.h"

void instanceTask() {
}
void endTask() {
}

void Loop_100Hz(uint32_t now) {
	readSensor();
}

void Loop_50Hz(uint32_t now) {

}
void Loop_4Hz(uint32_t now) {
	Serial.print("YPR: ");
	Serial.print(YPR[0]);
	Serial.print(", ");
	Serial.print(YPR[1]);
	Serial.print(", ");
	Serial.println(YPR[2]);
}


void setup() {
	initSensor();
	initServer();

	setInstanceTask(instanceTask);
	setEndTask(endTask);
	addTask(Loop_100Hz, "loop100", 0, 10);
	//addTask(Loop_50Hz, "loop50", 5, 20);
	//addTask(Loop_4Hz, "loop4", 5, 250);
}