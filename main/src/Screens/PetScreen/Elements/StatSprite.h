#ifndef CIRCUITPET_FIRMWARE_STATSPRITE_H
#define CIRCUITPET_FIRMWARE_STATSPRITE_H

#include "LV_Interface/LVObject.h"

class StatSprite : public LVObject {
public:
	enum Type {
		Happiness, OilLevel, Battery, XPLevel
	};
	StatSprite(lv_obj_t* parent, Type type, uint8_t level, bool longBar = false);
	void setLevel(uint8_t level, bool anim = true);

private:
	Type type;
	uint8_t level;

	lv_obj_t* icon;
	lv_obj_t* bar;
	bool longBar;

	constexpr static uint8_t iconWidth = 15;
	constexpr static uint8_t XPiconWidth = 19;
	constexpr static uint8_t iconHeight = 10;
	constexpr static uint8_t barWidth = 24;
	constexpr static uint8_t longBarWidth = 97;
	constexpr static uint8_t barHeight = 7;
	static constexpr const char* paths[] = {
			"S:/Stats/Happiness.bin",
			"S:/Stats/OilLevel.bin",
			"S:/Stats/Battery.bin",
			"S:/Stats/Exp.bin"
	};

	static constexpr const char* BarPath = "S:/Stats/bar.bin";
	static constexpr const char* LongbarPath = "S:/Stats/LongBar.bin";

	void drawLevel(uint8_t length);
};


#endif //CIRCUITPET_FIRMWARE_STATSPRITE_H
