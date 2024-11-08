#ifndef BIT_FIRMWARE_HEARTS_H
#define BIT_FIRMWARE_HEARTS_H

#include <memory>
#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "FS/File.h"

class Hearts {
public:
	Hearts(File heart);

	std::shared_ptr<GameObject> getGO() const;
	void setLives(uint8_t lives);

private:
	File heart;

	std::shared_ptr<GameObject> go;
	std::shared_ptr<SpriteRC> rc;
	std::shared_ptr<Sprite> sprite;
};


#endif //BIT_FIRMWARE_HEARTS_H
