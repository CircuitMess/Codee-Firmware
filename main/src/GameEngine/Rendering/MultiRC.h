#ifndef BIT_FIRMWARE_MULTIRC_H
#define BIT_FIRMWARE_MULTIRC_H

#include "RenderComponent.h"

class MultiRC : public RenderComponent {
public:
	MultiRC(std::shared_ptr<RenderComponent> rc);

	void setRC(std::shared_ptr<RenderComponent> rc);

protected:
	void push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const override;

private:
	std::shared_ptr<RenderComponent> rc;
};


#endif //BIT_FIRMWARE_MULTIRC_H
