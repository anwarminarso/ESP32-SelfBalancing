#include "Arduino.h"
#include "DCM.h"
//#include "VectorMath.h"


void Vector_Add(float out[3], const float v1[3], const float v2[3])
{
	for (int c = 0; c < 3; c++)
	{
		out[c] = v1[c] + v2[c];
	}
}
// Multiply two 3x3 matrices: out = a * b
// out has to different from a and b (no in-place)!
void Matrix_Multiply(const float a[3][3], const float b[3][3], float out[3][3])
{
	for (int x = 0; x < 3; x++)  // rows
	{
		for (int y = 0; y < 3; y++)  // columns
		{
			out[x][y] = a[x][0] * b[0][y] + a[x][1] * b[1][y] + a[x][2] * b[2][y];
		}
	}
}
// Computes the dot product of two vectors
float Vector_Dot_Product(const float v1[3], const float v2[3])
{
	float result = 0;

	for (int c = 0; c < 3; c++)
	{
		result += v1[c] * v2[c];
	}

	return result;
}

// Multiply the vector by a scalar
void Vector_Scale(float out[3], const float v[3], float scale)
{
	for (int c = 0; c < 3; c++)
	{
		out[c] = v[c] * scale;
	}
}
// Computes the cross product of two vectors
// out has to different from v1 and v2 (no in-place)!
void Vector_Cross_Product(float out[3], const float v1[3], const float v2[3])
{
	out[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	out[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	out[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}


void DCM::Update(imu_t imuData) {
	timestamp_old = timestamp;
	timestamp = millis();
	if (timestamp > timestamp_old)
		G_Dt = (float)(timestamp - timestamp_old) / 1000.0f; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
	else G_Dt = 0;

	// Matix Update
	Gyro_Vector[0] = radians(imuData.gyro[0]); //gyro x roll
	Gyro_Vector[1] = radians(imuData.gyro[1]); //gyro y pitch
	Gyro_Vector[2] = radians(imuData.gyro[2]); //gyro z yaw

	Accel_Vector[0] = imuData.accel[0];
	Accel_Vector[1] = imuData.accel[1];
	Accel_Vector[2] = imuData.accel[2];


	Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
	Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term


	Update_Matrix[0][0] = 0;
	Update_Matrix[0][1] = -G_Dt * Omega_Vector[2];//-z
	Update_Matrix[0][2] = G_Dt * Omega_Vector[1];//y
	Update_Matrix[1][0] = G_Dt * Omega_Vector[2];//z
	Update_Matrix[1][1] = 0;
	Update_Matrix[1][2] = -G_Dt * Omega_Vector[0];//-x
	Update_Matrix[2][0] = -G_Dt * Omega_Vector[1];//-y
	Update_Matrix[2][1] = G_Dt * Omega_Vector[0];//x
	Update_Matrix[2][2] = 0;

	Matrix_Multiply(DCM_Matrix, Update_Matrix, Temporary_Matrix); //a*b=c

	for (int x = 0; x < 3; x++) //Matrix Addition (update)
	{
		for (int y = 0; y < 3; y++)
		{
			DCM_Matrix[x][y] += Temporary_Matrix[x][y];
		}
	}


	//// Normalize

	float error = 0;
	float temporary[3][3];
	float renorm = 0;

	error = -Vector_Dot_Product(&DCM_Matrix[0][0], &DCM_Matrix[1][0])*.5; //eq.19

	Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
	Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19

	Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
	Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19

	Vector_Cross_Product(&temporary[2][0], &temporary[0][0], &temporary[1][0]); // c= a x b //eq.20

	renorm = .5 *(3 - Vector_Dot_Product(&temporary[0][0], &temporary[0][0])); //eq.21
	Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);

	renorm = .5 *(3 - Vector_Dot_Product(&temporary[1][0], &temporary[1][0])); //eq.21
	Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);

	renorm = .5 *(3 - Vector_Dot_Product(&temporary[2][0], &temporary[2][0])); //eq.21
	Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);


	// Drift Correction
		//float mag_heading_x;
		//float mag_heading_y;
	float errorCourse;
	//Compensation the Roll, Pitch and Yaw drift. 
	static float Scaled_Omega_P[3];
	static float Scaled_Omega_I[3];
	float Accel_magnitude;
	float Accel_weight;


	//*****Roll and Pitch***************

	// Calculate the magnitude of the accelerometer vector
	Accel_magnitude = sqrt(Accel_Vector[0] * Accel_Vector[0] + Accel_Vector[1] * Accel_Vector[1] + Accel_Vector[2] * Accel_Vector[2]);

	// Dynamic weighting of accelerometer info (reliability filter)
	// Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
	Accel_weight = constrain(1 - 2 * abs(1 - Accel_magnitude), 0, 1);  //  

	Vector_Cross_Product(&errorRollPitch[0], &Accel_Vector[0], &DCM_Matrix[2][0]); //adjust the ground of reference
	Vector_Scale(&Omega_P[0], &errorRollPitch[0], Kp_ROLLPITCH*Accel_weight);

	Vector_Scale(&Scaled_Omega_I[0], &errorRollPitch[0], Ki_ROLLPITCH*Accel_weight);
	Vector_Add(Omega_I, Omega_I, Scaled_Omega_I);

	////*****YAW***************
	//// We make the gyro YAW drift correction based on compass magnetic heading

	////mag_heading_x = cos(MAG_Heading);
	////mag_heading_y = sin(MAG_Heading);
	////errorCourse = (DCM_Matrix[0][0] * mag_heading_y) - (DCM_Matrix[1][0] * mag_heading_x);  //Calculating YAW error
	////Vector_Scale(errorYaw, &DCM_Matrix[2][0], errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.

	////Vector_Scale(&Scaled_Omega_P[0], &errorYaw[0], Kp_YAW);//.01proportional of YAW.
	////Vector_Add(Omega_P, Omega_P, Scaled_Omega_P);//Adding  Proportional.

	////Vector_Scale(&Scaled_Omega_I[0], &errorYaw[0], Ki_YAW);//.00001Integrator
	////Vector_Add(Omega_I, Omega_I, Scaled_Omega_I);//adding integrator to the Omega_I

	pitch = -asin(DCM_Matrix[2][0]);
	roll = atan2(DCM_Matrix[2][1], DCM_Matrix[2][2]);
	yaw = atan2(DCM_Matrix[1][0], DCM_Matrix[0][0]);
}

void DCM::getYawPitchRoll(float *data) {
	data[0] = degrees(yaw);
	data[1] = degrees(pitch);
	data[2] = degrees(roll);
}