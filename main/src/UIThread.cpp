#include "UIThread.h"
#include "Util/stdafx.h"
#include "GameEngine/Game.h"
#include "Devices/Battery.h"
#include "Screens/ShutdownScreen.h"
#include "LV_Interface/FSLVGL.h"
#include "Util/Services.h"
#include "Services/LEDService.h"

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
	if(data->action != Battery::Event::LevelChange){
		free(evt.data);
		return;
	}

	auto led = (LEDService*) Services.get(Service::LED);

	if(data->level == Battery::Critical){
		led->blink(LED::Red, -1, 200);

		Events::unlisten(&evts);
		evts.reset();

		// Skip UIThread::startScreen to avoid loading cache
		gamer.endGame();
		lvgl.startScreen([](){ return std::make_unique<ShutdownScreen>(); });
		active = Src::LVGL;
	}else if(data->level == Battery::VeryLow){
		led->blink(LED::Red, -1, 1000);
	}else{
		led->off(LED::Red);
	}

	free(evt.data);
}
