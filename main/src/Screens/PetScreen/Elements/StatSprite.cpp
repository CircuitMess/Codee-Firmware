#include "StatSprite.h"
#include <unordered_map>
#include <cmath>
#include <algorithm>

static std::unordered_map<StatSprite::Type, const char*> Paths = {
		{ StatSprite::Type::Happiness, "S:/Stats/Happ.bin" },
		{ StatSprite::Type::Battery,   "S:/Stats/Batt.bin" },
		{ StatSprite::Type::Oil,       "S:/Stats/Oil.bin" },
		{ StatSprite::Type::XP,        "S:/Stats/Xp.bin" },
};

const lv_point_precise_t StatSprite::LinePoints[] = { { 0, 0 }, { StatSprite::BarWidth - 4, 0 } };
const lv_point_precise_t StatSprite::LinePointsLong[] = { { 0, 0 }, { StatSprite::BarWidthLong - 4, 0 } };

StatSprite::StatSprite(lv_obj_t* parent, Type type, uint8_t perc) : LVObject(parent), type(type){
	lv_obj_set_size(*this, LV_SIZE_CONTENT, this->type == XP ? 7 : 10);

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_base_dir(*this, LV_BASE_DIR_RTL, 0);


	bar = lv_bar_create(*this);
	lv_obj_remove_style_all(bar);
	lv_bar_set_range(bar, 0, 100);
	lv_obj_set_style_base_dir(bar, LV_BASE_DIR_LTR, 0);

	if(type == XP){
		lv_obj_set_style_bg_image_src(bar, BarPathLong, LV_PART_MAIN);
		lv_obj_set_size(bar, BarWidthLong, BarHeight);
		lv_obj_set_style_size(bar, BarWidthLong-4, BarHeight-4, LV_PART_INDICATOR);

		lv_obj_set_style_pad_gap(*this, 1, 0);
	}else{
		lv_obj_set_style_bg_image_src(bar, BarPath, LV_PART_MAIN);
		lv_obj_set_size(bar, BarWidth, BarHeight);
		lv_obj_set_style_size(bar, BarWidth-4, BarHeight - 4, LV_PART_INDICATOR);

		lv_obj_set_style_translate_x(bar, -2, 0);
		lv_obj_set_style_margin_top(bar, 2, 0);
	}

	lv_obj_set_style_radius(bar, 1, LV_PART_INDICATOR);
	lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
	lv_obj_set_style_border_width(bar, 0, LV_PART_INDICATOR);
	lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_VER, LV_PART_INDICATOR);

	lv_obj_set_style_pad_left(bar, 2, 0);
	lv_obj_set_style_pad_right(bar, 3, 0);
	lv_obj_set_style_pad_ver(bar, 2, 0);


	const auto mkLine = [this](){
		lv_obj_t* line = lv_line_create(*this);

		lv_line_set_points(line, this->type == XP ? LinePointsLong : LinePoints, 2);
		lv_obj_add_flag(line, LV_OBJ_FLAG_FLOATING);
		lv_obj_set_size(line, (this->type == XP ? BarWidthLong : BarWidth) - 4, 1);
		lv_obj_set_style_line_width(line, 1, 0);
		lv_obj_set_style_line_color(line, lv_color_black(), 0);

		return line;
	};

	lv_obj_t* l1 = mkLine();
	lv_obj_t* l2 = mkLine();

	if(this->type == XP){
		lv_obj_align(l1, LV_ALIGN_TOP_RIGHT, -2, 1);
		lv_obj_align(l2, LV_ALIGN_TOP_RIGHT, -2, 1 + 4);
	}else{
		lv_obj_align(l1, LV_ALIGN_TOP_RIGHT, -4, 3);
		lv_obj_align(l2, LV_ALIGN_TOP_RIGHT, -4, 3 + 4);
	}


	icon = lv_image_create(*this);
	lv_image_set_src(icon, Paths[type]);


	set(perc, false);
}

void StatSprite::set(uint8_t perc, bool anim){
	perc = std::clamp((int) perc, 0, 100);

	const uint16_t hue = type == XP ? 277 : std::round((float) perc / 100.0f * 60.0f / 255.0f * 360.0f);

	lv_color_t color = lv_color_hsv_to_rgb(hue, 100, 100);
	lv_obj_set_style_bg_color(bar, color, LV_PART_INDICATOR);

	lv_color_t botColor = lv_color_hsv_to_rgb(hue, 100, 65);
	lv_obj_set_style_bg_grad_color(bar, botColor, LV_PART_INDICATOR);

	lv_bar_set_value(bar, perc, anim ? LV_ANIM_ON : LV_ANIM_OFF);
}
