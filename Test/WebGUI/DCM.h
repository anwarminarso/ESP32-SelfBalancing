
#ifndef _DCM_H_
#define _DCM_H_
#include "TypeDefStruct.h"

// DCM parameters
//#define Kp_ROLLPITCH 0.02f
#define Kp_ROLLPITCH 1.2f
#define Ki_ROLLPITCH 0.00002f
#define Kp_YAW 1.2f
#define Ki_YAW 0.00002f


class DCM {
public:
	void Update(imu_t imuData);
	void getYawPitchRoll(float *data);
private:
	float MAG_Heading;
	float Accel_Vector[3] = { 0, 0, 0 }; // Store the acceleration in a vector
	float Gyro_Vector[3] = { 0, 0, 0 }; // Store the gyros turn rate in a vector
	float Omega_Vector[3] = { 0, 0, 0 }; // Corrected Gyro_Vector data
	float Omega_P[3] = { 0, 0, 0 }; // Omega Proportional correction
	float Omega_I[3] = { 0, 0, 0 }; // Omega Integrator
	float Omega[3] = { 0, 0, 0 };
	float errorRollPitch[3] = { 0, 0, 0 };
	float errorYaw[3] = { 0, 0, 0 };
	float DCM_Matrix[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };
	float Update_Matrix[3][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} };
	float Temporary_Matrix[3][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
	// Euler angles
	float yaw;
	float pitch;
	float roll;


	// DCM timing in the main loop
	unsigned long timestamp;
	unsigned long timestamp_old;
	float G_Dt; // Integration time for DCM algorithm

};

#endif