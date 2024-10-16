#include "GrayscaleImageElement.h"

GrayscaleImageElement::GrayscaleImageElement(lv_obj_t* parent, const std::string& path, const std::string& pathGrayscale, bool grayscale) :
		LVObject(parent), path(path), pathGrayscale(pathGrayscale), grayscale(grayscale){
	lv_obj_set_size(*this, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	img = lv_img_create(*this);
	setGrayscale(grayscale);
}

bool GrayscaleImageElement::isGrayscale() const{
	return grayscale;
}

void GrayscaleImageElement::setGrayscale(bool value){
	grayscale = value;

	if(grayscale){
		lv_img_set_src(img, pathGrayscale.c_str());
	}else{
		lv_img_set_src(img, path.c_str());
	}
}
