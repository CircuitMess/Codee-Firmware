#include "MultiRC.h"

MultiRC::MultiRC(std::shared_ptr<RenderComponent> rc) : rc(rc){

}

void MultiRC::setRC(std::shared_ptr<RenderComponent> rc){
	this->rc = std::move(rc);
}

void MultiRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	if(rc){
		rc->push(parent, pos, rot, flipX, flipY);
	}
}
