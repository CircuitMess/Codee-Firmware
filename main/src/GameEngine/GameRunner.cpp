#include "GameRunner.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "Games/Game1/Game1.h"

static const std::unordered_map<Games, std::function<std::unique_ptr<Game>(Sprite& canvas)>> Launcher{
		{ Games::Oily, [](Sprite& canvas){ return std::make_unique<Game1>(canvas); }}
};

GameRunner::GameRunner(Display& display) : display(display){

}

Games GameRunner::getCurrent(){
	return currentGameEnum;
}

void GameRunner::startGame(Games game){
	endGame();

	if(!Launcher.contains(game)) return;

	const auto startTime = millis();

	auto launcher = Launcher.at(game);

	auto inst = launcher(display.getCanvas());

	inst->load();
	while(!inst->isLoaded() || (millis() - startTime) < 2000){
		delayMillis(100);
	}

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
	display.getLGFX().display();

	const auto loopTime = (micros() - currMicros) / 1000;
	if(loopTime > FrameTime){
		delayMillis(1);
	}else{
		delayMillis(FrameTime - loopTime);
	}
}
