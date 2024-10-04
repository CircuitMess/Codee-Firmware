#ifndef CIRCUITPET_FIRMWARE_STATSSPRITE_H
#define CIRCUITPET_FIRMWARE_STATSSPRITE_H

#include "StatSprite.h"
#include "Services/StatsManager.h"

class StatsSprite : public LVObject {
public:
	StatsSprite(lv_obj_t* parent, uint8_t oilLevel, uint8_t happiness, uint8_t battery);

	void setOilLevel(uint8_t level);
	void setHappiness(uint8_t level);
	void setBattery(uint8_t level);
	void setXPLevel();

private:
	StatSprite* happiness;
	StatSprite* oilLevel;
	StatSprite* battery;
	StatSprite* xpLevel;

	StatsManager* statsManager = nullptr;
	constexpr static uint8_t spacing = 33;
};


#endif //CIRCUITPET_FIRMWARE_STATSSPRITE_H
