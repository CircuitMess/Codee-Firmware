#ifndef CODEE_FIRMWARE_SCORESCREEN_H
#define CODEE_FIRMWARE_SCORESCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Services/Stats.hpp"
#include "Screens/PetScreen/Elements/StatSprite.h"
#include "Services/StatsManager.h"


class ScoreScreen : public LVScreen {
public:
	ScoreScreen(Stats statsIncrease);
	~ScoreScreen() override;

private:
	void onStart() override;
	void loop() override;
	void onStop() override;

	StatsManager* statsManager;
	const Stats statsIncrease;
	Stats startingStats;

	StatSprite* oil = nullptr;
	StatSprite* happiness = nullptr;
	StatSprite* xp = nullptr;

	lv_obj_t* frame;
	lv_obj_t* statsRows[3];

	char bgPath[20] = { 0 };

	lv_anim_t xpAnim;
	static constexpr const int XPAnimDurationPerLevel = 750;

	lv_timer_t* exitTimer;
	static constexpr const uint32_t ExitTimeout = 4000; //[ms]

	EventQueue queue;
};


#endif //CODEE_FIRMWARE_SCORESCREEN_H
