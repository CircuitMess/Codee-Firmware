#ifndef BIT_FIRMWARE_RECTCC_H
#define BIT_FIRMWARE_RECTCC_H

#include "CollisionComponent.h"
#include "../PixelDim.hpp"

class RectCC : public CollisionComponent {
public:
	RectCC(glm::vec2 dim, glm::vec2 offset = { 0, 0 });
	~RectCC() override = default;

	glm::vec2 getDim() const;
	glm::vec2 getOffset() const;
	void setOffset(const glm::vec2& offset);

private:
	glm::vec2 dim;
	glm::vec2 offset;
};


#endif //BIT_FIRMWARE_RECTCC_H
