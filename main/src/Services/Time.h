#ifndef CLOCKSTAR_FIRMWARE_TIME_H
#define CLOCKSTAR_FIRMWARE_TIME_H

#include "Util/Threaded.h"
#include "Devices/RTC.h"

class Time {
public:
	Time(RTC& rtc);

	void updateFromRTC();

	time_t getUnixTime() const;
	tm getTime() const;

private:
	RTC& rtc;

	uint64_t updateTime = 0;
	time_t time;

};


#endif //CLOCKSTAR_FIRMWARE_TIME_H
