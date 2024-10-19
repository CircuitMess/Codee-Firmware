#ifndef BIT_FIRMWARE_SCORE_H
#define BIT_FIRMWARE_SCORE_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "FS/File.h"

class Score {
public:
	explicit Score(File icon);

	std::shared_ptr<GameObject> getGO() const;
	void setScore(uint16_t score);

private:
	File icon;

	std::shared_ptr<GameObject> go;
	std::shared_ptr<SpriteRC> rc;
	std::shared_ptr<Sprite> sprite;
};

#endif //BIT_FIRMWARE_SCORE_H
