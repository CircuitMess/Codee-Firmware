#include "StaticRC.h"
#include <utility>
#include <esp_log.h>
#include "Color.h"

StaticRC::StaticRC(File file, PixelDim dim, glm::vec2 scale) : file(std::move(file)), dim(dim), scale(scale){}

void StaticRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	if(!file){
		ESP_LOGE("Render", "StaticRC: Pushing closed file");
		return;
	}

	if(rot == 0 && scale.x == 1.0f && scale.y == 1.0f){
		Display::drawFile(parent, file, std::round(pos.x), std::round(pos.y), dim.x, dim.y);
	}else{
		Sprite rotated = Sprite(&parent);
		rotated.createSprite(dim.x, dim.y);
		rotated.clear(TFT_TRANSPARENT);
		Display::drawFile(rotated, file, 0, 0, dim.x, dim.y);

		rotated.pushRotateZoomWithAA(std::round(pos.x + (float) dim.x * 0.5f * scale.x), std::round(pos.y + (float) dim.y * 0.5f * scale.y), rot, scale.x, scale.y, TFT_TRANSPARENT);
	}
}

void StaticRC::setFile(File file){
	StaticRC::file = file;
}

void StaticRC::setFile(File file, PixelDim dim){
	StaticRC::file = file;
	StaticRC::dim = dim;
}

void StaticRC::setScale(glm::vec2 value){
	scale = value;
}
