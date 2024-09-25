#ifndef CODEE_FIRMWARE_DEATHSCREEN_H
#define CODEE_FIRMWARE_DEATHSCREEN_H

#include "LV_Interface/LVScreen.h"

class DeathScreen : public LVScreen {
public:
	DeathScreen();
	~DeathScreen() override;

private:
	std::string getPath();
	void onStop() override;
	void onStart() override;

	lv_obj_t* explosion;
	lv_obj_t* duck;

	lv_timer_t* timer;

	class StatsManager* stats;

	bool exit = false;
	static constexpr uint32_t duckVisibleMillis = 560;
	static constexpr uint32_t explosionStart = 1000;
	static constexpr uint32_t exitPause = 800;
	bool exploded = false;

	char bgPath[20] = { 0 };
};

#endif //CODEE_FIRMWARE_DEATHSCREEN_H
