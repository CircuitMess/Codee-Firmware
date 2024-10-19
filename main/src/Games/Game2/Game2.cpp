#include "Game2.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Collision/CircleCC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "gtx/vector_angle.hpp"
#include "Util/Services.h"
#include <esp_random.h>

Game2::Game2(Sprite& canvas) : Game(canvas, Games::PolarSwim, "/Games/2/", {
		{ "player.gif", {}, true },
		{ "bg.raw", {}, true },
		RES_HEART,
		RES_GOBLET,
		{ TopObstacles[0].path, {}, true },
		{ TopObstacles[1].path, {}, true },
		{ BotObstacles[0].path, {}, true },
		{ BotObstacles[1].path, {}, true },
		{ BotObstacles[2].path, {}, true },
		{ BotObstacles[3].path, {}, true },
		{ BotObstacles[4].path, {}, true },
		{ BotObstacles[5].path, {}, true }}){

}

void Game2::onLoad(){
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());


	player = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("player.gif")),
			std::make_unique<CircleCC>(13, glm::vec2{ 20, 17 })
	);

	anim = std::static_pointer_cast<AnimRC>(player->getRenderComponent());
	anim->setLoopMode(GIF::Single);

	addObject(player);
	resetPlayer();
}

void Game2::onStart(){
	Game::onStart();

	anim->start();
}

void Game2::onStop(){
	anim->stop();
	anim->reset();
}

void Game2::onLoop(float deltaTime){
	updatePlayer(deltaTime);
	updateObstacles(deltaTime);

	if(state == Play){
		if(obstacles.empty() || obstacles.back().top->getPos().x <= 128 - obstacleSpacing){
			createObstaclePair();
		}
	}

	if(!obstacles.empty()){
		if(obstacles.front().bot->getPos().x < -85){
			removeObject(obstacles.front().top);
			removeObject(obstacles.front().bot);
			obstacles.pop_front();
		}
	}

	for(auto& obstacle: obstacles){
		if(obstacle.top->getPos().x + 15 <= playerPosX && !obstacle.passed && state == Play){
			audio.play({{ 600, 900, 50 },
						{ 0,   0,   50 },
						{ 600, 900, 50 }});
			score++;
			obstacle.passed = true;
			scoreDisplay->setScore(score);
			speedX += 1;
		}
	}

	if(state == FallOut && obstacles.empty()){
		if(life == 0){
			exit();
		}else{
			resetPlayer();
		}
	}
}

void Game2::updatePlayer(float delta){
	if(state != FlyIn && state != Play && state != FallOut) return;

	glm::vec2 pos = player->getPos();

	if(state == FlyIn){
		entry += delta / entryDuration;
		float tX = entry;
		float tY = 1.f - pow(std::max(0.0f, 1.f - (entry * 1.1f)), 2);

		glm::vec2 newPos = { playerPosX * tX, -20 + (StartingPlayerPosY + 20) * tY };
		player->setPos(newPos);

		pos.x -= entry * playerPosX;
		glm::vec2 dir = newPos - pos;
		float angle = glm::orientedAngle(glm::normalize(dir), glm::vec2{ 1, 0 });
		player->setRot(-glm::degrees(angle));

		newPos.x = playerPosX;
		player->setPos(newPos);

		if(entry >= 1){
			player->setPos({ playerPosX, StartingPlayerPosY });
			player->setRot(0);
			velocity.y = 0;
			state = Wait;
		}

		return;
	}

	velocity += delta * glm::vec2{ 0, gravity };
	pos += delta * velocity;
	pos.x = playerPosX;

	if(pos.y < 0){
		velocity.y = 0;
		pos.y = 0;
	}

	float angle = glm::orientedAngle(glm::normalize(velocity), glm::vec2{ 1, 0 });
	angle = -glm::degrees(angle);

	player->setRot(angle);
	player->setPos(pos);
}

void Game2::updateObstacles(float delta){
	auto move = [delta, this](GameObjPtr& obj){
		glm::vec2 pos = obj->getPos();
		pos.x -= delta * speedX;
		obj->setPos(pos);
	};

	for(auto& obstacle: obstacles){
		move(obstacle.top);
		move(obstacle.bot);
	}
}

void Game2::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;

	const auto& btn = data.btn;
	if(btn == Input::B){
		audio.play(Sound{ Chirp{ 400, 350, 50 }});
		exit();
		return;
	}else if(btn != Input::A) return;

	if(state == Wait || state == FlyIn){
		anim->setLoopMode(GIF::Single);
		state = Play;
	}

	if(state != Play) return;

	anim->reset();
	anim->start();

	velocity.y = -flapSpeedY;
	audio.play({{ 90,  90,  50 },
				{ 0,   0,   50 },
				{ 200, 600, 100 }});
}

void Game2::resetPlayer(){
	player->setPos({ 0, -20 });
	player->setRot(0);
	collision.wallBot(*player, [this](){ die(); });

	anim->setLoopMode(GIF::Infinite);
	velocity.y = 0;
	state = FlyIn;
	entry = 0;
}

void Game2::createObstaclePair(){
	int topi = esp_random() % (sizeof(TopObstacles) / sizeof(TopObstacles[0]));
	int boti = esp_random() % (sizeof(BotObstacles) / sizeof(BotObstacles[0]));

	const auto& topDesc = TopObstacles[topi];
	const auto& botDesc = BotObstacles[boti];

	GameObjPtr topObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(topDesc.path), topDesc.dim),
			std::make_unique<PolygonCC>(topDesc.collision)
	);

	auto botObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(botDesc.path), botDesc.dim),
			std::make_unique<PolygonCC>(botDesc.collision)
	);

	int offsetBoth = esp_random() % 40;

	int offsetTop = (esp_random() % 5) * (esp_random() % 2 == 0 ? 1 : -1);
	int offsetBot = esp_random() % 5;

//	offsetBoth = 0;

	auto gateLowPoint = std::min(128, (128 - botDesc.dim.y) + offsetBoth + offsetBot);
	auto gateHighPoint = std::max(0, offsetBoth + offsetTop + topDesc.dim.y);
	auto gap = gateLowPoint - gateHighPoint;
	if(gap < MinimumGap){
//		if(esp_random() % 2 == 0){
		if(gateHighPoint > (128 - gateLowPoint)){
			offsetTop -= (MinimumGap - gap);
		}else{
			offsetBot += (MinimumGap - gap);
		}
	}

	topObj->setPos({ 128, offsetBoth + offsetTop });
	botObj->setPos({ 128 + 15, (128 - botDesc.dim.y) + offsetBoth + offsetBot });

	collision.addPair(*player, *topObj, [this](){ die(); });
	collision.addPair(*player, *botObj, [this](){ die(); });

	addObject(topObj);
	addObject(botObj);
	obstacles.push_back({ topObj, botObj, false });
}

void Game2::die(){
	if(state != Play) return;

	life--;
	if(life > 0){
		audio.play({{ 60,  60, 50 },
					{ 0,   0,  50 },
					{ 300, 60, 150 }});
	}else{
		audio.play({{ 1000, 150, 1000 },
					{ 0,    0,   50 },
					{ 70,   70,  200 }});
	}
	hearts->setLives(life);

	state = FallOut;

	collision.wallBot(*player, {});

	for(const auto& obstacle: obstacles){
		collision.removePair(*player, *obstacle.top);
		collision.removePair(*player, *obstacle.bot);
	}
}

Stats Game2::returnStats(){
	float success = (float) std::min((int) ScoreCutoff, score) / (float) (ScoreCutoff);
	return Stats({ (uint8_t) (25.0f * success), (uint8_t) (20.0f * success), (uint16_t) (200.0f * success) });
}
