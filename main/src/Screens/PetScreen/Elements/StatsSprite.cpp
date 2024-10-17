#include "StatsSprite.h"
#include "Util/Services.h"

static constexpr const char* LevelPaths[] = {
		"S:/OS/1_0.bin",
		"S:/OS/One.bin",
		"S:/OS/10.bin",
		"S:/OS/X.bin",
		"S:/OS/9000.bin",
		"S:/OS/9001.bin"
};

StatsSprite::StatsSprite(lv_obj_t* parent, uint8_t oil, uint8_t happ, uint8_t batt, uint8_t level) : LVObject(parent){
	lv_obj_set_size(*this, 128, LV_SIZE_CONTENT);
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(*this, 2, 0);

	this->oil = new StatSprite(*this, StatSprite::Oil, oil);
	this->happ = new StatSprite(*this, StatSprite::Happiness, happ);
	this->batt = new StatSprite(*this, StatSprite::Battery, batt);

	this->level = lv_image_create(*this);
	lv_obj_add_flag(this->level, LV_OBJ_FLAG_IGNORE_LAYOUT);
	lv_obj_align(this->level, LV_ALIGN_RIGHT_MID, 0, 0);
	setLevel(level);
}

void StatsSprite::setHappiness(uint8_t perc){
	happ->set(perc);
}

void StatsSprite::setOil(uint8_t perc){
	oil->set(perc);
}

void StatsSprite::setBattery(uint8_t perc){
	batt->set(perc);
}

void StatsSprite::setLevel(uint8_t level){
	level = std::clamp((int) level, 1, 6) - 1;
	lv_image_set_src(this->level, LevelPaths[level]);
}
