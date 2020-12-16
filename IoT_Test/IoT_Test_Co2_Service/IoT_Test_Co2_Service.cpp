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
#include "secure_print.h"

#include <mh_z19.h>
#include <task.h>
#include "co2_service.h"
}


/* fake s_print */
FAKE_VOID_FUNC4_VARARG(s_print, char*, char*, char*);

/* Semaphore fakes */
FAKE_VALUE_FUNC(QueueHandle_t, _xSemaphoreCreateMutex);
FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreTake, SemaphoreHandle_t, TickType_t);
FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreGive, SemaphoreHandle_t);


/* Eventgroup fakes */
FAKE_VALUE_FUNC(EventBits_t, _xEventGroupGetBits, EventGroupHandle_t);
FAKE_VALUE_FUNC(EventBits_t, _xEventGroupClearBits, EventGroupHandle_t, EventBits_t);
FAKE_VALUE_FUNC(EventBits_t, _xEventGroupSetBits, EventGroupHandle_t, EventBits_t);

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
	co2_service_t sensor = co2_service_create(NULL, NULL);

	// Assert
	ASSERT_NE(sensor, nullptr);

}


TEST_F(co2_test, CreateInitialize_mhz19_driver) {

	// Arrange
	// Act
	co2_service_t sensor = co2_service_create(NULL, NULL);

	// Assert
	ASSERT_EQ(1, mh_z19_create_fake.call_count);

}

TEST_F(co2_test, CreateInitialize_mhz19_driver_with_argument) {

	// Arrange

	// Act
	co2_service_t sensor = co2_service_create(NULL, NULL);

	// Assert
	ASSERT_EQ(MH_Z19_USART, mh_z19_create_fake.arg0_val);
}


TEST_F(co2_test, co2MeasureCallDriver_mh_z19) {

	// Arrange
	co2_service_t sensor = co2_service_create(NULL, NULL);
	mh_z19_takeMeassuring_fake.return_val = MHZ19_OK;
	_xSemaphoreTake_fake.return_val = pdTRUE;
	// Act
	co2_service_measure(sensor);
	// Assert
	EXPECT_EQ(1, mh_z19_takeMeassuring_fake.call_count);
	EXPECT_EQ(2, _xSemaphoreTake_fake.call_count);
	EXPECT_EQ(1, mh_z19_getCo2Ppm_fake.call_count);
	EXPECT_EQ(2, _xSemaphoreGive_fake.call_count);
	EXPECT_EQ(1, _xEventGroupGetBits_fake.call_count);
}