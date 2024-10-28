#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/PWM.h"
#include "Pins.hpp"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "Periph/I2C.h"
#include "Devices/Display.h"
#include "Devices/Input.h"
#include "Devices/Battery.h"
#include "Util/Notes.h"
#include "FS/SPIFFS.h"
#include "UIThread.h"
#include "LV_Interface/LVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "LV_Interface/FSLVGL.h"
#include <esp_sleep.h>
#include <Util/stdafx.h>
#include "JigHWTest/JigHWTest.h"
#include "Periph/NVSFlash.h"
#include "Services/LEDService.h"
#include "Services/Time.h"
#include "Screens/IntroScreen.h"
#include "Screens/HatchScreen.h"
#include "Services/StatsManager.h"
#include "Util/HWVersion.h"
#include "Services/Power.h"

BacklightBrightness* bl;

void init(){

	auto nvs = new NVSFlash();
	Services.set(Service::NVS, nvs);

	auto settings = new Settings();
	Services.set(Service::Settings, settings);

	if(JigHWTest::checkJig()){
		printf("Jig\n");

		auto set = settings->get();
		set.sound = true;
		settings->set(set);

		auto test = new JigHWTest();
		test->start();
		vTaskDelete(nullptr);
	}

	if(!HWVersion::check()){
		while(true){
			vTaskDelay(1000);
			HWVersion::log();
		}
	}

	Power::resetPins();

	auto blPwm = new PWM(PIN_BL, LEDC_CHANNEL_1, true);
	blPwm->detach();
	bl = new BacklightBrightness(blPwm);
	Services.set(Service::Backlight, bl);

	auto adc1 = new ADC(ADC_UNIT_1);

	auto battery = new Battery(*adc1);
	if(battery->isShutdown()){
		Power::powerOff();
		return;
	}
	Services.set(Service::Battery, battery);

	auto i2c = new I2C(I2C_NUM_0, (gpio_num_t) I2C_SDA, (gpio_num_t) I2C_SCL);
	auto rtc = new RTC(*i2c);
	auto time = new Time(*rtc);
	Services.set(Service::Time, time);

	auto stats = new StatsManager();
	Services.set(Service::Stats, stats);

	auto power = new Power();

	auto led = new LEDService();
	Services.set(Service::LED, led);

	if(!SPIFFS::init()) return;

	auto disp = new Display();
	Services.set(Service::Display, disp);

	auto buzzPwm = new PWM(PIN_BUZZ, LEDC_CHANNEL_0, true);
	auto audio = new ChirpSystem(*buzzPwm);
	Services.set(Service::Audio, audio);

	auto input = new Input(true);
	Services.set(Service::Input, input);

	auto lvgl = new LVGL(*disp);
	auto lvInput = new InputLVGL();
	auto lvFS = new FSLVGL('S');

	auto gamer = new GameRunner(*disp);

	lvFS->loadCache();

	auto ui = new UIThread(*lvgl, *gamer, *lvFS, *power);
	Services.set(Service::UI, ui);

	ui->startScreen([](){ return std::make_unique<IntroScreen>(); });
	ui->start();
	delayMillis(LV_DEF_REFR_PERIOD);
	bl->fadeIn();


	// Start Battery scanning after everything else, otherwise Critical
	// Battery event might come while initialization is still in progress
	battery->begin();

	input->begin();
}

extern "C" void app_main(void){
	init();

	vTaskDelete(nullptr);
}
