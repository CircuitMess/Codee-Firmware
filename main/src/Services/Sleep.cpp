#include <esp_sleep.h>
#include "Sleep.h"
#include "Util/stdafx.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "ChirpSystem.h"
#include "LEDService.h"
#include "Pins.hpp"
#include "Devices/Battery.h"
#include "BacklightBrightness.h"

Sleep::Sleep() : Threaded("Sleep", 3 * 1024, 5, 1), evts(6){
	Events::listen(Facility::Input, &evts);

	activity = millis();
	start();
}

void Sleep::loop(){
	auto settings = (Settings*) Services.get(Service::Settings);
	const auto set = settings->get();
	if(set.sleepTime == 0 || set.sleepTime >= Settings::SleepSteps){
		vTaskDelay(10000);
		activity = millis();
		return;
	}

	const uint64_t sleepMillis = Settings::SleepSeconds[set.sleepTime] * 1000;
	const auto timeSinceAct = millis() - activity;
	if(timeSinceAct > sleepMillis){ // shouldn't happen
		activity = millis();
		return;
	}

	const auto waitTime = sleepMillis - timeSinceAct;

	Event evt;
	if(!evts.get(evt, waitTime)){
		if(millis() - activity >= sleepMillis){
			goSleep();
			return;
		}else{
			return;
		}
	}

	free(evt.data);
	activity = millis();
}

void Sleep::off(){
	extern void shutdown();
	shutdown();
}

void Sleep::goSleep(){
	auto chirp = (ChirpSystem*)Services.get(Service::Audio);
	chirp->stop();
	auto led = (LEDService*)Services.get(Service::LED);
	for(uint8_t i = 0; i < (uint8_t)LED::COUNT; i++){
		led->off((LED)i);
	}

	gpio_wakeup_enable((gpio_num_t) BTN_LEFT, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_RIGHT, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_A, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_B, GPIO_INTR_LOW_LEVEL);
	esp_sleep_enable_gpio_wakeup();

	auto battery = (Battery*)Services.get(Service::Battery);
	auto perc = std::max(battery->getPerc(), (uint8_t) 10);
	esp_sleep_enable_timer_wakeup((172800000000 / 100) * perc); // 2 days

	auto bl = (BacklightBrightness*)Services.get(Service::Backlight);
	bl->fadeOut();
	esp_light_sleep_start();


	auto cause = esp_sleep_get_wakeup_cause();
	if(cause == ESP_SLEEP_WAKEUP_TIMER){
		off();
	}

	bl->fadeIn();
}
