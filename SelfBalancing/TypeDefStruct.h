#include "Arduino.h"
#ifndef _TYPEDEFSTRUCT_H_
#define _TYPEDEFSTRUCT_H_

typedef void(*taskFunction)(uint32_t now);
typedef void(*instanceTaskFunction)();
typedef void(*endTaskFunction)();
struct taskItem {
	taskFunction fPtr;
	uint32_t next;
	uint32_t recur;
	char itemName[8];
};

typedef struct {
	// [0] => P, [1] => I, [2] => D
	float yaw[3];
	float pitch[3];
	float angle[3];
} stabilizer_t;

typedef struct
{
	int16_t gyro[3];
	int16_t accel[3];
	int16_t mag[3];
} raw_imu_t;
typedef struct
{
	float gyro[3];
	float accel[3];
	//float mag[3];
} imu_t;

typedef struct
{
	imu_t imuData;
	float YPR[3];
	long motor[2];
	//float mag[3];
} all_data_t;

typedef struct {
	float FWD_BCK;
	float LFT_RGT;
} rc_cmd_t;

#endif