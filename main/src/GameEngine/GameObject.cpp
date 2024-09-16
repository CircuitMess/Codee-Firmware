#include "GameObject.h"

GameObject::GameObject(std::unique_ptr<RenderComponent> rc, std::unique_ptr<CollisionComponent> cc) : renderComponent(std::move(rc)),
																									  collisionComponent(std::move(cc)){
}

glm::vec2 GameObject::getPos() const{
	return pos;
}

void GameObject::setPos(glm::vec2 pos){
	this->pos = pos;
}

void GameObject::setPos(float x, float y){
	this->pos = { x, y };
}

void GameObject::setPosX(float x){
	pos.x = x;
}

void GameObject::setPosY(float y){
	pos.y = y;
}

std::shared_ptr<RenderComponent> GameObject::getRenderComponent() const{
	return renderComponent;
}

std::shared_ptr<CollisionComponent> GameObject::getCollisionComponent() const{
	return collisionComponent;
}

void GameObject::setCollisionComponent(std::shared_ptr<CollisionComponent> collisionComponent){
	GameObject::collisionComponent = collisionComponent;
}

float GameObject::getRot() const{
	return rot;
}

void GameObject::setRot(float rot){
	this->rot = rot;
}
