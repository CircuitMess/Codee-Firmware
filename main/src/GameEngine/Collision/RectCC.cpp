#include "RectCC.h"

RectCC::RectCC(glm::vec2 dim, glm::vec2 offset) : CollisionComponent(CollisionType::Rect), dim(dim), offset(offset){

}

glm::vec2 RectCC::getDim() const{
	return dim;
}

glm::vec2 RectCC::getOffset() const{
	return offset;
}

void RectCC::setOffset(const glm::vec2& offset){
	this->offset = offset;
}
