#ifndef BIT_FIRMWARE_RENDERSYSTEM_H
#define BIT_FIRMWARE_RENDERSYSTEM_H

#include "Devices/Display.h"
#include "../GameSystem.h"

class RenderSystem : public GameSystem {
public:
	RenderSystem(const Game* game, Sprite& canvas);

	void update(uint32_t deltaMicros) override;

private:
	Sprite& canvas;
};


#endif //BIT_FIRMWARE_RENDERSYSTEM_H
