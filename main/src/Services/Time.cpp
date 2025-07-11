#include <esp_log.h>
#include "Time.h"
#include "Util/stdafx.h"
#include "Util/Events.h"

static const char* TAG = "Time";

Time::Time(RTC& rtc) : rtc(rtc){
	updateFromRTC();
}

time_t Time::getUnixTime() const{
	return time + (time_t) (millis() - updateTime) / 1000;
}

std::tm Time::getTime() const{
	time_t currentTime = getUnixTime();
	tm ret = {};
	gmtime_r(&currentTime, &ret);
	return ret;
}

void Time::updateFromRTC(){
	tm time_tm = rtc.getTime();
	time = mktime(&time_tm);

	if(time < 0 || time >= 31536000 * 10){ // Max 10 years // TODO: this code is only for Codee, where actual time is irrelevant
		tm timeZero = {
				.tm_mday = 1,
				.tm_year = 70
		};
		rtc.setTime(timeZero);
		time = 0;
	}

	updateTime = millis();
}
