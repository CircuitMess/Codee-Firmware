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
	~PetScreen() override;

private:
	void loop() override;
	void onStart() override;
	void onStop() override;

	void statsChanged(const Stats& stats, bool leveledUp);

	StatsManager* statsManager;
	Battery* battery;

	Character* characterSprite;

	StatsSprite* statsSprite;
	StatSprite* xpSprite;

	Menu* menu;

	bool dead = false;
	constexpr static uint8_t rustThreshold = 25;


	//Levelup
	bool levelupInProgress = false;
	lv_group_t* levelupGroup;
	static constexpr uint32_t LevelupShowTime = 3000; //[ms] = 3s
	static constexpr uint32_t LevelupFadeTime = 500; //[ms] = 0.5s
	lv_anim_t levelupAnim;
	lv_timer_t* levelupTimer;
	void startLevelupAnim();
	void stopLevelupAnim();
	lv_obj_t* levelupImg;

	uint64_t lastAlt = 0;
	uint64_t altCooldown = 0;

	uint8_t selection = 0;

	static constexpr const char* BgPaths[6] = {
			"S:/Bg/Level1.bin",
			"S:/Bg/Level2.bin",
			"S:/Bg/Level3.bin",
			"S:/Bg/Level4.bin",
			"S:/Bg/Level5.bin",
			"S:/Bg/Level6.bin"
	};

	EventQueue queue;


};


#endif //CODEE_FIRMWARE_PETSCREEN_H
