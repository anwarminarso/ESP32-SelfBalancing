
#include "I2Cdev.h"
//#include "helper_3dmath.h"
//#include "MS561101BA.h"
#include "SimpleMPU9250.h"
#include "TypeDefStruct.h"
//#include "VectorMath.h"
#include "DCM.h"


SimpleMPU9250 mpu;
//MS561101BA baro;

raw_imu_t rawData;
imu_t imuData;
DCM dcm;

float accelMinMax[6] = { -8104.0f, -8282.0f, -8562.0f, 8328.0f, 8166.0f, 8188.0f };
float gyroOffset[3] = { 62.29268293f, 17.48780488f, 11.19512195f };
float accelOffset[3];
float accelScale[3];
float accelLSB = 8192.0f; // +4g => 8192 LSB/g
float gyroLSB = 16.4f; // 2000 deg/s => 16.4 LSB/(deg/s)
float YPR[3] = {0, 0, 0};

#define IMU_ACC_ORIENTATION(X, Y, Z)  {rawData.accel[0]  = X; rawData.accel[1]  = -Y; rawData.accel[2] = Z;}
#define IMU_GYRO_ORIENTATION(X, Y, Z) {rawData.gyro[0] =  -X; rawData.gyro[1] = Y; rawData.gyro[2] = -Z;}
#define IMU_MAG_ORIENTATION(X, Y, Z)  {rawData.mag[0]  =  Y; rawData.mag[1]  =  -X; rawData.mag[2]  = Z;}


void setup() {
	Serial.begin(115200);
	Wire.begin();
	delay(1000);
	//baro.init();
	mpu.initialize();

	for (int i = 0; i < 3; i++)
	{
		accelOffset[i] = ((accelMinMax[i] + accelMinMax[i + 3]) / 2.0f);
		accelScale[i] = (accelLSB / (accelMinMax[i + 3] - accelOffset[i]));
	}

}

void loop() {
	//baro.update();

	int16_t rawSensor[6];
	mpu.getRawMotion6(rawSensor);
	IMU_ACC_ORIENTATION(rawSensor[0], rawSensor[1], rawSensor[2]);
	IMU_GYRO_ORIENTATION(rawSensor[3], rawSensor[4], rawSensor[5]);

	for (int i = 0; i < 3; i++)
	{
		imuData.accel[i] = (rawData.accel[i] - accelOffset[i]) * accelScale[i] / accelLSB;
	
		imuData.gyro[i] = (rawData.gyro[i] - gyroOffset[i]) / gyroLSB;
	}
	dcm.Update(imuData);
	dcm.getYawPitchRoll(YPR);
	//mpu.getRawCompass(&mag[0], &mag[1], &mag[2]);

	//float altitude = baro.getEstimatedAltitude();
	//float pressure = baro.getPressure();
	//float temperature = baro.getTemperature();
	//float velocity = baro.getBaroVelocity();


	//Serial.print("A: ");
	//Serial.print(altitude);
	//Serial.print(", P: ");
	//Serial.print(pressure);
	//Serial.print(", T: ");
	//Serial.print(temperature);
	//Serial.print(", V: ");
	//Serial.println(velocity);

	//Serial.print("RAW Acc: ");
	//Serial.print(rawData.accel[0]);
	//Serial.print(";");
	//Serial.print(rawData.accel[1]);
	//Serial.print(";");
	//Serial.println(rawData.accel[2]);

	//Serial.print("RAW Gyro: ");
	//Serial.print(rawData.gyro[0]);
	//Serial.print(", ");
	//Serial.print(rawData.gyro[1]);
	//Serial.print(", ");
	//Serial.println(rawData.gyro[2]);


	/*Serial.print("Acc: ");
	Serial.print(imuData.accel[0]);
	Serial.print(";");
	Serial.print(imuData.accel[1]);
	Serial.print(";");
	Serial.println(imuData.accel[2]);

	Serial.print("Gyro: ");
	Serial.print(imuData.gyro[0]);
	Serial.print(", ");
	Serial.print(imuData.gyro[1]);
	Serial.print(", ");
	Serial.println(imuData.gyro[2]);*/

	Serial.print("YPR: ");
	Serial.print(YPR[0]);
	Serial.print(", ");
	Serial.print(YPR[1]);
	Serial.print(", ");
	Serial.println(YPR[2]);

	/*Serial.print("Mag: ");
	Serial.print(mag[0]);
	Serial.print(", ");
	Serial.print(mag[1]);
	Serial.print(", ");
	Serial.println(mag[2]);*/
	delay(50);
}