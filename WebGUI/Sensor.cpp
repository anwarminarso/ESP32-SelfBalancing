
#include "GlobalVariables.h"
#include "Wire.h"
#include "SimpleMPU9250.h"
#include "MotorEncoder.h"
#include "Sensor.h"


// Global Variable redefinition
raw_imu_t rawData;
DCM dcm;
all_data_t allData;

SimpleMPU9250 mpu;

float accelMinMax[6] = { -8104.0f, -8282.0f, -8562.0f, 8328.0f, 8166.0f, 8188.0f };
float gyroOffset[3] = { 62.29268293f, 17.48780488f, 11.19512195f };
float accelOffset[3];
float accelScale[3];
float accelLSB = 8192.0f; // +4g => 8192 LSB/g
float gyroLSB = 16.4f; // 2000 deg/s => 16.4 LSB/(deg/s)
int16_t rawSensor[6];


#define IMU_ACC_ORIENTATION(X, Y, Z)  {rawData.accel[0]  = X; rawData.accel[1]  = -Y; rawData.accel[2] = Z;}
#define IMU_GYRO_ORIENTATION(X, Y, Z) {rawData.gyro[0] =  -X; rawData.gyro[1] = Y; rawData.gyro[2] = -Z;}
#define IMU_MAG_ORIENTATION(X, Y, Z)  {rawData.mag[0]  =  Y; rawData.mag[1]  =  -X; rawData.mag[2]  = Z;}


void initSensor() {
	Wire.begin();
	mpu.initialize();
	for (int i = 0; i < 3; i++)
	{
		accelOffset[i] = ((accelMinMax[i] + accelMinMax[i + 3]) / 2.0f);
		accelScale[i] = (accelLSB / (accelMinMax[i + 3] - accelOffset[i]));
	}
	initMotorEncoder();

}
void readSensor() {
	mpu.getRawMotion6(rawSensor);
	IMU_ACC_ORIENTATION(rawSensor[0], rawSensor[1], rawSensor[2]);
	IMU_GYRO_ORIENTATION(rawSensor[3], rawSensor[4], rawSensor[5]);

	for (int i = 0; i < 3; i++)
	{
		allData.imuData.accel[i] = (rawData.accel[i] - accelOffset[i]) * accelScale[i] / accelLSB;
		allData.imuData.gyro[i] = (rawData.gyro[i] - gyroOffset[i]) / gyroLSB;
	}
	dcm.Update(allData.imuData);
	dcm.getYawPitchRoll(allData.YPR);
}