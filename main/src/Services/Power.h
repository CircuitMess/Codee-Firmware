#ifndef CODEE_FIRMWARE_POWER_H
#define CODEE_FIRMWARE_POWER_H

#include "Pins.hpp"
#include "Util/Events.h"

class Power {
public:
	Power();

	void loop();

	static void resetPins();
	static void powerOff();
	static void sleepDeep();

private:
	bool stopped = false;

	EventQueue evts;
	void checkEvts();

	void checkSleep();
	uint64_t activityTime = 0;

	static void setupWake();

	void freePins();
	void returnPins();

	void sleepLight();

	//periodically waking from deep sleep to check stats
	static constexpr uint64_t DeepSleepWakeInterval = 1000ULL * 1000ULL * 60ULL * 65ULL; //65 minutes

	//after 'LightSleepTimeout' seconds have passed, go to deep sleep
	static constexpr uint64_t LightSleepTimeout = 1000ULL * 1000ULL * 60ULL * 5ULL; //5 minutes

};


#endif //CODEE_FIRMWARE_POWER_H
