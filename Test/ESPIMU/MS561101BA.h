// MS561101BA.h

#ifndef _MS561101BA_h
#define _MS561101BA_h

//#include <avr/pgmspace.h>
#include "I2Cdev.h"


#define MS561101BA_ADDR 0x77
#define MS561101BA_RESET 0x1E
#define MS561101BA_PROM_BASE_ADDR 0xA2 // by adding ints from 0 to 6 we can read all the prom configuration values. 

//#define MS561101BA_PROM_BASE_ADDR 0xA0 // by adding ints from 0 to 6 we can read all the prom configuration values. 
// C1 will be at 0xA2 and all the subsequent are multiples of 2
#define MS561101BA_PROM_REG_COUNT 6 // number of registers in the PROM
#define MS561101BA_PROM_REG_SIZE 2 // size in bytes of a prom registry.
// D1 and D2 result size (bytes)
#define MS561101BA_D1D2_SIZE 3
#define MS561101BA_D1 0x40
#define MS561101BA_D2 0x50

// OSR (Over Sampling Ratio) constants
//#define MS561101BA_OSR_256 0x00
//#define MS561101BA_OSR_512 0x02
//#define MS561101BA_OSR_1024 0x04
//#define MS561101BA_OSR_2048 0x06
//#define MS561101BA_OSR_4096 0x08
#define MS561101BA_OSR 0x08

#define MS561101BA_CONVERSION_TIME 10000l // conversion time in microseconds
#define MS561101BA_EXTRA_PRECISION 5 
#define MOVAVG_SIZE 11
#define MS561101BA_UPDATE_INTERVAL 25000    // 40hz update rate (20hz LPF on acc)

class MS561101BA {
    public:
		void init();
		void reset();
		uint8_t update();
		float getTemperature();
		float getPressure();
		int32_t getPressureSum();
		float getEstimatedAltitude();
		float getBaroVelocity();
		bool isBaroUpdated;
    private:
		void calculate();
		void readFactoryCalibration();
		uint32_t readConversion();
		void UT_Start();
		void UP_Start();
		void updateHistory();
		int32_t movavg_buff[MOVAVG_SIZE];
		int movavg_i;
		uint32_t presRawCache, tempRawCache;
		int32_t pressure, temperature;
		float EstAlt;
		float lastEstAlt;
		float velocity;

		int32_t pressureSum;
		uint16_t deadline;
		uint8_t buffer[4];
		uint8_t state;
		//int calb;
		uint16_t baroFactoryCalibration[MS561101BA_PROM_REG_COUNT];
};
#endif

