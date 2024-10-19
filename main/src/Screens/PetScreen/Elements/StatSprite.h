#ifndef CIRCUITPET_FIRMWARE_STATSPRITE_H
#define CIRCUITPET_FIRMWARE_STATSPRITE_H

#include "LV_Interface/LVObject.h"

class StatSprite : public LVObject {
public:
	enum Type {
		Happiness, Oil, Battery, XP
	};

	StatSprite(lv_obj_t* parent, Type type, uint8_t perc, bool shortXp = false);
	void set(uint8_t perc, bool anim = true);

private:
	const Type type;

	lv_obj_t* icon;
	lv_obj_t* bar;

	constexpr static uint8_t BarWidth = 23;
	constexpr static uint8_t BarWidthLong = 111;
	constexpr static uint8_t BarHeight = 7;

	static constexpr const char* BarPath = "S:/Stats/Bar.bin";
	static constexpr const char* BarPathLong = "S:/Stats/BarLong.bin";

	static const lv_point_precise_t LinePoints[];
	static const lv_point_precise_t LinePointsLong[];

};


#endif //CIRCUITPET_FIRMWARE_STATSPRITE_H
