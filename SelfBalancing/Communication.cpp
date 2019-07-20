#include "Communication.h"
#include "GlobalVariables.h"
#include "Sensor.h"
#include "Configuration.h"

#define MSG_BUFFER_SIZE 64
static uint8_t msgCode;
static uint8_t msgBuff[MSG_BUFFER_SIZE];
static uint8_t msgBuffIndex;

void sendSerializeObject(AsyncWebSocketClient * client, uint8_t cmd, uint8_t *data, size_t len) {
	msgBuffIndex = 0;
	msgBuff[msgBuffIndex++] = cmd;
	int i = len;
	while (i--)
		msgBuff[msgBuffIndex++] = *data++;
	client->binary(msgBuff, len + 1);
}

void setDeserializeObject(uint8_t *source, uint8_t *target, size_t len) {
	for (size_t i = 0; i < len; i++)
	{
		target[i] = source[i + 1];
	}
}

void executeCommunication(AsyncWebSocketClient * client, uint8_t *data, size_t len) {
	msgCode = data[0];
	switch (msgCode)
	{
		case MSG_IMU_DATA_ALL:
			allData.motor[0] = M1Counter;
			allData.motor[1] = M2Counter;
			sendSerializeObject(client, msgCode, (uint8_t*)&allData, 44);
			break;
		case MSG_IMU_DATA_GYRO:
			sendSerializeObject(client, msgCode, (uint8_t*)&allData.imuData.gyro, 12);
			break;
		case MSG_IMU_DATA_ACC:
			sendSerializeObject(client, msgCode, (uint8_t*)&allData.imuData.accel, 12);
			break;
		case MSG_IMU_DATA_YPR:
			sendSerializeObject(client, msgCode, (uint8_t*)&allData.YPR, 12);
			break;
		case MSG_MOTOR_ENCODER:
			allData.motor[0] = M1Counter;
			allData.motor[1] = M2Counter;
			sendSerializeObject(client, msgCode, (uint8_t*)&allData.motor, 8);
			break;
		case MSG_MOTOR_OUTPUT:
			sendSerializeObject(client, msgCode, (uint8_t*)&MotorPWM, 4);
			break;
		case MSG_CAL_GYRO_OFFSET:
			sendSerializeObject(client, msgCode, (uint8_t*)&gyroOffset, 12);
			break;
		case MSG_CAL_ACC_MIN_MAX:
			sendSerializeObject(client, msgCode, (uint8_t*)&accelMinMax, 24);
			break;
		case MSG_CAL_MAG:
			break;
		case MSG_CAL_PID:
			sendSerializeObject(client, msgCode, (uint8_t*)&stabilizerPID, 36);
			break;
		case MSG_SET_CAL_GYRO_OFFSET:
			setDeserializeObject(data, (uint8_t*)&gyroOffset, 12);
			break;
		case MSG_SET_CAL_ACC_MIN_MAX:
			setDeserializeObject(data, (uint8_t*)&accelMinMax, 24);
			break;
		case MSG_SET_CAL_MAG:
			break;
		case MSG_SET_CAL_PID:
			setDeserializeObject(data, (uint8_t*)&stabilizerPID, 36);
			break;
		case MSG_STABILZE_STATE:
			sendSerializeObject(client, msgCode, (uint8_t*)&stabilzerOn, 1);
			break;
		case MSG_TOGGLE_STABILZE_STATE:
			{
				uint8_t val;
				setDeserializeObject(data, (uint8_t*)&val, 1);
				stabilzerOn = val == 1;
				if (stabilzerOn)
					resetSensor();
			}
			break;
		case MSG_SET_RC:
			rcOnline = true;
			setDeserializeObject(data, (uint8_t*)&rcCmd, 8);
			failsafeCounter = 0;
			/*Serial.print("RC: ");
			Serial.print(rcCmd.FWD_BCK);
			Serial.print(", ");
			Serial.println(rcCmd.LFT_RGT);*/
			break;
		case MSG_LOAD_CONFIG:
			loadConfig();
			break;
		case MSG_RESET_CONFIG:
			resetConfig();
			break;
		case MSG_SAVE_CONFIG:
			saveConfig();
			break;
		case MSG_RESET:
			ESP.restart();
			break;
		default:
			break;
	}
}