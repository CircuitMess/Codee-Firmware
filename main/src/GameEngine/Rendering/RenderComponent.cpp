#include "RenderComponent.h"

int8_t RenderComponent::getLayer() const{
	return layer;
}

void RenderComponent::setLayer(int8_t layer){
	RenderComponent::layer = layer;
}

bool RenderComponent::isVisible() const{
	return visible;
}

void RenderComponent::setVisible(bool visible){
	RenderComponent::visible = visible;
}

void RenderComponent::update(uint32_t deltaMicros){

}

void RenderComponent::setFlipX(bool flipX){
	RenderComponent::flipX = flipX;
}

void RenderComponent::setFlipY(bool flipY){
	RenderComponent::flipY = flipY;
}
