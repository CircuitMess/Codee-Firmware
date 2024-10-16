#include "OSIcon.h"
#include <string>

OSIcon::OSIcon(lv_obj_t* parent, uint8_t level) : LVObject(parent){

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(*this, 1, 0);

	logo = lv_image_create(*this);
	lv_image_set_src(logo, "S:/OS/Logo.bin");

	version = lv_image_create(*this);
	setLevel(level);

	lv_obj_set_size(*this, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
}

void OSIcon::setLevel(uint8_t level){
	std::string path = "S:/OS/Level" + std::to_string(level) + ".bin";
	lv_image_set_src(version, path.c_str());
}
