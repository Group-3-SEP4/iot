#include "definitions.h"

#include "gtest/gtest.h"
#include "../fff/fff.h"

DEFINE_FFF_GLOBALS

extern "C" {
	#include <ATMEGA_FreeRTOS.h>
	#include <semphr.h>
	#include <serial.h>
	#include "wrapper_semaphore.h"
	#include "wrapper_eventGroup.h"
	#include "wrapper_task.h"
		
	#include <avr/eeprom.h>
	#include "configuration.h"

	#include <mh_z19.h>
	#include <task.h>
	#include "co2_sensor.h"
}

/* Semaphore fakes */
FAKE_VALUE_FUNC(QueueHandle_t, _xSemaphoreCreateMutex);
FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreTake, SemaphoreHandle_t, TickType_t);
FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreGive, SemaphoreHandle_t);


/* eeprom fakes */
FAKE_VALUE_FUNC(uint16_t, eeprom_read_word, uint16_t*);
FAKE_VOID_FUNC(eeprom_write_word, uint16_t*, uint16_t);


/* Eventgroup fakes */
FAKE_VALUE_FUNC(BaseType_t, _xEventGroupGetBits, EventGroupHandle_t);
FAKE_VALUE_FUNC(BaseType_t, _xEventGroupClearBits, EventGroupHandle_t, EventBits_t);
FAKE_VALUE_FUNC(BaseType_t, _xEventGroupSetBits, EventGroupHandle_t, EventBits_t);

/* fake task func */
FAKE_VALUE_FUNC(BaseType_t, _xTaskCreate, TaskFunction_t, const char*, configSTACK_DEPTH_TYPE, void *, UBaseType_t, TaskHandle_t*);
FAKE_VALUE_FUNC(TickType_t, _xTaskGetTickCount);
FAKE_VOID_FUNC(_vTaskDelayUntil, TickType_t*, TickType_t);
FAKE_VOID_FUNC(_vTaskDelay, TickType_t);


/* Fake driver */
typedef void(*mh_z19_callBack)(uint16_t ppm); // callbacks required by driver
FAKE_VOID_FUNC(mh_z19_create, serial_comPort_t, mh_z19_callBack);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);


class configuration_test : public ::testing::Test {
public:

	void SetUp() override {
		RESET_FAKE(_xSemaphoreCreateMutex)
			RESET_FAKE(_xSemaphoreGive)
			RESET_FAKE(_xSemaphoreTake)

			RESET_FAKE(eeprom_read_word)
			RESET_FAKE(eeprom_write_word)

		
			FFF_RESET_HISTORY()
	}
	void TearDown() override {}
};




TEST_F(configuration_test, createNotNull) {

	//Arrange

	//Act
	configuration_t conf = configuration_create();

	//Assert
	ASSERT_NE(conf, nullptr);

}

TEST_F(configuration_test, createCallEepromAtCreate) {

	//Arrange

	//Act
	configuration_t conf = configuration_create();

	//Assert
	ASSERT_EQ(3, eeprom_read_word_fake.call_count);

}



TEST_F(configuration_test, getStoredValues) {

	//Arrange
	uint16_t returnValues[3] = { 20, 800, 1000 };
	SET_RETURN_SEQ(eeprom_read_word, returnValues, 3);
	_xSemaphoreTake_fake.return_val = pdTRUE;


	//Act
	configuration_t conf = configuration_create();
	uint16_t temp = configuration_getDefaultTemperatur(conf);
	uint16_t min = configuration_getMinCo2(conf);
	uint16_t max = configuration_getMaxCo2(conf);

	//Assert
	EXPECT_EQ(3, _xSemaphoreTake_fake.call_count);
	EXPECT_EQ(1, _xSemaphoreCreateMutex_fake.call_count);
	ASSERT_EQ(returnValues[0], temp);
	ASSERT_EQ(returnValues[1], min);
	ASSERT_EQ(returnValues[2], max);


}

TEST_F(configuration_test, validateCorrectMemoryReadLocationOnCreate) {

	//Arrange

	//Act
	configuration_t conf = configuration_create();


	//Assert
	ASSERT_EQ(DEF_MEMLOC_TEMP, eeprom_read_word_fake.arg0_history[0]);
	ASSERT_EQ(DEF_MEMLOC_CO2_MIN, eeprom_read_word_fake.arg0_history[1]);
	ASSERT_EQ(DEF_MEMLOC_CO2_MAX, eeprom_read_word_fake.arg0_history[2]);

}


class co2_test : public ::testing::Test {
public:

	void SetUp() override {
		RESET_FAKE(_xSemaphoreCreateMutex)
			RESET_FAKE(_xSemaphoreGive)
			RESET_FAKE(_xSemaphoreTake)

			RESET_FAKE(_xEventGroupGetBits)
			RESET_FAKE(_xEventGroupClearBits)
			RESET_FAKE(_xEventGroupSetBits)

			RESET_FAKE(_xTaskCreate)
			RESET_FAKE(_xTaskGetTickCount)
			RESET_FAKE(_vTaskDelayUntil)
			RESET_FAKE(_vTaskDelay)


			RESET_FAKE(mh_z19_create)
			RESET_FAKE(mh_z19_takeMeassuring)
			RESET_FAKE(mh_z19_getCo2Ppm)

			FFF_RESET_HISTORY()
	}
	void TearDown() override {}
};


TEST_F(co2_test, CanInitializeWithCreateFunction) {

	// Arrange
	// Act
	co2_sensor_t sensor = co2_create(NULL, NULL);

	// Assert
	ASSERT_NE(sensor, nullptr);

}


TEST_F(co2_test, CreateInitialize_mhz19_driver) {

	// Arrange
	// Act
	co2_sensor_t sensor = co2_create(NULL, NULL);

	// Assert
	ASSERT_EQ(1, mh_z19_create_fake.call_count);

}

TEST_F(co2_test, CreateInitialize_mhz19_driver_with_argument) {

	// Arrange

	// Act
	co2_sensor_t sensor = co2_create(NULL, NULL);

	// Assert
	ASSERT_EQ(DEF_IO_PORT_CO2, mh_z19_create_fake.arg0_val);

}


TEST_F(co2_test, TestName) {

	// Arrange

	// Act

	// Assert

}