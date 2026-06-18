#ifndef _SENSOR_H_
#define _SENSOR_H_
void initSensor();
void readSensor();
void resetSensor();
// Recompute accel offset/scale from accelMinMax. Must be called whenever
// accelMinMax changes at runtime (web update, config load/reset).
void applyAccelCalibration();

// Accelerometer min/max capture helpers.
// While capture is active, readSensor() tracks the running min/max of the
// raw (oriented, pre-calibration) accel readings on each axis. The user
// rotates the robot through the 6 orientations to populate the extremes.
void startAccelCal();              // reset running min/max and enable tracking
void stopAccelCal();               // disable tracking
const float* getAccelCalLive();    // pointer to running min/max [6] (xMin,yMin,zMin,xMax,yMax,zMax)
void applyAccelCalFromCapture();   // copy capture into accelMinMax + recompute
#endif