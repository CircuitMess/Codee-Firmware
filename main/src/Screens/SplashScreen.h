#ifndef CODEE_FIRMWARE_SPLASHSCREEN_H
#define CODEE_FIRMWARE_SPLASHSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "GameEngine/Game.h"

class SplashScreen : public LVScreen {
public:
	SplashScreen(Games game);
	~SplashScreen() override;

private:
	Games game;
	void onStart() override;
	void loop() override;

	EventQueue queue;

	static constexpr uint32_t SplashTime = 2000;

	lv_timer_t* timer;

	struct SplashImages_t {
		const char* splashPath;
		const char* instructionsPath;
	};

	static constexpr SplashImages_t GamesSplashImages[(uint32_t) Games::COUNT] = {
			{ "S:/GameScreens/Splash1.bin", "S:/GameScreens/Inst1.bin" },
			{ "S:/GameScreens/Splash2.bin", "S:/GameScreens/Inst2.bin" },
			{ "S:/GameScreens/Splash3.bin", "S:/GameScreens/Inst3.bin" },
			{ "S:/GameScreens/Splash4.bin", "S:/GameScreens/Inst4.bin" },
			{ "S:/GameScreens/Splash5.bin", "S:/GameScreens/Inst5.bin" },
			{ "S:/GameScreens/Splash6.bin", "S:/GameScreens/Inst6.bin" }
	};
};


#endif //CODEE_FIRMWARE_SPLASHSCREEN_H
