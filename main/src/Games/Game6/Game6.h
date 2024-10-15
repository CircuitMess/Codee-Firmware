#ifndef CIRCUITPET_FIRMWARE_GAME6_H
#define CIRCUITPET_FIRMWARE_GAME6_H

#include "GameEngine/Game.h"
#include "Player.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"

class Game6 : public Game {
public:
	explicit Game6(Sprite& base);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	//Stats returnStats() override;

	inline virtual uint32_t getXP() const override { return 0; }

private:
	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;
	int score = 0;
	constexpr static int maxScore = 52;

	enum {
		Intro, Running, DeathAnim, DeathPause, Win
	} state = Intro;

	float introTimer = 0;
	static constexpr float introTime = 1.5f;
	float deathTimer = 0;
	static constexpr float deathPauseTime = 3.f;
	float winTimer = 0;
	static constexpr float winTime = 3.f;
	static constexpr float winAcceleration = 40.f;

	static constexpr glm::vec2 startPosition = { 55, 42 };

	uint8_t level = 0;
	void nextLevel();
	void gameOver();
	void spawnRandomAsteroid();

	//------------ Player ------------
	Player player;
	uint8_t life = 3;

	void playerHit();
	void updateInvincibility(float delta);
	bool invincible = false;
	float invincibilityTime = 0;
	float invincibilityBlink = 0;
	float invincibilityBlinkDuration = 0.2f;

	static constexpr std::initializer_list<glm::vec2> playerHitbox = {
			{ 2,  32 },
			{ 0,  24 },
			{ 9,  0 },
			{ 18, 24 },
			{ 16, 32 }};

	constexpr static float invincibilityDuration = 2.0f;
	//------------ Player end ------------


	//------------ Bullet ------------
	struct Bullet {
		std::shared_ptr<GameObject> gObj;
		glm::vec2 velocity;

		bool operator==(const Bullet& other){
			return (velocity == other.velocity) && (gObj == other.gObj);
		}
	};

	constexpr static float bulletSpeed = 80.0f;
	constexpr static uint8_t maxBullets = 3;

	std::vector<Bullet> bulletPool;
	void updateBullets(float deltaTime);
	void shootBullet();
	//------------ Bullet end ------------

	//------------ Asteroid ------------
	enum class AsteroidSize : uint8_t {
		Small, Medium, Large
	};

	struct ImageDesc {
		const char* path;
		PixelDim dim;
	};
	static constexpr ImageDesc iceIcons[] = {{ "/ice-1.raw", { 23, 14 }},
		{ "/ice-3.raw", { 32, 23 }},
		{ "/ice-2.raw", { 39, 25 }}};

	struct Asteroid {
		std::shared_ptr<GameObject> gObj;
		glm::vec2 velocity;
		AsteroidSize size;

		bool operator==(const Asteroid& other) const {
			return (velocity == other.velocity) && (gObj == other.gObj) && (size == other.size);
		}
	};

	constexpr static std::array<float, 3> asteroidSpeed = { 25.0f, 20.0f, 12.0f };
	constexpr static std::array<float, 3> asteroidRadius = { 7.5f, 10.0f, 15.0f };
	std::vector<Asteroid> asteroidPool;

	void asteroidHit(const Asteroid& asteroid);
	void createAsteroid(AsteroidSize size, glm::vec2 pos);
	void updateAsteroids(float deltaTime);

	struct {
		GameObject top;
		GameObject bot;
		GameObject left;
		GameObject right;
	} wrapWalls;
	constexpr static glm::vec2 wrapWallsSize = { 128.0f + 4 * asteroidRadius[(uint8_t)AsteroidSize::Large] + 2,
												 128.0f + 4 * asteroidRadius[(uint8_t)AsteroidSize::Large] + 2 };
	//------------ Asteroid end ------------


	bool leftHold = false;
	bool rightHold = false;
};

#endif //CIRCUITPET_FIRMWARE_GAME6_H
