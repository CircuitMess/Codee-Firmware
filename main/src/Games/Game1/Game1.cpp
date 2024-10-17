#include "Game1.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "Util/Services.h"
#include "Util/stdafx.h"

Game1::Game1(Sprite& canvas) : Game(canvas, Games::Oily, "/Games/1/", {
		{ "Arrow.raw",    {}, true },
		{ "EmptyCan.raw", {}, true },
		{ "bg.raw",       {}, true },
		{ "FullCan.raw",  {}, true },
		{ "OilyDone.gif", {}, true },
		{ "OilyIdle.gif", {}, true },
		{ "OilyJump.gif", {}, true }}),
							   statsManager((StatsManager*) Services.get(Service::Stats)),
							   audio((ChirpSystem*) Services.get(Service::Audio)){

}

void Game1::onLoad(){
	auto spriteBar = std::make_unique<SpriteRC>(PixelDim{ 9, 120 });
	bar = std::make_unique<Bar>(spriteBar->getSprite());
	bar->resetGoal();
	barGO = std::make_shared<GameObject>(
			std::move(spriteBar),
			nullptr
	);
	addObject(barGO);
	barGO->getRenderComponent()->setLayer(1);
	barGO->setPos({ 116, 4 });

	indicatorGO = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("Arrow.raw"), PixelDim{ 7, 11 }),
			nullptr
	);
	addObject(indicatorGO);
	indicatorGO->getRenderComponent()->setLayer(1);
	indicator = std::make_unique<Indicator>(indicatorGO);
	indicator->setGoal(bar->getY());

	auto spriteCan = std::make_unique<SpriteRC>(PixelDim{ 24, 21 });
	oilCan = std::make_unique<OilCan>(spriteCan->getSprite(), getFile("FullCan.raw"), getFile("EmptyCan.raw"));
	oilCanGO = std::make_shared<GameObject>(
			std::move(spriteCan),
			nullptr
	);
	addObject(oilCanGO);
	oilCanGO->setPos({ 76, 60 });
	oilCanGO->getRenderComponent()->setLayer(1);
	oilCan->setGameObject(oilCanGO);

	duckGo = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("OilyIdle.gif")),
			nullptr
	);
	addObject(duckGo);
	duckGo->getRenderComponent()->setLayer(1);
	duckGo->setPos({ 12, 36 });
	duckAnim = std::static_pointer_cast<AnimRC>(duckGo->getRenderComponent());

	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("bg.raw"), PixelDim({ 128, 128 })),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(0);
}

void Game1::onLoop(float deltaTime){
	if(exitFlag){
		exitFlag = false;
		exit();
		return;
	}

	indicator->move(deltaTime);
	if(oilCan->move(deltaTime)){
		duckGo->setPos({ 2, 22 }); //manually set for the gif to fit
		duckAnim->setAnim(getFile("OilyDone.gif"));
		duckAnim->setLoopDoneCallback([this](uint32_t i){
			delayMillis(700);
			exitFlag = true;
			return;
		});
	};
}

void Game1::onRender(Sprite& canvas){
	if(statsManager->getLevel() <= 2){
		canvas.setTextColor(TFT_WHITE);
	}else if(statsManager->getLevel() <= 4){
		canvas.setTextColor(TFT_BLACK);
	}else{
		canvas.setTextColor(TFT_WHITE);
	}

	canvas.setTextFont(0);
	canvas.setTextSize(0);
	canvas.setCursor(1, 2);
	canvas.printf("Attempts: %d", tries);
}

void Game1::onStart(){
	duckAnim->start();
}

void Game1::onStop(){
	duckAnim->stop();
}

void Game1::resetAnim(){
	duckAnim->setAnim(getFile("OilyIdle.gif"));
	duckAnim->setLoopDoneCallback({});
}

void Game1::addPoints(int difference){
	multiplier = (length - (float) difference) / length;
	multiplier = pow(multiplier, 12);
	fillPercent += multiplier * maxPoints;
	oilCan->fill(fillPercent);
	bar->resetGoal();

	if(done){

	}else if(indicator->getDifference() < 30){
		audio->play({{ 250, 200, 50 },
					 { 400, 700, 50 }});
	}else if(indicator->getDifference() >= 30){
		audio->play({{ 300, 300, 50 },
					 { 0,   0,   50 },
					 { 300, 300, 50 }});
	}

	if(fillPercent >= 0.999f){
		Sound s = {{ 600, 400,  200 },
				   { 400, 1000, 200 }};
		audio->play(s);

		removeObject(barGO);
		removeObject(indicatorGO);
		oilCan->startMoving();
		done = true;
	}else{
		if(indicator->getDifference() < 30){
			audio->play({{ 250, 200, 50 },
						 { 400, 700, 50 }});
			duckAnim->setAnim(getFile("OilyJump.gif"));
			duckAnim->setLoopDoneCallback([this](uint32_t){
				resetAnim();
			});
		}else if(indicator->getDifference() >= 30){
			audio->play({{ 300, 300, 50 },
						 { 0,   0,   50 },
						 { 300, 300, 50 }});
		}
	}

	indicator->setGoal(bar->getY());
}

Stats Game1::returnStats(){
	if(!done) return {};

	float success = (float) (minTries) / (float) (tries); // 0 - 1.0
	return Stats({ (uint8_t)(25.0 * success), (uint8_t)(60.0 * success), (uint16_t)(50.0 * success) });
}

void Game1::handleInput(const Input::Data& data){
	if(done) return;

	if(data.action != Input::Data::Press) return;

	if(data.btn == Input::D){
		audio->play(Sound{ Chirp{ 400, 350, 50 }});
		exit();
		return;
	}
	if(data.btn == Input::A){
		tries++;
		addPoints(indicator->getDifference());
	}
}
