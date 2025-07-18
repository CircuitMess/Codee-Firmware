#include "Power.h"
#include "Periph/PinOut.h"
#include "Util/Services.h"
#include "Services/BacklightBrightness.h"
#include "Services/LEDService.h"
#include "Devices/Input.h"
#include "Util/stdafx.h"
#include "Util/EfuseMeta.h"
#include "StatsManager.h"
#include "Time.h"
#include "UIThread.h"
#include "Screens/PetScreen/PetScreen.h"
#include "Screens/HatchScreen.h"
#include "Screens/DeathScreen.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <core/lv_global.h>

Power::Power() : evts(6){
	const auto resetReason = esp_reset_reason();
	const auto wakeCause = esp_sleep_get_wakeup_cause();
	if(resetReason == ESP_RST_DEEPSLEEP && wakeCause == ESP_SLEEP_WAKEUP_TIMER){
		sleepDeep();
		return;
	}

	Events::listen(Facility::Input, &evts);
	Events::listen(Facility::Battery, &evts);
}

void Power::loop(){
	if(stopped) return;

	if(activityTime == 0){
		activityTime = millis();
	}

	checkEvts();
	if(stopped) return;

	checkSleep();
}

void Power::checkEvts(){
	Event e{};
	if(!evts.get(e, 0)) return;

	if(e.facility == Facility::Input){
		activityTime = millis();
	}else if(e.facility == Facility::Battery){
		const auto data = (Battery::Event*) e.data;
		if(data->action == Battery::Event::LevelChange && data->level == Battery::Critical){
			stopped = true;
		}
	}

	free(e.data);
}

void Power::checkSleep(){
	const auto settings = ((Settings*) Services.get(Service::Settings))->get();
	const auto sleepSeconds = Settings::SleepSeconds[settings.sleepTime];
	if(sleepSeconds == 0) return;

	if((millis() - activityTime) / 1000 < sleepSeconds) return;

	auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
	bl->fadeOut();

//	sleepLight();
	sleepDeep();

	activityTime = millis();

	auto disp = (Display*) Services.get(Service::Display);
	disp->getCanvas().clear(0);
	disp->getCanvas().pushSprite(0, 0);

	delayMillis(LV_DEF_REFR_PERIOD);

	auto ui = (UIThread*) Services.get(Service::UI);
	auto statsMan = (StatsManager*)Services.get(Service::Stats);
	if(statsMan->isHatched()){
		if(statsMan->hasDied()){
			ui->startScreen([](){ return std::make_unique<DeathScreen>(); });
		}else{
			ui->startScreen([](){ return std::make_unique<PetScreen>(); });
		}
	}else{
		ui->startScreen([](){ return std::make_unique<HatchScreen>(); });
	}

	lv_timer_reset(lv_display_get_refr_timer(LV_GLOBAL_DEFAULT()->disp_default));
	lv_anim_refr_now();
	lv_refr_now(LV_GLOBAL_DEFAULT()->disp_default);

	delayMillis(LV_DEF_REFR_PERIOD);
	delayMillis(LV_DEF_REFR_PERIOD);

	bl->fadeIn();
}

void Power::setupWake(){
	uint64_t pinMask = 0;

	const int WakePins[] = { BTN_A, BTN_B, BTN_C, BTN_D };
	for(const auto& pin : WakePins){
		pinMask |= 1ULL << pin;

		rtc_gpio_init((gpio_num_t) pin);
		rtc_gpio_set_direction((gpio_num_t) pin, RTC_GPIO_MODE_INPUT_ONLY);
		rtc_gpio_pullup_en((gpio_num_t) pin);
		rtc_gpio_pulldown_dis((gpio_num_t) pin);
		rtc_gpio_hold_en((gpio_num_t) pin);
	}

	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
	esp_sleep_enable_ext1_wakeup_io(pinMask, ESP_EXT1_WAKEUP_ANY_LOW);
}

void Power::freePins(){
	if(auto input = (Input*) Services.get(Service::Input)){
		input->end();
	}

	if(auto led = (LEDService*) Services.get(Service::LED)){
		led->end();
	}

	const int OffPins[] = { PIN_VREF, PIN_LED };
	for(const auto& pin : OffPins){
		PinOut out(pin);
		out.off();
		gpio_hold_en((gpio_num_t) pin);
	}

	const int OnPins[] = { PIN_BL, PIN_BUZZ };
	for(const auto& pin : OnPins){
		PinOut out(pin);
		out.on();
		gpio_hold_en((gpio_num_t) pin);
	}
}

void Power::returnPins(){
	const int OffPins[] = { PIN_VREF, PIN_LED };
	for(const auto& pin : OffPins){
		gpio_hold_dis((gpio_num_t) pin);
		PinOut out(pin);
		out.off();
	}

	const int OnPins[] = { PIN_BL, PIN_BUZZ };
	for(const auto& pin : OnPins){
		gpio_hold_dis((gpio_num_t) pin);
		PinOut out(pin);
		out.on();
	}

	const int WakePins[] = { BTN_A, BTN_B, BTN_C, BTN_D };
	for(const auto& pin : WakePins){
		rtc_gpio_hold_dis((gpio_num_t) pin);
		rtc_gpio_deinit((gpio_num_t) pin);
	}

	auto input = (Input*) Services.get(Service::Input);
	input->begin();

	auto led = (LEDService*) Services.get(Service::LED);
	led->begin();
}

void Power::sleepLight(){
	freePins();

	setupWake();
	esp_sleep_enable_timer_wakeup(LightSleepTimeout);
	esp_light_sleep_start();

	auto time = (Time*) Services.get(Service::Time);
	time->updateFromRTC();

	auto stats = (StatsManager*) Services.get(Service::Stats);
	stats->syncTime();

	const auto wakeCause = esp_sleep_get_wakeup_cause();
	if(wakeCause == ESP_SLEEP_WAKEUP_TIMER){
		sleepDeep();
		return;
	}

	returnPins();
}

void Power::sleepDeep(){
	setupWake();
	if(EfuseMeta::getRev() == 0){
		esp_sleep_enable_timer_wakeup(DeepSleepWakeInterval);
	}
	powerOff();
}

void Power::powerOff(){
	auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
	delete bl;

	auto led = (LEDService*) Services.get(Service::LED);
	delete led;

	auto input = (Input*) Services.get(Service::Input);
	delete input;

	const int OffPins[] = { PIN_VREF, PIN_LED, TFT_SCK, TFT_MOSI, TFT_RST, TFT_DC };
	for(const auto& pin : OffPins){
		PinOut out(pin);
		out.off();
		gpio_hold_en((gpio_num_t) pin);
	}

	const int OnPins[] = { PIN_BL, I2C_SDA, I2C_SCL };
	for(const auto& pin : OnPins){
		PinOut out(pin);
		out.on();
		gpio_hold_en((gpio_num_t) pin);
	}

	PinOut buzzPin(PIN_BUZZ, EfuseMeta::getRev() == 0);
	buzzPin.off();

	gpio_deep_sleep_hold_en();

	// By default, esp_deep_sleep_start() powers down all RTC power domains which are not needed by the enabled wakeup sources.
	esp_deep_sleep_start();
}

void Power::resetPins(){
	const int OnPins[] = { PIN_VREF, PIN_LED, TFT_SCK, TFT_MOSI, TFT_RST, TFT_DC, PIN_BUZZ, I2C_SDA, I2C_SCL };
	for(const auto& pin : OnPins){
		gpio_hold_dis((gpio_num_t) pin);
	}

	const int WakePins[] = { BTN_A, BTN_B, BTN_C, BTN_D };
	for(const auto& pin : WakePins){
		rtc_gpio_hold_dis((gpio_num_t) pin);
		rtc_gpio_deinit((gpio_num_t) pin);
	}
}
