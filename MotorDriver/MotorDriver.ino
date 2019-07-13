#include "ESP32_TB6612.h"

int M1_AIN1 = 14;
int M1_AIN2 = 12;
//int M1_PWM = 34;
int M1_PWM = 32;
int M_STBY = 27;
int M2_BIN1 = 26;
int M2_BIN2 = 25;
//int M2_PWM = 35;
int M2_PWM = 33;


//const int freq = 1000;
//const int M1Ch = 0;
//const int M2Ch = 1;
//const int pwmResolution = 12; //Resolution 8, 10, 12, 15

Motor motor1 = Motor(1, M1_AIN1, M1_AIN2, M1_PWM, 1, M_STBY);
Motor motor2 = Motor(2, M2_BIN1, M2_BIN2, M2_PWM, -1, M_STBY);

void setup() {
	pinMode(M1_PWM, OUTPUT);
	pinMode(M2_PWM, OUTPUT);
	Serial.begin(115200);
	//ledcSetup(M1Ch, freq, pwmResolution);
	//ledcSetup(M2Ch, freq, pwmResolution);
	//ledcAttachPin(M1_PWM, M1Ch);
	//ledcAttachPin(M2_PWM, M2Ch);
	//pinMode(M1_AIN1, OUTPUT);
	//pinMode(M1_AIN2, OUTPUT);
	//pinMode(M_STBY, OUTPUT);
	//pinMode(M2_BIN1, OUTPUT);
	//pinMode(M2_BIN2, OUTPUT);
	pinMode(2, OUTPUT);
}

void loop() {

	digitalWrite(2, HIGH);
	Serial.println("Forward");
	forward(motor1, motor2, 255);
	delay(3000);
	digitalWrite(2, LOW);
	brake(motor1, motor2);
	delay(1000);

	Serial.println("Backward");
	digitalWrite(2, HIGH);
	back(motor1, motor2, 255);
	delay(3000);
	digitalWrite(2, LOW);
	brake(motor1, motor2);
	delay(1000);

	Serial.println("Turn Left");
	digitalWrite(2, HIGH);
	left(motor1, motor2, 255);
	delay(3000);
	digitalWrite(2, LOW);
	brake(motor1, motor2);
	delay(1000);

	Serial.println("Turn Right");
	digitalWrite(2, HIGH);
	right(motor1, motor2, 255);
	delay(3000);
	digitalWrite(2, LOW);
	brake(motor1, motor2);
	delay(1000);

}