#include "Hearts.h"
#include "Color.h"

Hearts::Hearts(File heart) : heart(heart){
	go = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(PixelDim{ 31, 8 }),
			nullptr
	);

	rc = std::static_pointer_cast<SpriteRC>(go->getRenderComponent());
	rc->setLayer(10);
	sprite = rc->getSprite();

	setLives(3);
}

void Hearts::setLives(uint8_t lives){
	sprite->clear(TFT_TRANSPARENT);

	for(int i = 0; i < lives; i++){
		Display::drawFile(*sprite, heart, i * 10, 0, 9, 8, 1, TFT_TRANSPARENT);
	}
}

std::shared_ptr<GameObject> Hearts::getGO() const{
	return go;
}
