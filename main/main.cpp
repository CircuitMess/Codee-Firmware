#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/PWM.h"
#include "Pins.hpp"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
//#include "Services/Sleep.h"
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
#include "driver/rtc_io.h"
#include "Services/LEDService.h"

BacklightBrightness* bl;

void shutdown(){
	bl->fadeOut();

/*	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RC_FAST, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_AUTO);
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

	//PIN_BL will be held high, since that is the last state set by bl->fadeOut()
	//Required to prevent MOSFET activation on TFT_BL with leaked current if pin is floating
	rtc_gpio_isolate((gpio_num_t)PIN_BL);*/

	esp_deep_sleep_start();
}

void init(){

	gpio_config_t cfg = {
			.pin_bit_mask = (1ULL << I2C_SDA) | (1ULL << I2C_SCL),
			.mode = GPIO_MODE_INPUT
	};
	gpio_config(&cfg);

	gpio_set_direction((gpio_num_t)I2C_SCL, GPIO_MODE_OUTPUT);
	gpio_set_level((gpio_num_t)I2C_SCL, 0);


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

	auto blPwm = new PWM(PIN_BL, LEDC_CHANNEL_1, true);
	blPwm->detach();
	bl = new BacklightBrightness(blPwm);
	Services.set(Service::Backlight, bl);

	auto battery = new Battery();
	if(battery->isShutdown()){
		shutdown();
		return;
	}
	Services.set(Service::Battery, battery);

	auto led = new LEDService();
	Services.set(Service::LED, led);

	if(!SPIFFS::init()) return;

	auto disp = new Display();
	Services.set(Service::Display, disp);

	auto splashStart = millis();

	auto buzzPwm = new PWM(PIN_BUZZ, LEDC_CHANNEL_0);
	auto audio = new ChirpSystem(*buzzPwm);
	Services.set(Service::Audio, audio);

	auto input = new Input();
	Services.set(Service::Input, input);

//	auto sleep = new Sleep();

	auto lvgl = new LVGL(*disp);
	auto lvInput = new InputLVGL();
	auto lvFS = new FSLVGL('S');

	auto gamer = new GameRunner(*disp);




	if(settings->get().sound){
		audio->play({
							Chirp { NOTE_D4, NOTE_A4, 100 },
							Chirp { 0, 0, 50 },
							Chirp { NOTE_D5, NOTE_D5, 100 },
							Chirp { 0, 0, 50 },
							Chirp { NOTE_A4, NOTE_A4, 100 },
							Chirp { 0, 0, 50 },
							Chirp { NOTE_G5, NOTE_G4, 100 },
							Chirp { 0, 0, 50 },
							Chirp { NOTE_D4, NOTE_A4, 100 },
							Chirp { NOTE_D5, NOTE_D5, 100 },
					});
	}

	lvFS->loadCache();

	auto ui = new UIThread(*lvgl, *gamer, *lvFS);
	Services.set(Service::UI, ui);

	while(millis() - splashStart < 2000){
		delayMillis(10);
	}

	bl->fadeOut();
	ui->start();
//	ui->startScreen([](){ return std::make_unique<MainMenu>(true); });
	delayMillis(200);
	bl->fadeIn();

	// Start Battery scanning after everything else, otherwise Critical
	// Battery event might come while initialization is still in progress
	battery->begin();
}

extern "C" void app_main(void){
	init();

	vTaskDelete(nullptr);
}
