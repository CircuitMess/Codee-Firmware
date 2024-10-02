#include "Input.h"
#include "Util/Events.h"
#include <Util/stdafx.h>
#include <Pins.hpp>
#include <driver/gpio.h>

// button index -> GPIO port
const std::unordered_map<Input::Button, gpio_num_t> Input::PinMap{
		{ Left,  (gpio_num_t) BTN_LEFT },
		{ Right, (gpio_num_t) BTN_RIGHT },
		{ A,     (gpio_num_t) BTN_A },
		{ B,     (gpio_num_t) BTN_B }
};

const std::unordered_map<Input::Button, const char*> Input::PinLabels{
		{ Left,  "Left/Up" },
		{ Right, "Right/Down" },
		{ A,     "A" },
		{ B,     "B" },
};

Input::Input(bool invertLogic) : SleepyThreaded(SleepTime, "Input", 2048, 6, 0), invertLogic(invertLogic){
	auto mask = 0ULL;
	for(const auto& pair: PinMap){
		const auto port = pair.first;
		const auto pin = pair.second;

		btnState[port] = false;
		dbTime[port] = 0;

		mask |= (1ULL << pin);
	}

	gpio_config_t io_conf = {
			.pin_bit_mask = mask,
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = invertLogic ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
			.pull_down_en = invertLogic ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);

	start();
}

Input::~Input(){
	stop();
}

void Input::sleepyLoop(){
	scan();
}

void Input::scan(){
	for(const auto& pair: PinMap){
		const auto port = pair.first;
		const auto pin = pair.second;

		bool state = invertLogic ^ gpio_get_level(pin);

		if(state){
			pressed(port);
		}else{
			released(port);
		}
	}
}

void Input::pressed(Input::Button btn){
	if(btnState[btn]){
		dbTime[btn] = 0;
		return;
	}

	auto t = millis();

	if(dbTime[btn] == 0){
		dbTime[btn] = t;
		return;
	}else if(t - dbTime[btn] < DebounceTime){
		return;
	}

	btnState[btn] = true;
	dbTime[btn] = 0;

	Data data = {
			.btn = btn,
			.action = Data::Press
	};
	Events::post(Facility::Input, data);
}

void Input::released(Input::Button btn){
	if(!btnState[btn]){
		dbTime[btn] = 0;
		return;
	}

	auto t = millis();

	if(dbTime[btn] == 0){
		dbTime[btn] = t;
		return;
	}else if(t - dbTime[btn] < DebounceTime){
		return;
	}

	btnState[btn] = false;
	dbTime[btn] = 0;

	Data data = {
			.btn = btn,
			.action = Data::Release
	};
	Events::post(Facility::Input, data);
}
