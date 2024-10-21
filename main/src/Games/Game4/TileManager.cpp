#include "TileManager.h"
#include "../../GameEngine/Rendering/SpriteRC.h"
#include "../../GameEngine/Collision/RectCC.h"
#include <esp_random.h>

#include <utility>

Game4::TileManager::TileManager(std::vector<std::shared_ptr<GameObject>>& movingObjects) : movingObjects(movingObjects){
}


void Game4::TileManager::addFilePair(File topFile, File botFile){
	addTopFile(std::move(topFile));
	addBotFile(std::move(botFile));
}

void Game4::TileManager::create(){
	for(int iArray = 0; iArray < arrays; iArray++){

		auto setOfTiles = std::make_shared<GameObject>(
				std::make_unique<SpriteRC>(PixelDim{ tileDim * tilesPerArray, tileDim * 2 }),
				std::make_unique<RectCC>(glm::vec2{ tileDim, tileDim })
		);
		movingObjects.push_back(setOfTiles);
		drawTiles(iArray);
		setOfTiles->setPos({ iArray * tilesPerArray * tileDim, topY });
		setOfTiles->getRenderComponent()->setLayer(-1);
	}

}

void Game4::TileManager::reset(int objectIndex){
	int maxX = 0;
	for(auto& obj: movingObjects){
		if(obj->getPos().x > maxX){
			maxX = obj->getPos().x;
		}
	}
	movingObjects[objectIndex]->setPos({ maxX + tilesPerArray * tileDim, topY });
}

void Game4::TileManager::drawTiles(int objectIndex){
	auto sprite = std::static_pointer_cast<SpriteRC>(movingObjects[objectIndex]->getRenderComponent())->getSprite();

	sprite->clear(TFT_TRANSPARENT);
	for(int iTile = 0; iTile < tilesPerArray; iTile++){
		int randTop = esp_random() % topFiles.size();
		Display::drawFile(*sprite, topFiles[randTop], iTile * tileDim, 0, tileDim, tileDim);
		int randBot = esp_random() % botFiles.size();
		Display::drawFile(*sprite, botFiles[randBot], iTile * tileDim, tileDim, tileDim, tileDim);
	}
}

void Game4::TileManager::addTopFile(File topFile){
	topFiles.push_back(topFile);

}

void Game4::TileManager::addBotFile(File botFile){
	botFiles.push_back(botFile);
}
