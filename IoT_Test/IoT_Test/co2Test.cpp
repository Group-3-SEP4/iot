#include "gtest/gtest.h"
#include "../fff/fff.h"
DEFINE_FFF_GLOBALS


extern "C" {
#include <FreeRTOS.h>
#include <mh_z19.h>
#include <serial.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include <wrapper_eventGroup.h>
#include <wrapper_semaphore.h>
#include <wrapper_task.h>
#include <co2_sensor.h>
}

typedef void (*mh_z19_callback)(uint16_t ppm);

FAKE_VOID_FUNC(mh_z19_create, serial_comPort_t, mh_z19_callback);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_takeMeassuring);
FAKE_VALUE_FUNC(mh_z19_returnCode_t, mh_z19_getCo2Ppm, uint16_t*);


class co2_test : public ::testing::Test {
protected:
	void SetUp() override {
		RESET_FAKE(mh_z19_create);
		RESET_FAKE(mh_z19_takeMeassuring)
		RESET_FAKE(mh_z19_getCo2Ppm);
		FFF_RESET_HISTORY();
	}
	void TearDown() override{}
};


TEST(co2_Test, TestName) {
	//arrange
	EventGroupHandle_t e1 = xEventGroupCreate();
	EventGroupHandle_t e2 = xEventGroupCreate();

	co2_sensor_t _sensor = co2_create(e1, e2);
	mh_z19_getCo2Ppm_fake.return_val = MHZ19_OK;
	//act
	co2_getMeasurement(_sensor);
	//assert

  //EXPECT_EQ(1, 1);
  //EXPECT_TRUE(true);
}           