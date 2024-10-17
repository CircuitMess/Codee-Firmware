#ifndef BIT_FIRMWARE_GAME_H
#define BIT_FIRMWARE_GAME_H

#include "ResourceManager.h"
#include "GameObject.h"
#include <set>
#include <vector>
#include "GameSystem.h"
#include "Collision/CollisionSystem.h"
#include "Rendering/RenderSystem.h"
#include <Devices/Display.h>
#include <FS/File.h>
#include <Util/Threaded.h>
#include <Util/Events.h>
#include "Devices/Input.h"
#include "Services/ChirpSystem.h"
#include <atomic>
#include "Services/Stats.hpp"

enum class Games : uint8_t {
	Oily, PolarSwim, PingoSnack, PolarJump, Dance, IceBlast, COUNT
};

class Game {

	friend GameSystem;

public:
	virtual ~Game() = default;

	void load();
	bool isLoaded() const;

	void start();
	void stop();

	void loop(uint micros);

	inline Games getType() const{ return gameType; }

	void exit();

protected:
	Game(Sprite& base, Games gameType, const char* root, std::vector<ResDescriptor> resources);

	virtual void onStart();
	virtual void onStop();
	virtual void onLoad();
	virtual void onLoop(float deltaTime);
	virtual void preRender(Sprite& canvas);
	virtual void onRender(Sprite& canvas);

	File getFile(const std::string& path);

	void addObject(const GameObjPtr& obj);
	void addObjects(std::initializer_list<const GameObjPtr> objs);
	void removeObject(const GameObjPtr& obj);
	void removeObjects(std::initializer_list<const GameObjPtr> objs);

	CollisionSystem collision;

	EventQueue inputQueue;
	virtual void handleInput(const Input::Data& data);

	ChirpSystem& audio;

	virtual Stats returnStats() = 0;

	inline static bool exited = false; // yolo
	// Exit is going to get called in the game's onLoop, and when exit is called, the Game object
	// will get deleted. Once onLoop exits (in Game::loop), the object is already deleted. When that
	// happens, the loop function should return immeidatelly after onLoop is done. Since the object
	// is already deleted at that point, we can't store the exited variable inside the Game class.

private:
	const Games gameType;
	Sprite& base;
	ResourceManager resMan;
	const std::vector<ResDescriptor> resources;

	std::atomic_bool loaded = false;
	bool started = false;
	ThreadedClosure loadTask;

	RenderSystem render;

	std::set<GameObjPtr> objects;

	void loadFunc();
};


#endif //BIT_FIRMWARE_GAME_H
