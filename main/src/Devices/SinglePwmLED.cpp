#include "SinglePwmLED.h"
#include <algorithm>

SinglePwmLED::SinglePwmLED(uint8_t pin, ledc_channel_t channel, uint8_t limit) : SingleLED(
		std::min(limit, (uint8_t) 100)), pwm(pin, channel){
	pwm.setDuty(0);
}

SinglePwmLED::~SinglePwmLED(){
	pwm.setDuty(0);
	pwm.detach();
}

void SinglePwmLED::write(uint8_t val){
	if(val == 0){
		pwm.detach();
		return;
	}
	pwm.setDuty(val);
}
