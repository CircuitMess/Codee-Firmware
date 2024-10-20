#include "Game3.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include <esp_random.h>
#include "lvgl.h"

Game3::Game3::Game3(Sprite& canvas) : Game(canvas, Games::PingoSnack, "/Games/3/", {
		{ "bg.raw", {}, true },
		{ "bomb1.raw", {}, true },
		{ "bomb2.raw", {}, true },
		{ "fish1.raw", {}, true },
		{ "fish2.raw", {}, true },
		{ "BarFrame.raw", {}, true },
		RES_HEART,
		{ "eatBad.gif", {}, false },
		{ "walk.gif", {}, true },
		{ "eat.gif", {}, false }}){
}

void Game3::Game3::onLoad(){
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(-2);

	collectorBot = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(PixelDim{ 128, 15 })
	);
	collectorBot->setPos({ 0, 128 + 30 });
	addObject(collectorBot);

	player = std::make_unique<Player>(getFile("walk.gif"), getFile("eat.gif"), getFile("eatBad.gif"));
	addObject(player->getGameObject());
	player->getGameObject()->getRenderComponent()->setLayer(1);

	addTemplate("fish1.raw", PixelDim{ 14, 20 }, 10);
	addTemplate("fish2.raw", PixelDim{ 14, 20 }, 5);
	addTemplate("bomb1.raw", PixelDim{ 14, 22 }, 0);
	addTemplate("bomb2.raw", PixelDim{ 5, 20 }, 0);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 10, 5 });
	addObject(hearts->getGO());

	auto barRC = std::make_unique<SpriteRC>(PixelDim{ 8, 122 });
	hungerBar = barRC->getSprite();
	barRC->setLayer(2);
	auto barGO = std::make_shared<GameObject>(
			std::move(barRC),
			nullptr
	);
	addObject(barGO);
	barGO->setPos({ 1, 2 });
	hungerBar->clear(TFT_TRANSPARENT);
	Display::drawFile(*hungerBar, getFile("BarFrame.raw"), 0, 0, 6, 120);
}

void Game3::Game3::onLoop(float deltaTime){
	if(state != Running){
		if(state != WinPause && state != DeadPause) return;
		donePauseCounter += deltaTime;
		if(donePauseCounter >= DonePause){
			exit();
		}
		return;
	}

	player->loop(deltaTime);
	timeToSpawn += deltaTime;
	if(timeToSpawn >= spawnInterval){
		spawnRandom();
		timeToSpawn -= spawnInterval;
	}

	if(movingObjects.empty()) return;

	for(const auto& object: movingObjects){
		int y = deltaTime * object.second + object.first->getPos().y;
		int x = object.first->getPos().x;
		object.first->setPos({ x, y });
	}
}

void Game3::Game3::onStop(){
	player->btnReleased(Input::A);
	player->btnReleased(Input::B);
}

void Game3::Game3::addTemplate(std::string file, PixelDim dim, int value){
	Template temp{ file, dim, value };
	if(value > 0){
		foods.push_back(temp);
	}else{
		bombs.push_back(temp);
	}
}

void Game3::Game3::spawnRandom(){
	if(foodCounter == FoodInSet && bombCounter == BombInSet){
		foodCounter = bombCounter = 0;
	}
	int randNum = esp_random() % (101);
	if((randNum <= 75 && foodCounter < FoodInSet) || bombCounter == BombInSet){
		foodCounter++;
		int pick = esp_random() % foods.size();
		spawnItem(foods[pick]);
	}else{
		bombCounter++;
		int pick = esp_random() % bombs.size();
		spawnItem(bombs[pick]);
	}
}

void Game3::Game3::spawnItem(Game3::Game3::Template temp){
	int randPos = esp_random() % (128 - temp.dim.x - 10) + 10; //10 - so that it doesn't spawn on the bar
	auto go = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(temp.path), temp.dim),
			std::make_unique<RectCC>(temp.dim)
	);
	addObject(go);
	float speed = (esp_random() % (speedMax - speedMin + 1)) + speedMin;

	movingObjects.insert(std::make_pair(go, speed));
	go->setPos({ randPos, -temp.dim.y });
	Item item{ go, temp.value };
	collision.addPair(*player->getGameObject(), *item.go, [this, item](){
		collisionHandler(item);
	});
	collision.addPair(*collectorBot, *item.go, [this, item](){
		movingObjects.erase(item.go);
		removeObject(item.go);
	});
}

void Game3::Game3::collisionHandler(Item item){
	if(state != Running) return;
	if(player->isEatingBad() && item.value == 0){
		return;
	}

	removeObject(item.go);
	player->startEating(item.value);
	if(item.value > 0){
		hungerMeter = std::min(hungerMeter + item.value, hungerMeterMax);
		drawBar();
		if(hungerMeter >= hungerMeterMax){
			Sound s = {{ 400, 600,  200 },
					   { 0,   0,    50 },
					   { 400, 800,  200 },
					   { 0,   0,    50 },
					   { 400, 1000, 200 },
					   { 0,   0,    150 },
					   { 800, 1000, 50 }};
			audio.play(s);
			player->filled(this);
			state = Win;

		}else{
			audio.play({{ 250, 200, 50 },
						{ 400, 700, 50 }});
		}
	}else{
		lives--;
		if(lives > 0){
			audio.play({{ 80,  300, 50 },
						{ 0,   0,   50 },
						{ 200, 50,  100 }});
		}
		hearts->setLives(lives);
	}
	if(lives <= 0){
		player->killed(this);
		audio.play({{ 300, 400, 100 },
					{ 400, 300, 100 },
					{ 200, 300, 100 },
					{ 300, 200, 100 },
					{ 80,  80,  300 }});
		state = Dead;
	}

	movingObjects.erase(item.go);
}

void Game3::Game3::drawBar(){
	float fillPercent = ((float) hungerMeter / (float) hungerMeterMax);
	double hue = (float)fillPercent * 85.f;
	auto rgbColor0 = lv_color_hsv_to_rgb(hue, 100, 100);

	uint16_t c0 = lgfx::color565(rgbColor0.red, rgbColor0.green, rgbColor0.blue);
	uint16_t filledPixels = std::round(116 * fillPercent);

	hungerBar->fillRect(2, 2 + 116 - filledPixels, 2, filledPixels, c0);
}

void Game3::Game3::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Press){
		player->btnPressed(data.btn);
	}else if(data.action == Input::Data::Release){
		player->btnReleased(data.btn);
	}
}

Stats Game3::Game3::returnStats(){
	float success = (float) hungerMeter / (float) hungerMeterMax;
	return Stats({ (uint8_t) (30.0 * success), (uint8_t) (20.0 * success), (uint16_t) (400.0 * success) });
}
