#include "GameRunner.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "LV_Interface/LVGL.h"
#include "Games/Game1/Game1.h"
#include "Games/Game2/Game2.h"
#include "Games/Game3/Game3.h"
#include "Games/Game4/Game4.h"
#include "Games/Game5/Game5.h"
#include "Games/Game6/Game6.h"

static const std::unordered_map<Games, std::function<std::unique_ptr<Game>(Sprite& canvas)>> Launcher{
		{ Games::Oily, [](Sprite& canvas){ return std::make_unique<Game1>(canvas); }},
		{ Games::PolarSwim, [](Sprite& canvas){ return std::make_unique<Game2>(canvas); }},
		{ Games::PingoSnack, [](Sprite& canvas){ return std::make_unique<Game3::Game3>(canvas); }},
		{ Games::PolarJump, [](Sprite& canvas){ return std::make_unique<Game4::Game4>(canvas); }},
		{ Games::Dance, [](Sprite& canvas){ return std::make_unique<Game5>(canvas); }},
		{ Games::IceBlast, [](Sprite& canvas) { return std::make_unique<Game6>(canvas); }}
};

GameRunner::GameRunner(Display& display) : display(display){

}

Games GameRunner::getCurrent(){
	return currentGameEnum;
}

void GameRunner::startGame(Games game){
	endGame();

	if(!Launcher.contains(game)) return;

	LVGL::drawImage(SplashImages[(int) game].splash);

	const auto startTime = millis();

	auto launcher = Launcher.at(game);

	auto inst = launcher(display.getCanvas());

	inst->load();
	while(!inst->isLoaded() || (millis() - startTime) < 2000){
		delayMillis(100);
	}

	LVGL::drawImage(SplashImages[(int) game].instr);

	EventQueue evts(6);
	Events::listen(Facility::Input, &evts);
	for(;;){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;
		if(evt.facility != Facility::Input){
			free(evt.data);
			continue;
		}

		auto data = (Input::Data*) evt.data;
		if(data->action != Input::Data::Press){
			free(evt.data);
			continue;
		}

		free(evt.data);
		break;
	}
	Events::unlisten(&evts);

	currentGameEnum = game;
	currentGame = std::move(inst);
	currentGame->start();
	lastMicros = micros();
}

void GameRunner::endGame(){
	if(!currentGame) return;
	currentGame->stop();
	currentGame.reset();
	currentGameEnum = Games::COUNT;
}

void GameRunner::exitGame(){
	if(!currentGame){
		return;
	}

	currentGame->exit();
}

void GameRunner::resume(){
	lastMicros = micros();
	currentGame->start();
}

void GameRunner::loop(){
	if(!currentGame){
		delayMillis(FrameTime);
		return;
	}

	const auto currMicros = esp_timer_get_time();
	const auto delta = currMicros - lastMicros;
	lastMicros = currMicros;

	currentGame->loop(delta);
	if(!currentGame) return; // in case the game exited in its loop

	display.getCanvas().pushSprite(0, 0);

	const auto loopTime = (micros() - currMicros) / 1000;
	if(loopTime > FrameTime){
		delayMillis(1);
	}else{
		delayMillis(FrameTime - loopTime);
	}
}
