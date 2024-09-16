#include "GameSystem.h"
#include "Game.h"

GameSystem::GameSystem(const Game* game) : game(game){ }

std::set<GameObjPtr> GameSystem::getObjects(){
	return game->objects;
}