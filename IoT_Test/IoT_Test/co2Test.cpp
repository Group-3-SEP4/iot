//
//#include "definitions.h"
//#include "gtest/gtest.h"
//#include "../fff/fff.h"
//
//
//
//extern "C" {
//#include <ATMEGA_FreeRTOS.h>
//#include <semphr.h>
//#include "wrapper_semaphore.h"
//#include "wrapper_eventGroup.h"
//
//
//#include <serial.h>
//#include <mh_z19.h>
//#include "co2_sensor.h"
//
//}
//
//namespace
//{
//
//
//	/* Semaphore fakes */
//	FAKE_VALUE_FUNC(QueueHandle_t, _xSemaphoreCreateMutex);
//	FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreTake, SemaphoreHandle_t, TickType_t);
//	FAKE_VALUE_FUNC(BaseType_t, _xSemaphoreGive, SemaphoreHandle_t);
//
//	/* Eventgroup fakes */
//	FAKE_VALUE_FUNC(BaseType_t, _xEventGroupGetBits, EventGroupHandle_t);
//	FAKE_VALUE_FUNC(BaseType_t, _xEventGroupClearBits, EventGroupHandle_t, EventBits_t);
//	FAKE_VALUE_FUNC(BaseType_t, _xEventGroupSetBits, EventGroupHandle_t, EventBits_t);
//
//	/* Fake driver */
//	typedef void(*mh_z19_callBack)(uint16_t ppm); // callbacks required by driver
//	FAKE_VOID_FUNC(mh_z19_create, serial_comPort_t, mh_z19_callBack);
//	FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
//	FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);
//
//
//	class co2_test : public ::testing::Test {
//	protected:
//
//
//
//		void SetUp() override {
//			RESET_FAKE(_xSemaphoreCreateMutex)
//				RESET_FAKE(_xSemaphoreGive)
//				RESET_FAKE(_xSemaphoreTake)
//
//				//RESET_FAKE(_xEventGroupGetBits)
//				//RESET_FAKE(_xEventGroupClearBits)
//				//RESET_FAKE(_xEventGroupSetBits)
//
//				RESET_FAKE(mh_z19_create)
//				RESET_FAKE(mh_z19_takeMeassuring)
//				RESET_FAKE(mh_z19_getCo2Ppm)
//
//
//
//				FFF_RESET_HISTORY()
//		}
//		void TearDown() override {}
//	};
//
//
//	TEST_F(co2_test, CanInitializeWithCreateFunction) {
//
//		// Arrange
//		EventGroupHandle_t eventGroupMeasure = xEventGroupCreate();
//		EventGroupHandle_t eventGroupDataReady = xEventGroupCreate();
//		// Act
//		co2_sensor_t sensor = co2_create(eventGroupMeasure, eventGroupDataReady);
//
//		// Assert
//		ASSERT_NE(sensor, nullptr);
//
//	}
//
//
//	TEST_F(co2_test, TestName) {
//
//		// Arrange
//
//		// Act
//
//		// Assert
//
//	}
//
//}