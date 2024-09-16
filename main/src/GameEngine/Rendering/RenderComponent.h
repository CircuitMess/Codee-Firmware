#ifndef BIT_FIRMWARE_RENDERCOMPONENT_H
#define BIT_FIRMWARE_RENDERCOMPONENT_H

#include "Devices/Display.h"
#include "../PixelDim.hpp"

class RenderSystem;

class MultiRC;

class RenderComponent {
	friend RenderSystem;
	friend MultiRC;

public:
	virtual ~RenderComponent() = default;
	int8_t getLayer() const;
	void setLayer(int8_t layer);

	bool isVisible() const;
	void setVisible(bool visible);
	void setFlipX(bool flipX);
	void setFlipY(bool flipY);

protected:
	/**
	 * Update is used for animated RenderComponents
	 * @param deltaMicros Time delta since last update.
	 */
	virtual void update(uint32_t deltaMicros);
	//TODO - apply flip transformation for all RenderComponent types
	virtual void push(Sprite& parent, PixelDim pos, float rot, bool flipX = false, bool flipY = false) const = 0;

private:
	bool visible = true;
	int8_t layer = 0;
	bool flipX = false;
	bool flipY = false;
};


#endif //BIT_FIRMWARE_RENDERCOMPONENT_H
