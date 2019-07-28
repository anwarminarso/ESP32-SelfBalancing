


// 13 Yellow Encoder M1
// 15 Green Encoder M1

// 4 Yellow Encoder M2
// 5 Green Encoder M2


uint8_t YellowEncoderM1 = 13;
uint8_t GreenEncoderM1 = 15;
uint8_t YellowEncoderM2 = 4;
uint8_t GreenEncoderM2 = 5;

volatile long M1Counter = 0;
volatile long M2Counter = 0;

void setup() {
	Serial.begin(115200);
	pinMode(YellowEncoderM1, INPUT);
	pinMode(GreenEncoderM1, INPUT);
	pinMode(YellowEncoderM2, INPUT);
	pinMode(GreenEncoderM2, INPUT);
	attachInterrupt(YellowEncoderM1, M1_YellowEncoder, RISING);
	//attachInterrupt(GreenEncoderM1, M1_GreenEncoder, CHANGE);
	attachInterrupt(YellowEncoderM2, M2_YellowEncoder, RISING);
	//attachInterrupt(GreenEncoderM2, M2_GreenEncoder, CHANGE);
}


void loop() {

	Serial.print("M1: ");
	Serial.print(M1Counter);
	Serial.print(", M2: ");
	Serial.println(M2Counter);

	delay(1000);
}


void IRAM_ATTR M1_YellowEncoder() {
	if (digitalRead(GreenEncoderM1) == HIGH) {
		M1Counter += 1;
	}
	else {
		M1Counter -= 1;
	}
}
//void IRAM_ATTR M1_GreenEncoder() {
//	if (digitalRead(GreenEncoderM1) == HIGH) {
//		M1Counter -= 1;
//	}
//}
void IRAM_ATTR M2_YellowEncoder() {
	if (digitalRead(GreenEncoderM2) == HIGH) {
		M2Counter += 1;
	}
	else {
		M2Counter -= 1;
	}
}
//void IRAM_ATTR M2_GreenEncoder() {
//	if (digitalRead(GreenEncoderM2) == HIGH) {
//		M2Counter -= 1;
//	}
//}