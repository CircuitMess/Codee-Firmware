#include "UIThread.h"
#include "Util/stdafx.h"
#include "GameEngine/Game.h"
#include "Devices/Display.h"
//#include "Screens/PauseScreen.h"
#include "Devices/Battery.h"
#include "Screens/ShutdownScreen.h"
#include "LV_Interface/FSLVGL.h"

UIThread::UIThread(LVGL& lvgl, GameRunner& gameRunner, FSLVGL& fs, Power& pwr) : Threaded("UI", 6 * 1024, 5, 0), lvgl(lvgl), gamer(gameRunner), fs(fs), pwr(pwr), evts(6){
	Events::listen(Facility::Battery, &evts);
}

UIThread::~UIThread(){
	stop();
}

void UIThread::loop(){
	checkShutdown();
	pwr.loop();

	if(active == Src::LVGL){
		lvgl.loop();
	}else if(active == Src::Game){
		gamer.loop();
	}else{
		delayMillis(10);
		return;
	}
}

void UIThread::startGame(Games game){
	lvgl.stopScreen();
	fs.unloadCache();
	gamer.startGame(game);
	active = Src::Game;
	lvgl.resetDisplayRefreshTimer();
}

void UIThread::startScreen(std::function<std::unique_ptr<LVScreen>()> create){
	gamer.endGame();
	lvgl.stopScreen();
	fs.loadCache();
	lvgl.startScreen(std::move(create));
	active = Src::LVGL;
}

//void UIThread::pauseGame(){
//	lvgl.startScreen([this](){ return std::make_unique<PauseScreen>(gamer.getCurrent()); });
//	active = Src::LVGL;
//}
//
//void UIThread::resumeGame(){
//	lvgl.stopScreen();
//	gamer.resume();
//	active = Src::Game;
//}

void UIThread::exitGame() {
	gamer.exitGame();
}

void UIThread::checkShutdown(){
	Event evt;
	if(!evts.get(evt, 0)) return;

	auto data = (Battery::Event*) evt.data;

	if(data->action == Battery::Event::LevelChange && data->level == Battery::Critical){
		Events::unlisten(&evts);
		evts.reset();

		// Skip UIThread::startScreen to avoid loading cache
		gamer.endGame();
		lvgl.startScreen([](){ return std::make_unique<ShutdownScreen>(); });
		active = Src::LVGL;
	}

	free(evt.data);
}
