#include "Stabilizer.h"
#include "GlobalVariables.h"
#include "ESP32_TB6612.h"

#define M1_AIN1 14
#define M1_AIN2 12
#define M1_PWM 32
#define M_STBY 27
#define M2_BIN1 26
#define M2_BIN2 25
#define M2_PWM 33

bool stabilzerOn = false;
stabilizer_t stabilizerPID;
unsigned long lastTime;
unsigned long currentTime;
uint16_t dt;
unsigned long failsafeCounter;
bool rcOnline;
int MotorPWM[2];

float error, errorAngle;
float delta;
float gyroTemp;
float lastGyro[3];
float delta1[3], delta2[3];
float errorAngleI;
float errorGyroI[3];
float PTerm, ITerm, DTerm;
float axisPID[3];
float rc;
float deltaDeg;
float lastDeg;
float deltaDeg1, deltaDeg2;

#define PIDMIX(X,Y,Z) axisPID[0]*X + axisPID[1]*Y + axisPID[2]*Z
Motor motor1 = Motor(1, M1_AIN1, M1_AIN2, M1_PWM, 1, M_STBY);
Motor motor2 = Motor(2, M2_BIN1, M2_BIN2, M2_PWM, -1, M_STBY);

void initStabilizer() {
	stabilzerOn = false;
	rcOnline = false;
	errorAngleI = 0;
	errorGyroI[0] = 0;
	errorGyroI[1] = 0;
	errorGyroI[2] = 0;
	error = 0;

	lastGyro[0] = 0;
	delta2[0] = 0;
	delta1[0] = 0;
	lastGyro[1] = 0;
	delta2[1] = 0;
	delta1[1] = 0;
	lastGyro[2] = 0;
	delta2[2] = 0;
	delta1[2] = 0;

	lastDeg = 0;
	deltaDeg1 = 0;;
	deltaDeg2 = 0;
}

void executeStabilizer(uint32_t now) {
	currentTime = now;
	if (lastTime == 0)
		dt = 0;
	else
		dt = currentTime - lastTime;
	lastTime = currentTime;
	if (!stabilzerOn) {
		errorAngleI = 0;
		errorGyroI[0] = 0;
		errorGyroI[1] = 0;
		errorGyroI[2] = 0;
		error = 0;

		lastGyro[0] = 0;
		delta2[0] = 0;
		delta1[0] = 0;
		lastGyro[1] = 0;
		delta2[1] = 0;
		delta1[1] = 0;
		lastGyro[2] = 0;
		delta2[2] = 0;
		delta1[2] = 0;

		lastDeg = 0;
		deltaDeg1 = 0;;
		deltaDeg2 = 0;

		MotorPWM[0] = 0;
		MotorPWM[1] = 0;

		motor1.standby();
		motor2.standby();
		return;
	}
	if (rcOnline && failsafeCounter > 250) {
		rcCmd.FWD_BCK = 0;
		rcCmd.LFT_RGT = 0;
		failsafeCounter = 0;
		rcOnline = false;
	}
	else if (rcOnline) {
		failsafeCounter++;
	}
	else {
		rcCmd.FWD_BCK = 0;
		rcCmd.LFT_RGT = 0;
	}

	// pitch 
	rc = rcCmd.FWD_BCK * 0.1f;
	deltaDeg = allData.YPR[1] - lastDeg;
	deltaDeg = deltaDeg * dt / 1000.0f;
	/*Serial.print("RC Pitch: ");
	Serial.print(rc);*/
	errorAngle = rc - allData.YPR[1];
	PTerm = errorAngle * stabilizerPID.angle[0];

	errorAngleI = constrain(errorAngleI + errorAngle, -90.0f, +90.0f);
	ITerm = errorAngleI * stabilizerPID.angle[1];
	DTerm = (deltaDeg1 + deltaDeg2 + deltaDeg) / 3;
	DTerm = DTerm * stabilizerPID.angle[2];

	lastDeg = allData.YPR[1];
	deltaDeg2 = deltaDeg1;
	deltaDeg1 = deltaDeg;
	rc = (PTerm + ITerm - DTerm);
	/*Serial.print(", RC PID: ");
	Serial.print(rc);*/

	gyroTemp = allData.imuData.gyro[1]; // y axis
	error = rc - gyroTemp;
	delta = gyroTemp - lastGyro[1];
	delta = delta * dt / 1000.0f;

	PTerm = error * stabilizerPID.pitch[0]; // P

	errorGyroI[1] = constrain(errorGyroI[1] + error, -500.0f, +500.0f); //max 500 deg/s;
	ITerm = errorGyroI[1] * stabilizerPID.pitch[1]; //I;

	DTerm = (delta1[1] + delta2[1] + delta) / 3;
	DTerm = DTerm * stabilizerPID.pitch[2]; //D;

	lastGyro[1] = gyroTemp;
	delta2[1] = delta1[1];
	delta1[1] = delta;
	axisPID[1] = (PTerm + ITerm - DTerm);

	// yaw
	rc = rcCmd.LFT_RGT;
	gyroTemp = allData.imuData.gyro[2]; // z axis
	error = rc - gyroTemp;
	delta = gyroTemp - lastGyro[2];
	delta = delta * dt / 1000.0f;

	PTerm = error * stabilizerPID.yaw[0]; // P

	errorGyroI[2] = constrain(errorGyroI[2] + error, -500.0f, +500.0f); //max 500 deg/s;
	ITerm = errorGyroI[2] * stabilizerPID.yaw[1]; //I;

	DTerm = (delta1[2] + delta2[2] + delta) / 3;
	DTerm = DTerm * stabilizerPID.yaw[2]; //D;

	lastGyro[2] = gyroTemp;
	delta2[2] = delta1[2];
	delta1[2] = delta;
	axisPID[2] = (PTerm + ITerm - DTerm);



	// drive motor
	MotorPWM[0] = constrain(PIDMIX(0, -1, -1), -4095, 4095);
	MotorPWM[1] = constrain(PIDMIX(0, -1, 1), -4095, 4095);

	motor1.drive(MotorPWM[0]);
	motor2.drive(MotorPWM[1]);

	//Serial.print(", Pitch PID: ");
	//Serial.print(axisPID[1]);
	//Serial.print(", Yaw  PID: ");
	//Serial.println(axisPID[2]);


	/*Serial.print("M1: ");
	Serial.print(m1Val);
	Serial.print(", M2: ");
	Serial.println(m2Val);*/
}