//#include "definitions.h"
//#include "gtest/gtest.h"
//#include "../fff/fff.h"
//
//DEFINE_FFF_GLOBALS
//
//extern "C" {
//	#include <ATMEGA_FreeRTOS.h>
//	#include <semphr.h>
//	#include "wrapper_semaphore.h"
//
//	#include <avr/eeprom.h>
//	#include "configuration.h"
//
//	}
//
//FAKE_VALUE_FUNC(QueueHandle_t, _xSemaphoreCreateMutex);
//FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreTake, SemaphoreHandle_t, TickType_t);
//FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreGive, SemaphoreHandle_t);
//
//FAKE_VALUE_FUNC(uint16_t, eeprom_read_word, uint16_t*);
//FAKE_VOID_FUNC(eeprom_write_word, uint16_t*, uint16_t);
//
//
//class configuration_test : public ::testing::Test {
//public:
//
//	void SetUp() override {
//		RESET_FAKE(_xSemaphoreCreateMutex)
//			RESET_FAKE(_xSemaphoreGive)
//			RESET_FAKE(_xSemaphoreTake)
//
//			RESET_FAKE(eeprom_read_word)
//			RESET_FAKE(eeprom_write_word)
//			FFF_RESET_HISTORY()
//	}
//	void TearDown() override {}
//};
//
//
//
//
//TEST_F(configuration_test, createNotNull) {
//
//	//Arrange
//
//	//Act
//	configuration_t conf = configuration_create();
//
//	//Assert
//	ASSERT_NE(conf, nullptr);
//
//}
//
//TEST_F(configuration_test, createCallEepromAtCreate) {
//
//	//Arrange
//
//	//Act
//	configuration_t conf = configuration_create();
//
//	//Assert
//	ASSERT_EQ(3, eeprom_read_word_fake.call_count);
//
//}
//
//
//
//TEST_F(configuration_test, getStoredValues) {
//
//	//Arrange
//	uint16_t returnValues[3] = { 20, 800, 1000 };
//	SET_RETURN_SEQ(eeprom_read_word, returnValues, 3);
//	_xSemaphoreTake_fake.return_val = pdTRUE;
//
//
//	//Act
//	configuration_t conf = configuration_create();
//	uint16_t temp = configuration_getDefaultTemperatur(conf);
//	uint16_t min = configuration_getMinCo2(conf);
//	uint16_t max = configuration_getMaxCo2(conf);
//
//	//Assert
//	EXPECT_EQ(3, _xSemaphoreTake_fake.call_count);
//	EXPECT_EQ(1, _xSemaphoreCreateMutex_fake.call_count);
//	ASSERT_EQ(returnValues[0], temp);
//	ASSERT_EQ(returnValues[1], min);
//	ASSERT_EQ(returnValues[2], max);
//
//
//}
//
//TEST_F(configuration_test, validateCorrectMemoryReadLocationOnCreate) {
//
//	//Arrange
//
//	//Act
//	configuration_t conf = configuration_create();
//
//
//	//Assert
//	ASSERT_EQ(DEF_MEMLOC_TEMP, eeprom_read_word_fake.arg0_history[0]);
//	ASSERT_EQ(DEF_MEMLOC_CO2_MIN, eeprom_read_word_fake.arg0_history[1]);
//	ASSERT_EQ(DEF_MEMLOC_CO2_MAX, eeprom_read_word_fake.arg0_history[2]);
//
//}
//
//
