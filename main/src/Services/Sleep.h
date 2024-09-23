#ifndef BIT_FIRMWARE_SLEEP_H
#define BIT_FIRMWARE_SLEEP_H

#include "Util/Threaded.h"
#include "Util/Events.h"

class Sleep : public Threaded {
public:
	Sleep();

private:
	EventQueue evts;

	uint64_t activity;

	void loop() override;

	void off();

	void goSleep();

};


#endif //BIT_FIRMWARE_SLEEP_H
