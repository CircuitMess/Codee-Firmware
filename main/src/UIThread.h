#ifndef BIT_FIRMWARE_UITHREAD_H
#define BIT_FIRMWARE_UITHREAD_H

#include "LV_Interface/LVGL.h"
#include "LV_Interface/FSLVGL.h"
#include "GameEngine/GameRunner.h"
#include "Util/Threaded.h"
#include "GameEngine/Game.h"
#include "Devices/Display.h"

class UIThread : public Threaded {
public:
	UIThread(LVGL& lvgl, GameRunner& gameRunner, FSLVGL& fs);
	~UIThread() override;

	void startGame(Games game);
	void startScreen(std::function<std::unique_ptr<LVScreen>()> create);

//	void pauseGame();
//	void resumeGame();
	void exitGame();

private:
	LVGL& lvgl;
	GameRunner& gamer;
	FSLVGL& fs;

	EventQueue evts;

	enum class Src { LVGL, Game, None } active = Src::None;

	void loop() override;
	void checkShutdown();

};


#endif //BIT_FIRMWARE_UITHREAD_H
