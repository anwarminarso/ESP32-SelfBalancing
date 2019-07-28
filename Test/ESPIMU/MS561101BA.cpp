// 
// 
// 
#include "MS561101BA.h"
#include "Arduino.h"

#define applyDeadband(value, deadband)  \
  if(abs(value) < deadband) {           \
    value = 0;                          \
  } else if(value > 0){                 \
    value -= deadband;                  \
  } else if(value < 0){                 \
    value += deadband;                  \
  }


void MS561101BA::init() {
	lastEstAlt = -10000.f;
	velocity = 0;
	reset();
	delay(100);
	readFactoryCalibration();
	delay(10);
	UT_Start();
	deadline = micros() + MS561101BA_CONVERSION_TIME; 
}
void MS561101BA::reset() {
	I2Cdev::writeBytes(MS561101BA_ADDR, MS561101BA_RESET, 0, 0x00);
	//calb = 200;
	delay(100);
}
void MS561101BA::readFactoryCalibration() {
	for (int i=0;i< MS561101BA_PROM_REG_COUNT; i++) {
		I2Cdev::readBytes(MS561101BA_ADDR, MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE),  MS561101BA_PROM_REG_SIZE, buffer);
		baroFactoryCalibration[i] = buffer[0] << 8 | buffer[1];
	}
	for (int i = 0; i < MS561101BA_PROM_REG_COUNT; i++) {
		Serial.println(baroFactoryCalibration[i]);
	}
}
uint32_t MS561101BA::readConversion() {
	Wire.beginTransmission(MS561101BA_ADDR);
	Wire.write(0);
	Wire.endTransmission();

	Wire.beginTransmission(MS561101BA_ADDR);
	Wire.requestFrom(MS561101BA_ADDR, MS561101BA_D1D2_SIZE);
	uint32_t conversion;
	while (Wire.available() < 3) {};
	if(Wire.available()) {
		conversion = Wire.read() * 65536 + Wire.read() * 256 + Wire.read();
	}
	else {
		conversion = -1;
	}
	return conversion;
}
uint8_t MS561101BA::update() {
	if (state == 2) {               // a third state is introduced here to isolate calculate() and smooth timecycle spike
		state = 0;
		calculate();
		return 2;
	}
	unsigned long now = micros();
	if ((int16_t)(now - deadline)<0) {
		return 0;
	}
	deadline = now + MS561101BA_CONVERSION_TIME;  // UT and UP conversion take 8.5ms so we do next reading after 10ms 
	if (state == 0) {
		tempRawCache = readConversion();
		UP_Start();
		updateHistory();                              // moved here for less timecycle spike
		state = 1;
		return 1;
	} else {
		presRawCache = readConversion();
		UT_Start();
		state = 2;
		return 2;
	}
}
void MS561101BA::updateHistory() {
	uint8_t indexplus1 = (movavg_i + 1);
	if (indexplus1 == MOVAVG_SIZE)
		indexplus1 = 0;
	movavg_buff[movavg_i] = pressure;
	pressureSum += movavg_buff[movavg_i];
	pressureSum -= movavg_buff[indexplus1];
	movavg_i = indexplus1; 
}

void MS561101BA::UT_Start() {
	I2Cdev::writeBytes(MS561101BA_ADDR, MS561101BA_D2 + MS561101BA_OSR, 0, 0x00);
}
void MS561101BA::UP_Start() {
	I2Cdev::writeBytes(MS561101BA_ADDR, MS561101BA_D1 + MS561101BA_OSR, 0, 0x00);
}
void MS561101BA::calculate() {
	float dT        = (int32_t)tempRawCache - (int32_t)((uint32_t)baroFactoryCalibration[4] << 8);
	float off       = ((uint32_t)baroFactoryCalibration[1] <<16) + ((dT * baroFactoryCalibration[3]) /((uint32_t)1<<7));
	float sens      = ((uint32_t)baroFactoryCalibration[0] <<15) + ((dT * baroFactoryCalibration[2]) /((uint32_t)1<<8));
	int32_t delt	= (dT * baroFactoryCalibration[5])/((uint32_t)1<<23);
	temperature = (delt + 2000); 
	if (delt < 0) { // temperature lower than 20st.C 
		delt *= 5 * delt;
		off  -= delt>>1; 
		sens -= delt>>2;
	}
	pressure     = (((presRawCache * sens ) /((uint32_t)1<<21)) - off)/((uint32_t)1<<15);
}
float MS561101BA::getPressure() {
	return pressure / 100.f;
}
float MS561101BA::getTemperature() {
	return temperature / 100.f;
}
int32_t MS561101BA::getPressureSum() {
	return pressureSum;
}

float MS561101BA::getEstimatedAltitude() {
	float  pressureAvg;
	//int32_t  lastEstAlt;
	//static float baroGroundTemperatureScale,logBaroGroundPressureSum;
	//static float vel = 0.0f;
	//static int32_t groundAlt;
	static uint16_t previousT;
	uint16_t currentT = micros();
	uint16_t dTime;

	dTime = currentT - previousT;
	if (dTime < MS561101BA_UPDATE_INTERVAL) 
		return EstAlt;
	isBaroUpdated = true;
	previousT = currentT;
	pressureAvg = pressureSum / (MOVAVG_SIZE -1);
	//(1 - Math.pow((mb/pstd), 0.190284)) * 145366.45;
	//EstAlt = 44330.77f * (1.f - pow(pressureAvg / 101325.f, 0.19027));
	EstAlt = 44330.77f * (1.f - pow(pressureAvg / 101325.f, 0.190294957f));
	//EstAlt -= memConfig.groundAlt;
	if (lastEstAlt != -10000.f) {
		//int16_t baroVel = mul((EstAlt - lastEstAlt), (1000000 / dTime));
		//baroVel = constrain(baroVel, -300, 300);
		//applyDeadband(baroVel, 10); // to reduce noise near zero
		//velocity = -(gravityZ) * 9.80665f * ((float)dTime / 10000.0f);
		//applyDeadband(velocity, 1.5f);
	
		//// complementary filter
		//velocity = velocity * 0.985f + baroVel * 0.015f;
	}
	lastEstAlt = EstAlt;
	//nggak kepake
	//EstAlt = 145366.45f * (1.f - powf(pressureAvg / 101325.f, 0.190284));
	return EstAlt;
}
float MS561101BA::getBaroVelocity() {
	return velocity;
}