#ifndef CIRCUITPET_FIRMWARE_STATSSPRITE_H
#define CIRCUITPET_FIRMWARE_STATSSPRITE_H

#include "StatSprite.h"
#include "Services/StatsManager.h"

class StatsSprite : public LVObject {
public:
	StatsSprite(lv_obj_t* parent, uint8_t oil, uint8_t happ, uint8_t batt, uint8_t level);

	void setOil(uint8_t perc);
	void setHappiness(uint8_t perc);
	void setBattery(uint8_t perc);
	void setLevel(uint8_t level);

private:
	StatSprite* happ;
	StatSprite* oil;
	StatSprite* batt;
	lv_obj_t* level;

};


#endif //CIRCUITPET_FIRMWARE_STATSSPRITE_H
