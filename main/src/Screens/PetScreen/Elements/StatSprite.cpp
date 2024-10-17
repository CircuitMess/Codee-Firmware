#include <algorithm>
#include "StatSprite.h"
#include "Util/stdafx.h"

StatSprite::StatSprite(lv_obj_t* parent, Type type, uint8_t level, bool longBar) : LVObject(parent), type(type), level(level), longBar(longBar){

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	icon = lv_image_create(*this);
	lv_image_set_src(icon, paths[type]);

	bar = lv_bar_create(*this);
	if(longBar){
		lv_obj_set_style_bg_image_src(bar, LongbarPath, LV_PART_MAIN);
		lv_bar_set_range(bar, 0, longBarWidth - 4);
		lv_obj_set_size(bar, longBarWidth, barHeight);
	}else{
		lv_obj_set_style_bg_image_src(bar, BarPath, LV_PART_MAIN);
		lv_bar_set_range(bar, 0, barWidth - 4);
		lv_obj_set_size(bar, barWidth, barHeight);
	}
	lv_obj_set_style_radius(bar, 1, LV_PART_INDICATOR);
	lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
	lv_obj_set_style_pad_top(bar, 2, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(bar, 1, LV_PART_MAIN);
	lv_obj_set_style_pad_hor(bar, 2, LV_PART_MAIN);


	lv_obj_set_size(*this, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

	setLevel(level);
}

void StatSprite::setLevel(uint8_t _level, bool anim){
	level = std::clamp(_level, (uint8_t) 0, (uint8_t) 100);
	float hue;
	if(type == XPLevel){
		hue = 276.9;
	}else{
		hue = (float) level / 100.0 * 60.0 / 255.0 * 360;
	}

	lv_color_t color = lv_color_hsv_to_rgb((uint16_t) hue, 100, 100);
	lv_color_t botColor = lv_color_hsv_to_rgb((uint16_t) hue, 100, 65);

	lv_obj_set_style_bg_grad_color(bar, botColor, LV_PART_INDICATOR);
	lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_VER, LV_PART_INDICATOR);

	lv_obj_set_style_bg_color(bar, color, LV_PART_INDICATOR);


	int width = map(level, 0, 100, 0, (longBar ? longBarWidth : barWidth) - 4); //-4 pixels from the edge bar

	lv_bar_set_value(bar, width, anim ? LV_ANIM_ON : LV_ANIM_OFF);
}
