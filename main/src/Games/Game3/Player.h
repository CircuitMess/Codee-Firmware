#ifndef CIRCUITPET_FIRMWARE_DUCK_H
#define CIRCUITPET_FIRMWARE_DUCK_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Devices/Input.h"

namespace Game3 {
class Game3;

class Player {
public:
	Player(File walk, File eat, File eatBad);
	~Player();
	std::shared_ptr<GameObject> getGameObject();
	void loop(float deltaTime);
	void startEating(int value);
	void filled(Game3* game);
	void killed(Game3* game);

	bool isEatingBad() const;
	void btnPressed(Input::Button btn);
	void btnReleased(Input::Button btn);

private:
	void finishEating();
	const uint16_t width = 55;
	const uint16_t height = 89;
	float velocity = 0.0f;
	const float speed = 60.0f;
	const float collisionRadius = 12;

	const glm::vec2 collisionOffset = glm::vec2{ 27, 29 };
	bool eating = false;
	bool eatingBad = false;
	bool eatDone = false;
	Input::Button lastPressed = Input::D;

	File walk;
	File eat;
	File eatBad;

	std::shared_ptr<GameObject> go;
	std::shared_ptr<AnimRC> anim;
};

}
#endif //CIRCUITPET_FIRMWARE_DUCK_H
