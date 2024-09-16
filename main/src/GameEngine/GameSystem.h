#ifndef BIT_FIRMWARE_GAMESYSTEM_H
#define BIT_FIRMWARE_GAMESYSTEM_H

#include "GameObject.h"
#include <vector>
#include <set>

class Game;

class GameSystem {
public:
	virtual void update(uint32_t deltaMicros) = 0;

protected:
	GameSystem(const Game* game);

	std::set<GameObjPtr> getObjects();

private:
	const Game* game;

};


#endif //BIT_FIRMWARE_GAMESYSTEM_H
