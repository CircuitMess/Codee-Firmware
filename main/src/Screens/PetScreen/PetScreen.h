#ifndef CODEE_FIRMWARE_PETSCREEN_H
#define CODEE_FIRMWARE_PETSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Services/Stats.hpp"
#include "Devices/Input.h"
#include "Elements/Character.h"
#include "Elements/StatsSprite.h"
#include "Elements/Menu.h"
#include "Devices/Battery.h"

class PetScreen : public LVScreen {
public:
	PetScreen();

private:
	void loop() override;
	void onStart() override;
	void onStop() override;

	EventQueue evts;
	void processEvents();

	void statsChanged(const Stats& stats, bool leveledUp);

	int8_t queuedLaunch = -1;
	void launch(uint8_t i);

	StatsManager* statsManager;
	Battery* battery;

	Character* characterSprite;
	StatsSprite* statsSprite;
	StatSprite* xpSprite;
	Menu* menu;

	bool startShown = false;
	bool stopped = false;

	constexpr static uint8_t RustThreshold = 25;

	uint64_t lastAlt = 0;
	uint64_t altCooldown = 0;

	static constexpr const char* BgPaths[6] = {
			"S:/Bg/Level1.bin",
			"S:/Bg/Level2.bin",
			"S:/Bg/Level3.bin",
			"S:/Bg/Level4.bin",
			"S:/Bg/Level5.bin",
			"S:/Bg/Level6.bin"
	};

};


#endif //CODEE_FIRMWARE_PETSCREEN_H
