#include "StatsSprite.h"
#include "Util/Services.h"

StatsSprite::StatsSprite(lv_obj_t* parent, uint8_t oilLevel, uint8_t happiness, uint8_t battery) :
		LVObject(parent), statsManager((StatsManager*) Services.get(Service::Stats)){

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

	lv_obj_set_style_pad_gap(*this, 1, 0);
	this->oilLevel = new StatSprite(*this, StatSprite::OilLevel, oilLevel);
	this->happiness = new StatSprite(*this, StatSprite::Happiness, happiness);
	this->battery = new StatSprite(*this, StatSprite::Battery, battery);
	this->xpLevel = new StatSprite(*this, StatSprite::XPLevel, statsManager->getExpPercentage(), true);


	lv_obj_set_size(*this, 128, LV_SIZE_CONTENT);
}

void StatsSprite::setHappiness(uint8_t level){
	happiness->setLevel(level);
}

void StatsSprite::setOilLevel(uint8_t level){
	oilLevel->setLevel(level);
}

void StatsSprite::setBattery(uint8_t level){
	battery->setLevel(level);
}

void StatsSprite::setXPLevel(){
	xpLevel->setLevel(statsManager->getExpPercentage());
}
