#ifndef CIRCUITPET_FIRMWARE_OILCAN_H
#define CIRCUITPET_FIRMWARE_OILCAN_H

#include "Devices/Display.h"
#include "GameEngine/GameObject.h"


class OilCan {
public:
	OilCan(std::shared_ptr<Sprite> sprite, File fullCan, File emptyCan);
	void fill(float fillPercent);
	bool move(float deltaTime);
	void setGameObject(std::shared_ptr<GameObject> gameObj);
	void startMoving();

private:
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<GameObject> gameObject;
	File fullCan;
	File emptyCan;
	PixelDim startPos;

	bool moving = false;

	const float speed = 80.0f;
	const PixelDim endPos = { 76, 47 };
};


#endif //CIRCUITPET_FIRMWARE_OILCAN_H
