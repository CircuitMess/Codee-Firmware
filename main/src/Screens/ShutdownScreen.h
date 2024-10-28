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

	static constexpr uint32_t ShutdownTime = 4000;
	static constexpr uint32_t BlinkPeriod = 500;
	uint32_t startTime = 0;

	static constexpr const char* BgPaths[6] = {
			"S:/Bg/Level1.bin",
			"S:/Bg/Level2.bin",
			"S:/Bg/Level3.bin",
			"S:/Bg/Level4.bin",
			"S:/Bg/Level5.bin",
			"S:/Bg/Level6.bin"
	};

};


#endif //CLOCKSTAR_FIRMWARE_SHUTDOWNSCREEN_H
