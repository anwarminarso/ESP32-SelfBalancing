
#ifndef _TYPEDEFSTRUCT_H_
#define _TYPEDEFSTRUCT_H_


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
	float mag[3];
} imu_t;


#endif