#ifndef BIT_FIRMWARE_FLAPPY_H
#define BIT_FIRMWARE_FLAPPY_H

#include "GameEngine/Game.h"
#include "GameEngine/Rendering/AnimRC.h"
#include <deque>
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"

class Game2 : public Game {
public:
	Game2(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void onStart() override;
	void onStop() override;
	Stats returnStats() override;

private:
	static constexpr float gravity = 150.0f;
	float speedX = 60.0f;
	static constexpr float flapSpeedY = 80.0f;
	static constexpr float playerPosX = 15;
	static constexpr float StartingPlayerPosY = 50;
	static constexpr float obstacleSpacing = 125;
	static constexpr uint8_t MinimumGap = 65;

	static constexpr uint8_t ScoreCutoff = 60;

	enum {
		FlyIn, Wait, Play, FallOut
	} state = FlyIn;

	float entry = 0;
	float entryDuration = 1.0f;

	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;

	uint8_t life = 3;
	void die();

	GameObjPtr player;
	std::shared_ptr<AnimRC> anim;
	glm::vec2 velocity = { speedX, 0 };

	int score = 0;

	struct ObstaclePair {
		GameObjPtr top;
		GameObjPtr bot;
		bool passed;
	};
	std::deque<ObstaclePair> obstacles;

	void updatePlayer(float delta);
	void updateObstacles(float delta);

	void createObstaclePair();
	void resetPlayer();

	void handleInput(const Input::Data& data) override;

	struct ObstacleDesc {
		const char* path;
		PixelDim dim;
		std::initializer_list<glm::vec2> collision;
	};

	static constexpr ObstacleDesc TopObstacles[] = {
			//orca
			{ "up1.raw", { 80, 47 }, {{ 0, 28 }, { 35, 0 }, { 79, 42 }, { 25, 46 }}},
			//shark
			{ "up2.raw", { 70, 30 }, {{ 0, 14 }, { 32, 0 }, { 69, 5 }, { 69, 28 }, { 16, 29 }}},
	};

	static constexpr ObstacleDesc BotObstacles[] = {
			//bomb
			{ "down1.raw", { 43, 117 }, {{ 22, 116 }, { 0, 22 }, { 21, 1 }, { 42, 23 }}},
			//plant-1
			{ "down2.raw", { 52, 87 }, {{ 21, 86 }, { 0, 55 }, { 2, 29 }, { 5, 15 }, { 29, 3 }, { 40, 0 }, { 51, 36 }, { 35, 86 }}},
			//plant-2
			{ "down3.raw", { 53, 55 }, {{ 3, 54 }, { 0, 20 }, { 6, 5 }, { 34, 0 }, { 52, 33 }, { 44, 54 }}},
			//plant-3
			{ "down4.raw", { 45, 52 }, {{ 11, 51 }, { 0, 18 }, { 22, 0 }, { 44, 12 }, { 38, 51 }}},
			//plant-4
			{ "down5.raw", { 55, 68 }, {{ 13, 67 }, { 0, 25 }, { 26, 0 }, { 54, 31 }, { 35, 67 }}},
			//plant-5
			{ "down6.raw", { 43, 42 }, {{ 26, 41 }, { 0, 20 }, { 13, 0 }, { 42, 14 }}},
	};
};


#endif //BIT_FIRMWARE_FLAPPY_H
