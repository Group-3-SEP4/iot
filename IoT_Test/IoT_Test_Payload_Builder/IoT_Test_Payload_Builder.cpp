#include "definitions.h"	
#include "gtest/gtest.h"
#include "../fff/fff.h"

DEFINE_FFF_GLOBALS

extern "C" {
	#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>
#include <lora_driver.h>
#include "payload_builder.h"

#include "secure_print.h"
}


/* fake s_print */
FAKE_VOID_FUNC4_VARARG(s_print, char*, char*, char*);


class payload_builder_test : public ::testing::Test {
public:

	void SetUp() override {
		RESET_FAKE(s_print)

		FFF_RESET_HISTORY()
	}
	void TearDown() override {}
};





TEST_F(payload_builder_test, empty) {
	//Arrange
	const int16_t temperature = 0;
	const uint16_t humidity = 0;
	const uint16_t co2 = 0;
	const uint16_t servoPos = 0;

	//Act
	lora_driver_payload_t payload = payload_builder_get_payload(temperature, humidity, co2, servoPos);

	//Assert
	ASSERT_EQ(0, payload.bytes[0]);
	ASSERT_EQ(0, payload.bytes[1]);
	ASSERT_EQ(0, payload.bytes[2]);
	ASSERT_EQ(0, payload.bytes[3]);
	ASSERT_EQ(0, payload.bytes[4]);
	ASSERT_EQ(0, payload.bytes[5]);
	ASSERT_EQ(0, payload.bytes[6]);
	ASSERT_EQ(0, payload.bytes[7]);
}

TEST_F(payload_builder_test, setTemp) {
	//Arrange
	const int16_t temperature = 25;
	const uint16_t humidity = 0;
	const uint16_t co2 = 0;
	const uint16_t servoPos = 0;

	//Act
	lora_driver_payload_t payload = payload_builder_get_payload(temperature, humidity, co2, servoPos);

	uint16_t readVal = uint16_t(payload.bytes[2] << 8) | payload.bytes[3];
	//Assert
	ASSERT_EQ(temperature, readVal);
}

TEST_F(payload_builder_test, setHum) {
	//Arrange
	const int16_t temperature = 0;
	const uint16_t humidity = 50;
	const uint16_t co2 = 0;
	const uint16_t servoPos = 0;

	//Act
	lora_driver_payload_t payload = payload_builder_get_payload(temperature, humidity, co2, servoPos);

	uint16_t readVal = uint16_t(payload.bytes[0] << 8) | payload.bytes[1];
	//Assert
	ASSERT_EQ(humidity, readVal);
}

TEST_F(payload_builder_test, setCo2) {
	//Arrange
	const int16_t temperature = 0;
	const uint16_t humidity = 0;
	const uint16_t co2 = 500;
	const uint16_t servoPos = 0;

	//Act
	lora_driver_payload_t payload = payload_builder_get_payload(temperature, humidity, co2, servoPos);

	uint16_t readVal = uint16_t(payload.bytes[4] << 8) | payload.bytes[5];
	//Assert
	ASSERT_EQ(co2, readVal);
}

TEST_F(payload_builder_test, setServo) {
	//Arrange
	const int16_t temperature = 0;
	const uint16_t humidity = 0;
	const uint16_t co2 = 0;
	const uint16_t servoPos = 80;

	//Act
	lora_driver_payload_t payload = payload_builder_get_payload(temperature, humidity, co2, servoPos);

	uint16_t readVal = uint16_t(payload.bytes[6] << 8) | payload.bytes[7];
	//Assert
	ASSERT_EQ(servoPos, readVal);
}