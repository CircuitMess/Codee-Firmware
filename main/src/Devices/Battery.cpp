#include "Battery.h"
#include "../Pins.hpp"
#include "Util/Events.h"
#include <soc/efuse_reg.h>
#include <Util/stdafx.h>
#include <cmath>
#include <driver/gpio.h>

#define MAX_READ 3820 // 4.5V
#define MIN_READ 2870 // 3.6V

Battery::Battery() : SleepyThreaded(MeasureIntverval, "Battery", 3 * 1024, 5, 1), adc((gpio_num_t) PIN_BATT, 0.05, MIN_READ, MAX_READ, getVoltOffset()),
					 hysteresis({ 0, 4, 15, 30, 50, 70, 90, 100 }, 3){

	sample(true);
}

void Battery::begin(){
	start();
}

uint16_t Battery::mapRawReading(uint16_t reading){
	return std::round(map((double) reading, MIN_READ, MAX_READ, 3600, 4500));
}

int16_t Battery::getVoltOffset(){
	int16_t upper = 0, lower = 0;
	esp_efuse_read_field_blob((const esp_efuse_desc_t**) efuse_adc1_low, &lower, 8);
	esp_efuse_read_field_blob((const esp_efuse_desc_t**) efuse_adc1_high, &upper, 8);
	return (upper << 8) | lower;
}

void Battery::sample(bool fresh){
	if(shutdown) return;

	auto oldLevel = getLevel();

	if(fresh){
		adc.resetEma();
		hysteresis.reset(adc.getVal());
	}else{
		auto val = adc.sample();
		hysteresis.update(val);
	}

	if(oldLevel != getLevel() || fresh){
		Events::post(Facility::Battery, Battery::Event{ .action = Event::LevelChange, .level = getLevel() });
	}

	if(getLevel() == Critical){
		stop(0);
		shutdown = true;
		return;
	}
}

void Battery::sleepyLoop(){
	if(shutdown) return;
	sample();
}

uint8_t Battery::getPerc() const{
	return adc.getVal();
}

Battery::Level Battery::getLevel() const{
	return (Level) hysteresis.get();
}

bool Battery::isShutdown() const{
	return shutdown;
}
