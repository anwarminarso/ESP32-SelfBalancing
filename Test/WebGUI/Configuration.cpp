#include "Configuration.h"
#include "TypeDefStruct.h"
#include "GlobalVariables.h"
#include <Preferences.h>
Preferences prefs;

#define prefNamespace "MyESP"

void loadConfig() {
	bool isValid = true;
	size_t schLen;

	prefs.begin(prefNamespace, true);
	schLen = prefs.getBytesLength("stabilizerPID");
	if (schLen != sizeof(stabilizerPID))
		isValid = false;

	schLen = prefs.getBytesLength("accelMinMax");
	if (schLen != sizeof(accelMinMax))
		isValid = false;

	schLen = prefs.getBytesLength("gyroOffset");
	if (schLen != sizeof(gyroOffset))
		isValid = false;

	if (isValid) {
		prefs.getBytes("stabilizerPID", (uint8_t*)&stabilizerPID, sizeof(stabilizerPID));
		prefs.getBytes("accelMinMax", (uint8_t*)&accelMinMax, sizeof(accelMinMax));
		prefs.getBytes("gyroOffset", (uint8_t*)&gyroOffset, sizeof(gyroOffset));
	}
	prefs.end();
	if (!isValid)
		resetConfig();
	else
		Serial.println("Config Loaded");
}

void resetConfig() {
	stabilizerPID.yaw[0] = 10;
	stabilizerPID.yaw[1] = 5;
	stabilizerPID.yaw[2] = 2;

	stabilizerPID.pitch[0] = 25;
	stabilizerPID.pitch[1] = 5;
	stabilizerPID.pitch[2] = 2;

	stabilizerPID.roll[0] = 10;
	stabilizerPID.roll[1] = 8;
	stabilizerPID.roll[2] = 3;

	accelMinMax[0] = -8104.0f;
	accelMinMax[1] = -8282.0f;
	accelMinMax[2] = -8562.0f;
	accelMinMax[3] = 8328.0f;
	accelMinMax[4] = 8166.0f;
	accelMinMax[5] = 8188.0f;

	gyroOffset[0] = 62.29268293f;
	gyroOffset[1] = 17.48780488f;
	gyroOffset[2] = 11.19512195f;

	prefs.begin(prefNamespace, false);
	prefs.clear();
	prefs.putBytes("stabilizerPID", (uint8_t*)&stabilizerPID, sizeof(stabilizerPID));
	prefs.putBytes("accelMinMax", (uint8_t*)&accelMinMax, sizeof(accelMinMax));
	prefs.putBytes("gyroOffset", (uint8_t*)&gyroOffset, sizeof(gyroOffset));
	prefs.end();

	Serial.println("Reset Config");
}

void saveConfig() {
	prefs.begin(prefNamespace, false);
	prefs.putBytes("stabilizerPID", (uint8_t*)&stabilizerPID, sizeof(stabilizerPID));
	prefs.putBytes("accelMinMax", (uint8_t*)&accelMinMax, sizeof(accelMinMax));
	prefs.putBytes("gyroOffset", (uint8_t*)&gyroOffset, sizeof(gyroOffset));
	prefs.end();
	Serial.println("Config Saved");
}