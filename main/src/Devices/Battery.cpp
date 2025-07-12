#include "Battery.h"
#include "../Pins.hpp"
#include "Util/Events.h"
#include <soc/efuse_reg.h>
#include <Util/stdafx.h>
#include <cmath>
#include <driver/gpio.h>
#include <Util/EfuseMeta.h>

Battery::Battery(ADC& adc) : SleepyThreaded(MeasureIntverval, "Battery", 3 * 1024, 5, 1), adc(adc), refSwitch(PIN_VREF), hysteresis({ 0, 4, 15, 30, 50, 70, 90, 100 }, 3){
	const auto config = [&adc](int pin, adc_cali_handle_t& cali, std::unique_ptr<ADCReader>& reader, bool emaAndMap){
		adc_unit_t unit;
		adc_channel_t chan;
		ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pin, &unit, &chan));
		assert(unit == adc.getUnit());

		const auto atten = EfuseMeta::getRev() == 1 ? ADC_ATTEN_DB_2_5 : ADC_ATTEN_DB_0;

		adc.config(chan, {
				.atten = atten,
				.bitwidth = ADC_BITWIDTH_12
		});

		const adc_cali_curve_fitting_config_t curveCfg = {
				.unit_id = unit,
				.chan = chan,
				.atten = atten,
				.bitwidth = ADC_BITWIDTH_12
		};
		ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&curveCfg, &cali));

		if(emaAndMap){
			reader = std::make_unique<ADCReader>(adc, chan, cali, Offset, Factor, EmaA, Battery::getRange().min, Battery::getRange().max);
		}else{
			reader = std::make_unique<ADCReader>(adc, chan, cali, Offset, Factor);
		}
	};

	config(PIN_BATT, caliBatt, readerBatt, true);
	config(PIN_BATT, caliRef, readerRef, false);

	calibrate();

	sample(true);
	sample(true);
}

void Battery::begin(){
	start();
}

const Battery::BattRange& Battery::getRange(){
	if(EfuseMeta::getRev() == 1){
		return RangeRev2;
	}else{
		return RangeRev1;
	}
}

uint8_t Battery::getPerc() const{
	return readerBatt->getValue();
}

Battery::Level Battery::getLevel() const{
	return (Level) hysteresis.get();
}

bool Battery::isCharging(){
	if(EfuseMeta::getRev() == 0) return false;
	return readerBatt->getRaw() >= ChrgThresh;
}

bool Battery::isShutdown() const{
	return shutdown;
}

void Battery::calibrate(){
	refSwitch.on();

	delayMillis(100);
	for(int i = 0; i < CalReads; i++){
		readerRef->sample();
		delayMillis(10);
	}

	float total = 0;
	for(int i = 0; i < CalReads; i++){
		total += readerRef->sample();
		delayMillis(10);
	}

	const float reading = total / (float) CalReads;
	const float offset = CalExpected - reading;
	readerBatt->setMoreOffset(offset);

	refSwitch.off();
	delayMillis(100);

	printf("Calibration: Read %.02f mV, expected %.02f mV. Applying %.02f mV offset.\n", reading, CalExpected, offset);
}

void Battery::sample(bool fresh){
	if(shutdown) return;

	auto oldLevel = getLevel();

	if(fresh){
		readerBatt->resetEma();
		hysteresis.reset(readerBatt->getValue());
	}else{
		hysteresis.update(readerBatt->sample());
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
