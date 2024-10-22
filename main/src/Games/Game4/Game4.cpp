#include "Game4.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Util/Services.h"
#include <esp_random.h>

Game4::Game4::Game4(Sprite& canvas) : Game(canvas, Games::PolarJump, "/Games/4/", {
		{ "bg.raw", {}, true },
		{ "podium.raw", {}, false },
		{ "sign.raw", {}, false },

		{ "tileTop1.raw", {}, true },
		{ "tileTop2.raw", {}, true },
		{ "tileBot1.raw", {}, true },
		{ "tileBot2.raw", {}, true },
		{ "tileBot3.raw", {}, true },

		{ "iceblock.raw", {}, true },
		{ "snowman.raw", {}, true },
		{ "chest.raw", {}, true },
		{ "sign.raw", {}, true },

		{ "bird.raw", {}, true },

		{ "ducked.gif", {}, true },
		{ "ducking.gif", {}, false },
		{ "unducking.gif", {}, false },
		{ "jump.gif", {}, false },
		{ "walkDead.gif", {}, false },
		{ "win.gif", {}, false },
		{ "walk.gif", {}, true },
		RES_HEART,
		RES_GOBLET}){
}

void Game4::Game4::onLoad(){
	setupObstacles();
	leftWallObject = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 10, 128 })
	);
	leftWallObject->setPos({ -55, 0 });
	addObject(leftWallObject);


	///Tiles
	tileManager = std::make_unique<TileManager>(movingTiles);
	tileManager->addFilePair(getFile("tileTop1.raw"), getFile("tileBot1.raw"));
	tileManager->addFilePair(getFile("tileTop2.raw"), getFile("tileBot2.raw"));
	tileManager->addBotFile(getFile("tileBot3.raw"));
	tileManager->create(); // creates GameObjects into movingTiles

	leftWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 1, 128 })
	);
	leftWall->setPos({ -tileDim * tilesPerArray, 0 });
	addObject(leftWall);


	for(int i = 0; i < movingTiles.size(); i++){
		auto obj = movingTiles[i];
		addObject(obj);
		collision.addPair(*leftWall, *obj, [this, i](){ tileManager->reset(i); });
	}

	///Background
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-2);
	addObject(bg);


	///Player
	std::initializer_list<glm::vec2> points = {{ 26, 50 },
											   { 23, 42 },
											   { 30, 23 },
											   { 38, 15 },
											   { 47, 16 },
											   { 45, 42 },
											   { 38, 51 }};
	auto playerGoCc = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<PolygonCC>(points, glm::vec2{ 35, 35 })
	);

	addObject(playerGoCc);
	playerGoCc->setPos({ 0, 50 });

	auto playerGoRc = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("walk.gif")),
			nullptr
	);
	addObject(playerGoRc);
	playerGoRc->getRenderComponent()->setLayer(10);
	playerGoRc->setPos({ 0, 50 });

	player = std::make_unique<Player>(playerGoRc, playerGoCc, this);
	player->setFiles(getFile("walk.gif"),
					 getFile("walkDead.gif"),
					 getFile("jump.gif"),
					 getFile("ducking.gif"),
					 getFile("ducked.gif"),
					 getFile("unducking.gif"),
					 getFile("win.gif"));
	///Score
	goblet = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(FILE_GOBLET), PixelDim{ 7, 7 }),
			nullptr);
	goblet->setPos({ 87, 2 });
	addObject(goblet);

	std::string scoreText = std::to_string(score) + "/" + std::to_string(scoreMax);
	auto scoreObj = std::make_shared<GameObject>(
			std::make_unique<TextRC>(scoreText, TextStyle{ &Font0, TFT_WHITE, 1, TL_DATUM }),
			nullptr);
	scoreObj->setPos({ 96, 2 });
	scoreTextRC = std::static_pointer_cast<TextRC>(scoreObj->getRenderComponent());
	addObject(scoreObj);
	///Hearts
	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());
}

void Game4::Game4::onLoop(float deltaTime){
	if(endPauseState != Running){
		endPauseCounter += deltaTime;
		if((endPauseState == Win && endPauseCounter >= WinPause) || (endPauseState == Lose && endPauseCounter >= DeathPause)){
			exit();
		}
		return;
	}

	player->update(deltaTime);
	for(const auto& obj: movingObjects){
		float x = obj->getPos().x - deltaTime * speed;
		float y = obj->getPos().y;
		obj->setPos({ x, y });
	}
	for(const auto& obj: movingTiles){
		float x = obj->getPos().x - deltaTime * speed;
		float y = obj->getPos().y;
		obj->setPos({ x, y });
	}

	if(isDone){
		if(speed == 0) return;
		if(podium->getPos().x <= 58){
			auto diff = 58 - podium->getPos().x;

			for(const auto& obj: movingObjects){
				float x = obj->getPos().x + diff;
				float y = obj->getPos().y;
				obj->setPos({ x, y });
			}

			for(const auto& obj: movingTiles){
				float x = obj->getPos().x + diff;
				float y = obj->getPos().y;
				obj->setPos({ x, y });
			}

			player->win();
			speed = 0;
		}
	}else{
		spawnValue += deltaTime;
		if(spawnValue >= spawnRate){
			spawnValue -= spawnRate;
			spawn();
		}
	}
}

void Game4::Game4::onStop(){
	player->duckReleased();
}

void Game4::Game4::setupObstacles(){
	obstacleOver.push_back({ getFile("chest.raw"), { 26, 19 }, {}});
	obstacleOver.push_back({ getFile("iceblock.raw"), { 24, 19 }, {}});
	obstacleOver.push_back({ getFile("snowman.raw"), { 14, 19 }, {}});
	obstacleOver.push_back({ getFile("sign.raw"), { 24, 19 }, {}});

	obstacleUnder.push_back({ getFile("bird.raw"), { 36, 21 }, {{ 16, 20 }, { 0, 14 }, { 0, 0 }, { 35, 1 }, { 35, 12 }}});
}

void Game4::Game4::spawn(){
	if(speed < speedMax){
		speed += speedIncrement;
		spawnRate -= spawnIncrement;
	}

	if(score >= scoreMax - 1){
		isDone = true;
		podium = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("podium.raw"), PixelDim{ 44, 20 }),
				nullptr
		);
		addObject(podium);
		podium->getRenderComponent()->setLayer(0);
		podium->setPos({ 128, topY - 20 });
		movingObjects.push_back(podium);
		return;
	}

	if(obstacleIndex.empty()){
		for(int i = 0; i < obstacleOver.size() + obstacleUnder.size(); i++){
			obstacleIndex.insert(i);
		}
	}

	auto item = obstacleIndex.begin();
	std::advance(item, esp_random() % obstacleIndex.size());

	uint8_t in = *item;
	Obstacle obstacle;
	int posY;
	if(in >= obstacleOver.size()){
		obstacle = obstacleUnder[in - obstacleOver.size()];
		posY = topY - 28 - obstacle.dim.y;
	}else{
		obstacle = obstacleOver[in];
		posY = topY - obstacle.dim.y * 0.75f;
	}
	obstacleIndex.erase(item);

	std::shared_ptr<GameObject> gObj;
	if(obstacle.points.size() >= 3){
		gObj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(obstacle.file, obstacle.dim),
				std::make_unique<PolygonCC>(obstacle.points)
		);
	}else{
		gObj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(obstacle.file, obstacle.dim),
				std::make_unique<RectCC>(obstacle.dim)
		);
	}
	addObject(gObj);
	gObj->setPos({ 128, posY });
	gObj->getRenderComponent()->setLayer(1);

	collision.addPair(*gObj, *player->getGameObjectCc(), [this](){
		if(player->invincible) return;
		playerHit();
	});
	movingObjects.push_back(gObj);
	collision.addPair(*leftWallObject, *gObj, [this](){
		removeObject(movingObjects[0]);
		movingObjects.erase(movingObjects.begin());
		scoreUp();
	});
}

void Game4::Game4::playerHit(){
	life--;
	hearts->setLives(life);
	if(life == 0){
		speed = 0.0f;
		spawnRate = 10000.0f;
		player->death();
		audio.play({{ 300, 400, 100 },
					{ 400, 300, 100 },
					{ 200, 300, 100 },
					{ 300, 200, 100 },
					{ 80,  80,  300 }});
		return;
	}
	player->invincible = true;
	audio.play({{ 100, 100, 50 },
				{ 0,   0,   25 },
				{ 300, 100, 100 }});
}

float Game4::Game4::getSpeed() const{
	return speed;
}

void Game4::Game4::scoreUp(){
	audio.play({{ 400, 600, 75 },
				{ 0,   0,   50 },
				{ 500, 900, 75 }});

	score++;

	std::string scoreText = std::to_string(score) + "/" + std::to_string(scoreMax);
	scoreTextRC->setText(scoreText);
}

void Game4::Game4::handleInput(const Input::Data& data){
	if(isDone || endPauseState != Running) return;

	if(data.action == Input::Data::Press){
		if(data.btn == Input::A){
			player->duckPressed();
		}
		if(data.btn == Input::B){
			player->jumpPressed();
		}
	}else if(data.action == Input::Data::Release){
		if(data.btn == Input::A){
			player->duckReleased();
		}
	}

}

Stats Game4::Game4::returnStats(){
	float success = (float) (score) / (float) (scoreMax);
	return Stats({ (uint8_t) (35.0 * success), (uint8_t) (20.0 * success), (uint16_t) (1000.0 * success) });
}
