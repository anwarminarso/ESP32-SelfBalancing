#include "MotorEncoder.h"
#include "GlobalVariables.h"
uint8_t YellowEncoderM1 = 13;
uint8_t GreenEncoderM1 = 15;
uint8_t YellowEncoderM2 = 4;
uint8_t GreenEncoderM2 = 5;

volatile long M1Counter = 0;
volatile long M2Counter = 0;

void IRAM_ATTR M1_YellowEncoder() {
	if (digitalRead(GreenEncoderM1) == HIGH) {
		M1Counter += 1;
	}
	else {
		M1Counter -= 1;
	}
}
void IRAM_ATTR M2_YellowEncoder() {
	if (digitalRead(GreenEncoderM2) == HIGH) {
		M2Counter -= 1;
	}
	else {
		M2Counter += 1;
	}
}
void initMotorEncoder() {
	pinMode(YellowEncoderM1, INPUT);
	pinMode(GreenEncoderM1, INPUT);
	pinMode(YellowEncoderM2, INPUT);
	pinMode(GreenEncoderM2, INPUT);
	resetMotorEncoder();
	attachInterrupt(YellowEncoderM1, M1_YellowEncoder, RISING);
	attachInterrupt(YellowEncoderM2, M2_YellowEncoder, RISING);
}

void resetMotorEncoder() {
	M1Counter = 0;
	M2Counter = 0;
}
