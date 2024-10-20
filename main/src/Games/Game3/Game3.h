#ifndef CIRCUITPET_FIRMWARE_GAME3_H
#define CIRCUITPET_FIRMWARE_GAME3_H

#include "GameEngine/Game.h"
#include "Player.h"
#include "../Common/Hearts.h"

namespace Game3 {

class Player;

class Game3 : public Game {
	friend Player;
public:
	Game3(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;
	Stats returnStats() override;

private:
	void addTemplate(std::string file, PixelDim dim, int value);
	void spawnRandom();
	void drawBar();

	struct Item {
		GameObjPtr go;
		int value;
	};

	struct Template {
		std::string path;
		PixelDim dim;
		int value;
	};

	void spawnItem(Template temp);
	void collisionHandler(Item item);

	const int hungerMeterMax = 200;
	int hungerMeter = 0;
	int lives = 3;
	const int speedMin = 55;
	const int speedMax = 70;
	const float spawnInterval = 0.8f;
	float timeToSpawn = 0.0f;

	//3 food spawned for each bomb spawned
	static constexpr uint8_t FoodInSet = 3;
	static constexpr uint8_t BombInSet = 1;
	uint8_t foodCounter = 0;
	uint8_t bombCounter = 0;

	enum State {
		Running, Dead, DeadPause, Win, WinPause
	} state = Running;
	static constexpr float DonePause = 1.5f;
	float donePauseCounter = 0;

	std::unique_ptr<Player> player;

	std::shared_ptr<Hearts> hearts;
	std::shared_ptr<Sprite> hungerBar;
	GameObjPtr bg;
	GameObjPtr collectorBot;
	std::map<GameObjPtr, float> movingObjects;
	std::vector<Template> foods;
	std::vector<Template> bombs;
};

}
#endif //CIRCUITPET_FIRMWARE_GAME3_H
