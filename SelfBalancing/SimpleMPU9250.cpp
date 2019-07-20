#include "SimpleMPU9250.h"
#include "I2Cdev.h"

void SimpleMPU9250::initialize()
{
	//device reset
	I2Cdev::writeBit(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_DEVICE_RESET_BIT, true);
	delay(50);

	//setSleepEnabled
	I2Cdev::writeBit(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_SLEEP_BIT, false);
	//setCycleEnabled
	I2Cdev::writeBit(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_CYCLE_BIT, false);
	//setTempDisEnabled
	I2Cdev::writeBit(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_TEMP_DIS_BIT, false);
	//setClockSource
	I2Cdev::writeBits(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_PWR_MGMT_1, MPU9250_PWR1_CLKSEL_BIT, MPU9250_PWR1_CLKSEL_LENGTH, MPU9250_CLOCK_PLL_ZGYRO);


	// DLPF 0=>256Hz, 1=>188Hz, 2=>98Hz, 3=>42Hz, 4=>20Hz, 5=>10Hz, 6=>5Hz
	I2Cdev::writeByte(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_CONFIG, 0x02);
	//setFullScaleGyroRange
	I2Cdev::writeBits(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_GYRO_CONFIG, MPU9250_GCONFIG_FS_SEL_BIT, MPU9250_GCONFIG_FS_SEL_LENGTH, MPU9250_GYRO_FS_2000);

	//setFullScaleAccelRange
	I2Cdev::writeBits(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_ACCEL_CONFIG, MPU9250_ACONFIG_AFS_SEL_BIT, MPU9250_ACONFIG_AFS_SEL_LENGTH, MPU9250_ACCEL_FS_4);


}

void SimpleMPU9250::getRawMotion6(int16_t*  data) {
	I2Cdev::readBytes(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_ACCEL_XOUT_H, 14, buffer);
	data[0] = (((int16_t)buffer[0]) << 8) | buffer[1];
	data[1] = (((int16_t)buffer[2]) << 8) | buffer[3];
	data[2] = (((int16_t)buffer[4]) << 8) | buffer[5];
	data[3] = (((int16_t)buffer[8]) << 8) | buffer[9];
	data[4] = (((int16_t)buffer[10]) << 8) | buffer[11];
	data[5] = (((int16_t)buffer[12]) << 8) | buffer[13];
}
void SimpleMPU9250::getRawCompass(int16_t*  data) {
	//writeMaskedRegister(MPU9250_INT_PIN_CFG, MPU9250_BYPASS_EN_MASK, enabled);
	//I2Cdev::readByte(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_INT_PIN_CFG, buffer);
	//uint8_t masked_value = (MPU9250_BYPASS_EN_MASK & true);
	//uint8_t regvalue = buffer[0];
	//regvalue = regvalue & ~MPU9250_BYPASS_EN_MASK; // Zero Mask bits
	//regvalue = regvalue | masked_value; //Set Mask value
	//I2Cdev::writeByte(MPU9250_DEFAULT_ADDRESS, MPU9250_RA_INT_PIN_CFG, regvalue);
	//I2Cdev::writeByte(MPU9250_RA_MAG_ADDRESS, MPU9250_RA_MAG_CNTL, 0x01);
	I2Cdev::readBytes(MPU9250_RA_MAG_ADDRESS, MPU9250_RA_MAG_XOUT_L, 6, buffer);
	data[0] = ((((int16_t)buffer[1]) << 8) | buffer[0]) / 100.f;
	data[1] = ((((int16_t)buffer[3]) << 8) | buffer[2]) / 100.f;
	data[2] = ((((int16_t)buffer[5]) << 8) | buffer[4]) / 100.f;
}