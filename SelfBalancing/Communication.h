#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#ifndef _Communication_H
#define _Communication_H



// Get Data Message Code
// Message Table Mapping
#define MSG_IMU_DATA_ALL			11
#define MSG_IMU_DATA_GYRO			12
#define MSG_IMU_DATA_ACC			13
#define MSG_IMU_DATA_YPR			14

#define MSG_MOTOR_ENCODER			21
#define MSG_MOTOR_OUTPUT			22

#define MSG_CAL_GYRO_OFFSET			31
#define MSG_CAL_ACC_MIN_MAX			32
#define MSG_CAL_MAG					33
#define MSG_CAL_PID					34

#define MSG_SET_CAL_GYRO_OFFSET		41
#define MSG_SET_CAL_ACC_MIN_MAX		42
#define MSG_SET_CAL_MAG				43
#define MSG_SET_CAL_PID				44

#define MSG_STABILZE_STATE			51
#define MSG_SET_STABILZE_STATE		52


#define MSG_SET_RC					101

#define	MSG_LOAD_CONFIG				221
#define	MSG_RESET_CONFIG			222
#define	MSG_SAVE_CONFIG				223
#define MSG_RESET					255

void executeCommunication(AsyncWebSocketClient * client, uint8_t *data, size_t len);
#endif
