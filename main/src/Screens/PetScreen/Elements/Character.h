#ifndef CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H
#define CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H

#include "LV_Interface/LVObject.h"
#include "LV_Interface/LVGIF.h"
#include <optional>
#include <string>

class Character : public LVObject {
public:
	Character(lv_obj_t* parent, uint8_t level, bool rusty);
	~Character() override;

	void setLevel(uint8_t level);
	void setRusty(bool rusty);
	void playIdle();

private:
	LVGIF* gif = nullptr;

	struct {
		const char* idle;
		const char* idle_r;
		const char* alt;
		const char* alt_r;
	} static constexpr Paths[3] = {
			{
				.idle = "S:/Pingo/1_idle",
				.idle_r = "S:/Pingo/1R_idle",
				.alt = "S:/Pingo/1_alt",
				.alt_r = "S:/Pingo/1R_alt"
			},
			{
				.idle = "S:/Pingo/2_idle",
				.idle_r = "S:/Pingo/2R_idle",
				.alt = "S:/Pingo/2_alt",
				.alt_r = "S:/Pingo/2R_alt"
			},
			{
					.idle = "S:/Pingo/3_idle",
					.idle_r = "S:/Pingo/3R_idle",
					.alt = "S:/Pingo/3_alt",
					.alt_r = "S:/Pingo/3R_alt"
			},
	};
	static constexpr const char* getAnimPath(uint8_t level, bool rustLevel, bool alt);
	static constexpr uint8_t getAnimIndex(uint8_t level);

	struct QueuedAnim {
		uint8_t level = 0;
		bool rusty = false;
		bool alt = false;
		explicit operator bool() const { return level != 0; }
	} queuedAnim = {};

	void startAnim(); // starts and clears queued anim, or plays anim for current level and rust

	uint8_t level; // [1-6]
	bool rusty;

};


#endif //CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H
