
#include "TypeDefStruct.h"
#include "DCM.h"

#ifndef _GlobalVariables_H
#define _GlobalVariables_H

extern raw_imu_t rawData;
extern DCM dcm;
extern all_data_t allData;
extern stabilizer_t stabilizerPID;
extern rc_cmd_t rcCmd;
extern float accelMinMax[6];
extern float gyroOffset[3];
extern volatile long M1Counter;
extern volatile long M2Counter;

extern bool rcOnline;
extern int MotorPWM[2];
extern bool stabilzerOn;
extern unsigned long failsafeCounter;

#endif