#include "Stabilizer.h"
#include "GlobalVariables.h"


stabilizer_t stabilizerPID;

void initStabilizer() {
	stabilizerPID.yaw[0] = 10;
	stabilizerPID.yaw[1] = 5;
	stabilizerPID.yaw[2] = 2;


	stabilizerPID.pitch[0] = 25;
	stabilizerPID.pitch[1] = 5;
	stabilizerPID.pitch[2] = 2;

	stabilizerPID.roll[0] = 10;
	stabilizerPID.roll[1] = 8;
	stabilizerPID.roll[2] = 3;

}