#ifndef CODEE_FIRMWARE_PETSCREEN_H
#define CODEE_FIRMWARE_PETSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Services/Stats.hpp"
#include "Devices/Input.h"
#include "Elements/OSIcon.h"
#include "Elements/Character.h"
#include "Elements/StatsSprite.h"
#include "Screens/PetScreen/Elements/Menu.h"
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

	void hideMenu();
	void unhideMenu();

	lv_obj_t* topBar;

	StatsManager* statsManager;
	Battery* battery;

	OSIcon* osSprite;
	Character* characterSprite;

	StatsSprite* statsSprite;

	bool dead = false;
	constexpr static uint8_t rustThreshold = 25;

	static constexpr int16_t MenuY = -4;
	Menu* menu;
	bool menuHidden = true;
	uint64_t hideMillis = 0;
	static constexpr uint32_t HideTimeout = 5000; //[ms] = 5s
	static constexpr uint32_t HideAnimDuration = 300;
	lv_group_t* hideGroup;


	lv_anim_t hiderAnimation;

	uint64_t randCounter = 0;
	uint64_t randInterval = 0; //between 2s and 6s
	bool specialAnimPlaying = false;

	uint8_t selection = 0;

	constexpr static uint8_t characterX = 34;
	constexpr static uint8_t characterY = 36;

	char bgPath[20] = { 0 };

	EventQueue queue;


};


#endif //CODEE_FIRMWARE_PETSCREEN_H
