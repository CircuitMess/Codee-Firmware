#ifndef BIT_FIRMWARE_GAMERUNNER_H
#define BIT_FIRMWARE_GAMERUNNER_H


#include "Util/Threaded.h"
#include "Game.h"

class GameRunner{
public:
	GameRunner(Display& display);

	Games getCurrent();

	void startGame(Games game);
	void endGame();
	void exitGame();

	void loop();
	void resume();

private:
	Display& display;

	uint64_t lastMicros;

	std::unique_ptr<Game> currentGame;
	Games currentGameEnum = Games::COUNT;

	static constexpr uint32_t FrameTime = 25; // [ms]

	struct {
		const char* splash;
		const char* instr;
	} static constexpr SplashImages[(uint32_t) Games::COUNT] = {
			{ "S:/GameSplash/Splash1.bin", "S:/GameSplash/Instr1.bin" },
			{ "S:/GameSplash/Splash2.bin", "S:/GameSplash/Instr2.bin" },
			{ "S:/GameSplash/Splash3.bin", "S:/GameSplash/Instr3.bin" },
			{ "S:/GameSplash/Splash4.bin", "S:/GameSplash/Instr4.bin" },
			{ "S:/GameSplash/Splash5.bin", "S:/GameSplash/Instr5.bin" },
			{ "S:/GameSplash/Splash6.bin", "S:/GameSplash/Instr6.bin" }
	};

};


#endif //BIT_FIRMWARE_GAMERUNNER_H
