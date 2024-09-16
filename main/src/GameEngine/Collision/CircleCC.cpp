#include "CircleCC.h"

CircleCC::CircleCC(float radius, glm::vec2 offset) : CollisionComponent(CollisionType::Circle), radius(radius), offset(offset){

}

float CircleCC::getRadius() const{
	return radius;
}

glm::vec2 CircleCC::getOffset() const{
	return offset;
}

void CircleCC::setOffset(const glm::vec2& offset){
	CircleCC::offset = offset;
}
