#include "Input.h"
#include "Util/Events.h"
#include <Util/stdafx.h>
#include <driver/gpio.h>

const std::unordered_map<Input::Button, const char*> Input::PinLabels{
		{ A,     "A" },
		{ B,     "B" },
		{ C,     "C" },
		{ D,     "D" },
};

Input::Input(bool invertLogic) : SleepyThreaded(SleepTime, "Input", 2048, 6, 0), invertLogic(invertLogic){
	setupPins();
}

Input::~Input(){
	stop();
}

void Input::begin(){
	setupPins();
	start();
}

void Input::end(){
	stop();
}

void Input::setupPins(){
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
}

std::unordered_map<Input::Button, bool> Input::getState(){
	return btnState;
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
