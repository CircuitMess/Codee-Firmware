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

private:
	bool stopped = false;

	EventQueue evts;
	void checkEvts();

	void checkSleep();
	uint64_t activityTime = 0;

	static constexpr int WakePins[] = { BTN_A, BTN_B, BTN_C, BTN_D };
	void setupWake();

	void freePins();
	void returnPins();

	void sleepLight();
	void sleepDeep();


};


#endif //CODEE_FIRMWARE_POWER_H
