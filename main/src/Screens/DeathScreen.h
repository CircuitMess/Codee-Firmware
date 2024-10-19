#ifndef CODEE_FIRMWARE_DEATHSCREEN_H
#define CODEE_FIRMWARE_DEATHSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "PetScreen/Elements/Character.h"
#include "Services/StatsManager.h"

class DeathScreen : public LVScreen {
public:
	DeathScreen();

private:
	void onStart() override;
	void loop() override;

	StatsManager* statsManager;

	Character* character = nullptr;

	enum Phase {
		Start, Explo1, Explo2, Stop, Done
	} phase = Start;
	uint32_t startTime;

	static constexpr const char* BgPaths[6] = {
			"S:/Bg/Level1.bin",
			"S:/Bg/Level2.bin",
			"S:/Bg/Level3.bin",
			"S:/Bg/Level4.bin",
			"S:/Bg/Level5.bin",
			"S:/Bg/Level6.bin"
	};

	constexpr static uint8_t RustThreshold = 25;

};

#endif //CODEE_FIRMWARE_DEATHSCREEN_H
