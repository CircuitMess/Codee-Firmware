#ifndef CLOCKSTAR_FIRMWARE_SHUTDOWNSCREEN_H
#define CLOCKSTAR_FIRMWARE_SHUTDOWNSCREEN_H


#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"

class ShutdownScreen : public LVScreen {
public:
	ShutdownScreen();

private:
	void loop() override;
	void onStart() override;

	void off();

	lv_obj_t* label;
	lv_obj_t* img;

	static constexpr uint32_t ShutdownTime = 6000;
	static constexpr uint32_t BlinkPeriod = 500;
	uint32_t blinkTime = 0;
	uint32_t startTime = 0;
};


#endif //CLOCKSTAR_FIRMWARE_SHUTDOWNSCREEN_H
