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


	static const gpio_num_t WakeupGpios[] = { (gpio_num_t) BTN_A, (gpio_num_t) BTN_B, (gpio_num_t) BTN_C, (gpio_num_t) BTN_D };
	for(const auto& pin: WakeupGpios){
		gpio_sleep_sel_en(pin);
		gpio_sleep_set_pull_mode(pin, GPIO_PULLUP_ONLY);
		gpio_wakeup_enable(pin, GPIO_INTR_LOW_LEVEL);
	}
	esp_sleep_enable_gpio_wakeup();

	auto battery = (Battery*)Services.get(Service::Battery);
	auto perc = std::max(battery->getPerc(), (uint8_t) 10);
	esp_sleep_enable_timer_wakeup((172800000000 / (uint64_t) 100) * (uint64_t) perc); // 2 days max, shorter if battery is lower

	auto bl = (BacklightBrightness*)Services.get(Service::Backlight);
	bl->fadeOut();

	static const uint8_t HeldPins[] = { LED_G, LED_Y, LED_O, LED_R, PIN_BUZZ };

	for(const auto& pin:HeldPins){
		gpio_set_level((gpio_num_t) pin, 0);
		gpio_hold_en((gpio_num_t) pin);
	}
	gpio_set_level((gpio_num_t) PIN_BL, 1);
	gpio_hold_en((gpio_num_t) PIN_BL);

	esp_light_sleep_start();

	for(const auto& pin:HeldPins){
		gpio_hold_dis((gpio_num_t) pin);
	}
	gpio_hold_dis((gpio_num_t) PIN_BL);


	auto cause = esp_sleep_get_wakeup_cause();
	if(cause == ESP_SLEEP_WAKEUP_TIMER){
		off();
	}

	bl->fadeIn();
}
